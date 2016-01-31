#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <boost/program_options.hpp>
#include <iostream>
#include <string>
extern "C" {
#include "pciemem_driver.h"
};

char device_path[128];
char msgbuf[1024];

int main(int argc, char *argv[])
{
    namespace po = boost::program_options;
    boost::program_options::variables_map vm;
    po::options_description desc("Options");
    desc.add_options()
      ("help", "Print help messages")
	  ("bdf", po::value<std::string>(),  "specify BDF of PCIe device like 00:5.0")
      ("driverpath", po::value<std::string>(),  "specify driver path like /dev/myclass_mydevice_symlink")
	  ;

    try
    {
    	po::store(po::command_line_parser(argc, argv).options(desc).allow_unregistered().run(),vm);
      po::notify(vm);
      if ( vm.count("help") || vm.count("bdf")==0 )
      {
        std::cout << argv[0] << " usage:" << std::endl
                  << desc << std::endl;
        return 0;
      }
    }
    catch(po::error& e)
    {
      std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
      std::cerr << desc << std::endl;
      return __LINE__;
    }
    sprintf(device_path, "/dev/%s_%s", CLASS_NAME, DEVICE_NAME);
    if(vm.count("driverpath")) {
    	std::string data = vm["driverpath"].as<std::string>();
    	strcpy(device_path, data.c_str());
    }
	pcie_config_mem_t myarg;
    if(vm.count("bdf")) {
    	std::string data = vm["bdf"].as<std::string>();
    	int bus=0, device=0, function=0;
    	std::cout << data << std::endl;
    	int cnt = sscanf(data.c_str(), "%x:%x.%x", &bus, &device, &function);
    	if(cnt<3) {
    		std::cout << "Invalid bdf " << data<<std::endl;
    		return __LINE__;
    	}
    	int bdf = (bus<<8) | (device<<3)|function;
    	sprintf(myarg.config_space, "0x%x", bdf);
    	std::cout << myarg.config_space << std::endl;
    }
//	if(argc>1)
//		strcpy(myarg.config_space, argv[1]);
//	else
//		return 0;

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
    	printf("%02x ", (unsigned int)myarg.config_space[i]&0xFF);
    	if (i%16 == 15)
    		printf("\n");
    }
    return 0;
}
