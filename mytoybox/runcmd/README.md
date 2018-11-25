# Build on Windows 10
## Generate project files
Run the following command in ConEmuPack.180626 <BR>
c:\apps\cmake-3.13.0-win64-x64\bin\cmake.exe -DCMAKE\_GENERATOR\_PLATFORM=x64 -DCMAKE\_TOOLCHAIN\_FILE="C:\oss\vcpkg\scripts\buildsystems\vcpkg.cmake" .. <BR>
Note: in Eclipse CDT terminal the above command reports error: <BR>  
Could NOT find wxWidgets (missing: wxWidgets\_LIBRARIES <BR>
  wxWidgets\_INCLUDE\_DIRS)
## Build
c:\apps\cmake-3.13.0-win64-x64\bin\cmake.exe --build . --config RelWithDebInfo