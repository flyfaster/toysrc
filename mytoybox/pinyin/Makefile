all: pinyindict wxpinyindict

mainwnd_moc.cpp: mainwnd.h
	/usr/lib64/qt4/bin/moc $< -o $@
	
dict.bin: dict.txt
	objcopy --input binary --output elf64-x86-64 --binary-architecture i386 $< $@
	objdump -t $@
		
pinyindict:main.cpp mainwnd.cpp mainwnd_moc.cpp dict.bin
	g++ -o pinyindict -std=c++11  mainwnd.cpp main.cpp mainwnd_moc.cpp /usr/local/lib/libboost_locale.a dict.bin -licui18n -licuuc -lQtCore -lQtGui

# /usr/local/bin/wx-config --libs --cppflags
wxpinyindict: main.cpp wxfront.cpp dict.bin
	g++ -o $@ -std=c++11 wxfront.cpp main.cpp /usr/local/lib/libboost_locale.a dict.bin -I/usr/local/lib/wx/include/gtk3-unicode-3.0 -I/usr/local/include/wx-3.0 -D_FILE_OFFSET_BITS=64 -DWXUSINGDLL -D__WXGTK__ -L/usr/local/lib -pthread   -lwx_gtk3u_xrc-3.0 -lwx_gtk3u_html-3.0 -lwx_gtk3u_qa-3.0 -lwx_gtk3u_adv-3.0 -lwx_gtk3u_core-3.0 -lwx_baseu_xml-3.0 -lwx_baseu_net-3.0 -lwx_baseu-3.0 -licui18n -licuuc

clean:
	rm pinyindict mainwnd_moc.cpp dict.bin wxpinyindict