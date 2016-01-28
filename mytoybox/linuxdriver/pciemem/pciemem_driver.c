/*
 * Linux 2.6 and later
 * copy PCIe configuration space memory to client in user space
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/types.h>
#include <linux/mutex.h>
#include <linux/kfifo.h>
#include "pciemem_driver.h"
#include <linux/version.h>
#include <linux/fs.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>
#include <linux/ioctl.h>
#include <linux/time.h>
#include <linux/rtc.h>
#include <linux/pci.h>

/* Module information */
MODULE_AUTHOR(AUTHOR);
MODULE_DESCRIPTION(DESCRIPTION);
MODULE_VERSION(VERSION);
MODULE_LICENSE("GPL");

/* Device variables */
static struct class* pciemem_class = NULL;
static struct device* pciemem_device = NULL;
static int pciemem_major;
/* Flag used with the one_shot mode */
static bool message_read;
/* A mutex will ensure that only one process accesses our device */
static DEFINE_MUTEX(pciemem_device_mutex);
/* Use a Kernel FIFO for read operations */
static DECLARE_KFIFO(pciemem_msg_fifo, char, MSG_FIFO_SIZE);
/* This table keeps track of each message length in the FIFO */
static unsigned int pciemem_msg_len[MSG_FIFO_MAX];
/* Read and write index for the table above */
static int pciemem_msg_idx_rd, pciemem_msg_idx_wr;

/* Module parameters that can be provided on insmod */
static bool debug = false;	/* insmod pciemem_driver.ko debug=1 */
module_param(debug, bool, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(debug, "enable debug info (default: false)");
static bool one_shot = true;	/* only read a single message after open() */
module_param(one_shot, bool, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(one_shot, "disable the readout of multiple messages at once (default: true)");

char current_time_str_buf[32];
char* current_time_str(char* buf) {
	char* pbuf = buf;
	if(pbuf==NULL) pbuf = current_time_str_buf;
	struct timeval time;
	unsigned long local_time;
	struct rtc_time tm;
	do_gettimeofday(&time);
	local_time = (u32)(time.tv_sec - (sys_tz.tz_minuteswest * 60));
	rtc_time_to_tm(local_time, &tm);
	sprintf(pbuf, "%04d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	return pbuf;
}

struct file* file_open(const char* path, int flags, int rights) {
    struct file* filp = NULL;
    mm_segment_t oldfs;
    int err = 0;

    oldfs = get_fs();
    set_fs(get_ds());
    filp = filp_open(path, flags, rights);
    set_fs(oldfs);
    if(IS_ERR(filp)) {
        err = PTR_ERR(filp);
        return NULL;
    }
    return filp;
}
void file_close(struct file* file) {
    filp_close(file, NULL);
}
int file_read(struct file* file, unsigned long long offset, unsigned char* data, unsigned int size) {
    mm_segment_t oldfs;
    int ret;

    oldfs = get_fs();
    set_fs(get_ds());

    ret = vfs_read(file, data, size, &offset);

    set_fs(oldfs);
    return ret;
}   

int file_write(struct file* file, unsigned long long offset, unsigned char* data, unsigned int size) {
    mm_segment_t oldfs;
    int ret;

    oldfs = get_fs();
    set_fs(get_ds());

    ret = vfs_write(file, data, size, &offset);

    set_fs(oldfs);
    return ret;
}
int file_sync(struct file* file) {
    vfs_fsync(file, 0);
    return 0;
}
int read_pcie_config_space(int bdf, char* buf) {
	struct pci_dev *dev = NULL;
	sprintf(buf, "%s %s bdf=%04x not found", __FUNCTION__, current_time_str(NULL), bdf);
	while (dev = pci_get_device(PCI_ANY_ID /*VENDOR_ID*/, PCI_ANY_ID /* DEVICE_ID*/, dev)) {
		u16 deviceid, vendorid;
		int device_number, function_num, devbdf;
		pci_read_config_word(dev, 0, &vendorid);
		pci_read_config_word(dev, 2, &deviceid);
		device_number= (dev->devfn>>3) & 0x1F;
		function_num = dev->devfn & 7;
		dbg("bus %02x:%02x.%d devfn %04d vendorid %04x deviceid %04x  \n",
				dev->bus->number, device_number, function_num, dev->devfn, vendorid, deviceid);
		if (dev->dev.driver && dev->dev.driver->name)
			dbg("driver name %s\n",dev->dev.driver->name);//dev->dev.init_name
		devbdf = PCI_DEVID(dev->bus->number, dev->devfn);
		if ( devbdf == bdf) {
			int i;
			u32* outbuf=(u32*)buf;
			for( i=0; i<1024; i++) {
				pci_read_config_dword(dev, i*4, outbuf+i);
			}
			break;
		}
	}
	return 0;
}
volatile int varindex;
pcie_config_mem_t q[8];
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
static int my_ioctl(
		struct inode *i,
		struct file *f,
		unsigned int cmd, unsigned long arg)
#else
static long my_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
#endif
{
	long ret=0;
	int curindex=0;
	struct file *pcie_cfg_mem_file;
	long bdf = 0;
	curindex = __sync_fetch_and_add(&varindex, 1);
	dbg("%s cmd=0x%08X curindex=%d\n", current_time_str(NULL), cmd, curindex);
	memset(q[curindex].config_space, 0, sizeof(pcie_config_mem_t));
    switch (cmd)
    {
        case QUERY_GET_VARIABLES:
        	// get path from user
        	dbg("cmd=0x%08X QUERY_GET_VARIABLES\n", cmd);
            if (copy_from_user(q[curindex].config_space, (pcie_config_mem_t *)arg, sizeof(pcie_config_mem_t)))
            {
            	dbg("cmd=0x%08X failed to read path from user\n", cmd);
                ret = -EACCES;
                break;
            }
            dbg("cmd=0x%08X QUERY_GET_VARIABLES user input %s\n", cmd, q[curindex].config_space);
            ret = kstrtol(q[curindex].config_space, 16, &bdf);
            if (ret) {
            	dbg("kstrtol failed, error %x\n", ret);
            	break;
            }
            dbg("bdf is %04lX\n", bdf);
        	read_pcie_config_space((int)bdf, q[curindex].config_space);
            if (copy_to_user((pcie_config_mem_t *)arg, &q[curindex], sizeof(pcie_config_mem_t)))
            {
                ret = -EACCES;
            }
            break;
        case QUERY_CLR_VARIABLES:
        	dbg("cmd=0x%08X QUERY_CLR_VARIABLES\n", cmd);
            break;
        case QUERY_SET_VARIABLES:
            if (copy_from_user(&q[curindex], (pcie_config_mem_t *)arg, sizeof(q[curindex])))
            { 
                ret = -EACCES;
            }
            break;
        default:
            ret = -EINVAL;
	break;
    }
	__sync_fetch_and_sub(&varindex, 1);
	dbg("cmd=0x%08X curindex=%d return 0x%08X\n", cmd, curindex, (unsigned int)ret);
    return ret;
}
static int pciemem_device_open(struct inode* inode, struct file* filp)
{
	dbg("%s", current_time_str(NULL));

	if ( ((filp->f_flags & O_ACCMODE) == O_WRONLY)
	  || ((filp->f_flags & O_ACCMODE) == O_RDWR) ) {
		warn("write access is prohibited\n");
		return -EACCES;
	}

	/* Ensure that only one process has access to our device at any one time
 	* For more info on concurrent accesses, see http://lwn.net/images/pdf/LDD3/ch05.pdf */
	if (!mutex_trylock(&pciemem_device_mutex)) {
		warn("another process is accessing the device\n");
		return -EBUSY;
	}

	message_read = false;
	return 0;
}

static int pciemem_device_close(struct inode* inode, struct file* filp)
{
	dbg("%s", current_time_str(NULL));
	mutex_unlock(&pciemem_device_mutex);
	return 0;
}

static ssize_t pciemem_device_read(struct file* filp, char __user *buffer, size_t length, loff_t* offset)
{
	int retval;
	unsigned int copied;

	/* The default from 'cat' is to issue multiple reads until the FIFO is depleted
	 * one_shot avoids that */
	if (one_shot && message_read) return 0;
	dbg("%s", current_time_str(NULL));

	if (kfifo_is_empty(&pciemem_msg_fifo)) {
		dbg("no message in fifo\n");
		return 0;
	}

	retval = kfifo_to_user(&pciemem_msg_fifo, buffer, pciemem_msg_len[pciemem_msg_idx_rd], &copied);
	/* Ignore short reads (but warn about them) */
	if (pciemem_msg_len[pciemem_msg_idx_rd] != copied) {
		warn("short read detected\n");
	}
	/* loop into the message length table */
	pciemem_msg_idx_rd = (pciemem_msg_idx_rd+1)%MSG_FIFO_MAX;
	message_read = true;

	return retval ? retval : copied;
}

/* The file_operation scructure tells the kernel which device operations are handled.
 * For a list of available file operations, see http://lwn.net/images/pdf/LDD3/ch03.pdf */
static struct file_operations fops = {
	.read = pciemem_device_read,
	.open = pciemem_device_open,
//#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
//    .ioctl = my_ioctl,
//#else
    .unlocked_ioctl = my_ioctl,
//#endif
	.release = pciemem_device_close
};

/* Placing data into the read FIFO is done through sysfs */
static ssize_t sys_add_to_fifo(struct device* dev, struct device_attribute* attr, const char* buf, size_t count)
{
	unsigned int copied;

	dbg("%s", current_time_str(NULL));
	if (kfifo_avail(&pciemem_msg_fifo) < count) {
		warn("not enough space left on fifo\n");
		return -ENOSPC;
	}
	if ((pciemem_msg_idx_wr+1)%MSG_FIFO_MAX == pciemem_msg_idx_rd) {
		/* We've looped into our message length table */
		warn("message length table is full\n");
		return -ENOSPC;
	}

	/* The buffer is already in kernel space, so no need for ..._from_user() */
	copied = kfifo_in(&pciemem_msg_fifo, buf, count);
	pciemem_msg_len[pciemem_msg_idx_wr] = copied;
	if (copied != count) {
		warn("short write detected\n");
	}
	pciemem_msg_idx_wr = (pciemem_msg_idx_wr+1)%MSG_FIFO_MAX;

	return copied;
}

/* This sysfs entry resets the FIFO */
static ssize_t sys_reset(struct device* dev, struct device_attribute* attr, const char* buf, size_t count)
{
	dbg("%s", current_time_str(NULL));

	/* Ideally, we would have a mutex around the FIFO, to ensure that we don't reset while in use.
	 * To keep this sample simple, and because this is a sysfs operation, we don't do that */
	kfifo_reset(&pciemem_msg_fifo);
	pciemem_msg_idx_rd = pciemem_msg_idx_wr = 0;

	return count;
}

/* Declare the sysfs entries. The macros create instances of dev_attr_fifo and dev_attr_reset */
static DEVICE_ATTR(fifo, S_IWUSR, NULL, sys_add_to_fifo);
static DEVICE_ATTR(reset, S_IWUSR, NULL, sys_reset);

/* Module initialization and release */
static int __init pciemem_module_init(void)
{
	int retval;
	dbg("%s QUERY_GET_VARIABLES=0x%08X QUERY_SET_VARIABLES=0x%08X QUERY_CLR_VARIABLES=0x%08X",
			current_time_str(NULL), QUERY_GET_VARIABLES, QUERY_SET_VARIABLES, QUERY_CLR_VARIABLES);
// /sys/devices/virtual/myclass/myclass_mydevice/dev
	/* First, see if we can dynamically allocate a major for our device */
	pciemem_major = register_chrdev(0, DEVICE_NAME, &fops);
	if (pciemem_major < 0) {
		err("failed to register device: error %d\n", pciemem_major);
		retval = pciemem_major;
		goto failed_chrdevreg;
	}

	/* We can either tie our device to a bus (existing, or one that we create)
	 * or use a "virtual" device class. For this example, we choose the latter */
	pciemem_class = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(pciemem_class)) {
		err("failed to register device class '%s'\n", CLASS_NAME);
		retval = PTR_ERR(pciemem_class);
		goto failed_classreg;
	}

	/* With a class, the easiest way to instantiate a device is to call device_create() */
	pciemem_device = device_create(pciemem_class, NULL, MKDEV(pciemem_major, 0), NULL, CLASS_NAME "_" DEVICE_NAME);
	if (IS_ERR(pciemem_device)) {
		err("failed to create device '%s_%s'\n", CLASS_NAME, DEVICE_NAME);
		retval = PTR_ERR(pciemem_device);
		goto failed_devreg;
	}

	/* Now we can create the sysfs endpoints (don't care about errors).
	 * dev_attr_fifo and dev_attr_reset come from the DEVICE_ATTR(...) earlier */
	retval = device_create_file(pciemem_device, &dev_attr_fifo);
	if (retval < 0) {
		warn("failed to create write /sys endpoint - continuing without\n");
	}
	retval = device_create_file(pciemem_device, &dev_attr_reset);
	if (retval < 0) {
		warn("failed to create reset /sys endpoint - continuing without\n");
	}

	mutex_init(&pciemem_device_mutex);
	/* This device uses a Kernel FIFO for its read operation */
	INIT_KFIFO(pciemem_msg_fifo);
	pciemem_msg_idx_rd = pciemem_msg_idx_wr = 0;

	return 0;

failed_devreg:
	class_destroy(pciemem_class);
failed_classreg:
	unregister_chrdev(pciemem_major, DEVICE_NAME);
failed_chrdevreg:
	return -1;
}

static void __exit pciemem_module_exit(void)
{
	dbg("%s", current_time_str(NULL));
	device_remove_file(pciemem_device, &dev_attr_fifo);
	device_remove_file(pciemem_device, &dev_attr_reset);
	device_destroy(pciemem_class, MKDEV(pciemem_major, 0));
	class_destroy(pciemem_class);
	unregister_chrdev(pciemem_major, DEVICE_NAME);
}

module_init(pciemem_module_init);
module_exit(pciemem_module_exit);
