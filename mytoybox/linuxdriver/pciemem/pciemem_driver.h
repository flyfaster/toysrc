/*
 */

#define DEVICE_NAME "mydevice"
#define CLASS_NAME "myclass"
#define MSG_FIFO_SIZE 1024
#define MSG_FIFO_MAX  128

#define AUTHOR "Onega Zhang <onega@unknown.com>"
#define DESCRIPTION "prototype device driver to read PCIe configuration space"
#define VERSION "1.0"

/* We'll use our own macros for printk */
#define dbg(format, arg...) do { if (debug) {pr_info(CLASS_NAME ": %s: " format , __FUNCTION__ , ## arg);} } while (0)
#define err(format, arg...) do { pr_err(CLASS_NAME ": " format, ## arg); } while (0)
#define info(format, arg...) do { pr_info(CLASS_NAME ": " format, ## arg); } while (0)
#define warn(format, arg...) do { pr_warn(CLASS_NAME ": " format, ## arg); } while (0)

typedef struct
{
    char config_space[4096];
} pcie_config_mem_t;

// QUERY_GET_VARIABLES=0x80087101 QUERY_SET_VARIABLES=0x40087103 QUERY_CLR_VARIABLES=0x00007102
#define QUERY_GET_VARIABLES _IOR('q', 1, pcie_config_mem_t )
#define QUERY_CLR_VARIABLES _IO('q', 2)
#define QUERY_SET_VARIABLES _IOW('q', 3, pcie_config_mem_t )
