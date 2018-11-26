# Build on Windows 10
## compile proto file
C:\oss\vcpkg\packages\protobuf\_x64-windows\tools\protobuf\protoc.exe --plugin=protoc-gen-grpc="C:\oss\vcpkg\packages\grpc\_x64-windows\tools\grpc\grpc\_cpp_plugin.exe" runcmd.proto --cpp\_out .

C:\oss\vcpkg\packages\protobuf\_x64-windows\tools\protobuf\protoc.exe --cpp\_out . -I .. runcmd.proto

## Generate project files
Run the following command in ConEmuPack.180626 <BR>
c:\apps\cmake-3.13.0-win64-x64\bin\cmake.exe -DCMAKE\_GENERATOR\_PLATFORM=x64 -DCMAKE\_TOOLCHAIN\_FILE="C:\oss\vcpkg\scripts\buildsystems\vcpkg.cmake" .. <BR>

Note: in Eclipse CDT terminal the above command reports error:
 
Could NOT find wxWidgets (missing: wxWidgets\_LIBRARIES <BR>
  wxWidgets\_INCLUDE\_DIRS)
## Build
c:\apps\cmake-3.13.0-win64-x64\bin\cmake.exe --build . --config RelWithDebInfo