all: usebreakpad

usebreakpad:usebreakpad.cpp
	echo "two patches are needed for C++14"
	echo "https://code.google.com/p/google-breakpad/issues/detail?id=481"
	echo "another one is sed -n '120,120p' minidump_writer.h"
	g++ -std=c++14 -g -I/usr/local/include/breakpad -L/usr/local/lib -o usebreakpad usebreakpad.cpp -lbreakpad_client -lbreakpad -pthread
	
clean:
	if [ -f usebreakpad ]; then rm usebreakpad; fi
	