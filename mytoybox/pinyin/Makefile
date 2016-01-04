all: pinyindict 

mainwnd_moc.cpp: mainwnd.h
	/usr/lib64/qt4/bin/moc $< -o $@
	
dict.bin: dict.txt
	objcopy --input binary --output elf64-x86-64 --binary-architecture i386 $< $@
	objdump -t $@
		
pinyindict:main.cpp mainwnd.cpp mainwnd_moc.cpp dict.bin
	g++ -o pinyindict -std=c++11 main.cpp mainwnd.cpp mainwnd_moc.cpp /usr/local/lib/libboost_locale.a dict.bin -licui18n -licuuc -lQtCore -lQtGui

clean:
	rm pinyindict mainwnd_moc.cpp dict.bin