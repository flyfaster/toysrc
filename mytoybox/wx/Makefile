all: hello miniwx gl

hello: hello.cpp
	wx-config --cppflags --libs base core aui
	g++ -I/usr/local/lib/wx/include/gtk3-unicode-3.0 -I/usr/local/include/wx-3.0 -D_FILE_OFFSET_BITS=64 -DWXUSINGDLL -D__WXGTK__ -L/usr/local/lib  -o hello hello.cpp -pthread   -lwx_baseu-3.0 -lwx_gtk3u_core-3.0 -lwx_gtk3u_aui-3.0

miniwx: miniwx.cpp
	g++ -o miniwx `wx-config --cppflags --libs base core aui` -pthread  miniwx.cpp

gl: BasicGLPane.cpp BasicGLPane.h
	echo "yum install mesa-libGLU-devel"
	echo "enable opengl when building wxWidgets ./configure --enable-stl --with-opengl"
	g++ BasicGLPane.cpp -o gl `wx-config --libs --cxxflags --gl-libs` -lGL -lGLU 


clean:
	rm hello miniwx gl
