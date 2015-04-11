#include <client/linux/dump_writer_common/mapping_info.h>
#include <client/linux/minidump_writer/linux_dumper.h>
#include <client/linux/handler/exception_handler.h>
#include <exception>
static bool dumpCallback(const google_breakpad::MinidumpDescriptor& descriptor,
                         void* context,
                         bool succeeded)
{
  printf("Dump path: %s\n", descriptor.path());
  return succeeded;
}

//g++ -I/usr/local/include/breakpad -L/usr/local/lib -o usebreakpad usebreakpad.cpp -lbreakpad_client -lbreakpad -pthread
// g++ -std=c++14 -g -I. -I/usr/local/include/breakpad -L/usr/local/lib -o usebreakpad usebreakpad.cpp -lbreakpad_client -lbreakpad -pthread

int crash(int cnt)
{
	return cnt/(cnt-1);
}

int main(int argc, char* argv[]) {
	int ret = -1;
	  google_breakpad::MinidumpDescriptor descriptor("/tmp");
	  google_breakpad::ExceptionHandler eh(descriptor,
	                                       NULL,
	                                       dumpCallback,
	                                       NULL,
	                                       true,
	                                       -1);

	try {
		crash(argc);
	} catch (const std::exception& e) {
//		BOOST_LOG_TRIVIAL(error)<< argv[0] << " exception: " << e.what() << std::endl;
	}
	return ret;
}
