#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
extern "C" {
#include "pciemem_driver.h"
};

char device_path[128];
char msgbuf[1024];

int main(int argc, char *argv[])
{
	pcie_config_mem_t myarg;
	if(argc>1)
		strcpy(myarg.config_space, argv[1]);
	else
		return 0;
	sprintf(device_path, "/dev/%s_%s", CLASS_NAME, DEVICE_NAME);
    int fd;
    fd = open(device_path, O_RDONLY);
    if (fd == -1)
    {
    	sprintf(msgbuf, "%s failed to open %s", argv[0], device_path);
        perror(msgbuf);
        return 2;
    }
    int ret = ioctl(fd, QUERY_GET_VARIABLES, &myarg);
    close (fd);
    printf("%s ioctl 0x%08X return %d\n", argv[0], QUERY_GET_VARIABLES, ret);

    if (ret)
    {
    	sprintf(msgbuf, "%s ioctl to %s return %08x", argv[0], device_path, ret);
	    perror(msgbuf);
    }
    // simulate output of sudo lspci -xxxx -s 00:07.0
    for (int i=0; i<4096; i++) {
    	if( (i%16)==0) {
    		printf("%03x: ",i);
    	}
    	printf("%02X ", (unsigned int)myarg.config_space[i]&0xFF);
    	if (i%16 == 15)
    		printf("\n");
    }
    return 0;
}
