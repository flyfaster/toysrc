#Build on Windows 10 via VC++ 17 and cmake
```
C:\Users\flyfaster>systeminfo | findstr /B /C:"OS Name" /C:"OS Version"
OS Name:                   Microsoft Windows 10 Home
OS Version:                10.0.17134 N/A Build 17134
```
## Install toolchain
Install vs\_community\_\_1480592634.1543001610.exe <br>
Install vcpkg <br>
Unzip C:\Users\flyfaster\Downloads\cmake-3.13.0-win64-x64.zip
## Install dependencies: boost and wxWidgets
C:\oss\vcpkg>vcpkg install boost wxWidgets --triplet x64-windows <br>
find_package(wxWidgets COMPONENTS xrc webview stc richtext ribbon qa propgrid media html gl core aui adv xml net base REQUIRED)

## Generate project files
c:\apps\cmake-3.13.0-win64-x64\bin\cmake.exe -DCMAKE\_GENERATOR\_PLATFORM=x64 -DCMAKE\_TOOLCHAIN\_FILE="C:\oss\vcpkg\scripts\buildsystems\vcpkg.cmake" ..
## Build release configuration
c:\apps\cmake-3.13.0-win64-x64\bin\cmake.exe --build . --config Release

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
