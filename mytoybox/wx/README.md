# Build via cygwin on Windows
## build cmake-3.13.0.tar.gz from source via cygwin
As administrator run make install
## build wxWidgets-3.1.1.7z from source via cygwin
```
export CPPFLAGS='-w -fpermissive -D__USE_W32_SOCKETS'; export LDFLAGS='-L /lib/w32api/'; ./configure --prefix=/usr/local --with-msw
make
```
As administrator run make install
Modify PATH environment variable to include C:\oss\wxWidgets-3.1.1\lib

```
$ wx-config --libs --cflags
-I/usr/local/lib/wx/include/msw-unicode-3.1 -I/usr/local/include/wx-3.1 -D_FILE_OFFSET_BITS=64 -DWXUSINGDLL -D__WXMSW__
-L/usr/local/lib   -lwx_mswu_xrc-3.1 -lwx_mswu_html-3.1 -lwx_mswu_qa-3.1 -lwx_mswu_adv-3.1 -lwx_mswu_core-3.1 -lwx_baseu_xml-3.1 -lwx_baseu_net-3.1 -lwx_baseu-3.1
```
