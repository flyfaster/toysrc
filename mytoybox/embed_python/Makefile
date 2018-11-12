all: embed_python

clean:
	rm embed_python
	

CFLAGS=-I/usr/local/Cellar/python/3.7.0/Frameworks/Python.framework/Versions/3.7/include/python3.7m \
	-I/usr/local/Cellar/python/3.7.0/Frameworks/Python.framework/Versions/3.7/include/python3.7m \
	-Wno-unused-result -Wsign-compare -Wunreachable-code -fno-common -dynamic -DNDEBUG -g -fwrapv \
	-O3 -Wall -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.14.sdk \
	-I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.14.sdk/System/Library/Frameworks/Tk.framework/Versions/8.5/Headers

LDFLAGS=-L/usr/local/opt/python/Frameworks/Python.framework/Versions/3.7/lib/python3.7/config-3.7m-darwin \
	-lpython3.7m -ldl -framework CoreFoundation

CFLAGS=`python3-config --cflags`
LDFLAGS=`python3-config --ldflags`

embed_python: embed_python.cpp
	clang++ -std=c++14 ${CFLAGS} ${LDFLAGS} -o embed_python $<