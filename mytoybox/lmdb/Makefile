all: lmdbfun

lmdbfun: lmdbfun.cpp
	objcopy --input binary --output elf64-x86-64 --binary-architecture i386 lmdbfun.cpp lmdbfun.cpp.bin
	objcopy --input binary --output elf64-x86-64 --binary-architecture i386 Makefile Makefile.bin
	g++ -I/home/onega/src/lmdb/libraries/liblmdb -I/home/onega/src/lmdb/lmdbxx -L/home/onega/src/lmdb/libraries/liblmdb -llmdb -o lmdbfun -g -pthread -std=c++11 lmdbfun.cpp lmdbfun.cpp.bin Makefile.bin

clean:
	@if [ -f lmdbfun ] ; then rm lmdbfun ; fi ;

cleandb:
	rm lock.mdb data.mdb
