
CFLAGS = -fno-omit-frame-pointer -finstrument-functions -std=c++11
#CFLAGS = -fno-omit-frame-pointer -std=c++11

all: testgperf

testgperf: testgperf.cpp profiler.cpp profiler.h
	objcopy --input binary \
            --output elf64-x86-64 \
            --binary-architecture i386 testgperf.cpp testgperf.cpp.o
	objcopy --input binary \
            --output elf64-x86-64 \
            --binary-architecture i386 profiler.cpp profiler.cpp.o
	objcopy --input binary \
            --output elf64-x86-64 \
            --binary-architecture i386 profiler.h profiler.h.o
	objcopy --input binary \
            --output elf64-x86-64 \
            --binary-architecture i386 Makefile Makefile-binary.o
            
	g++ -o testgperf -L/home/onega/src/gperftools/gperftools-2.3.90/.libs -rdynamic ${CFLAGS} -g -std=c++11 testgperf.cpp profiler.cpp *.o -lboost_program_options -lbfd -lprofiler -ltcmalloc -ldl
	
clean:
	rm testgperf Makefile-binary.o profiler.h.o profiler.cpp.o testgperf.cpp.o
