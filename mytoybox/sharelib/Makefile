all: libmylib.so.1 libmylib.so.2 myapp1 myapp2
	#g++ -shared -Wl,-soname,libmylib.so -o libmylib.so.1 mylib1.cpp
	#g++ -shared -Wl,-soname,libmylib.so -o libmylib.so.2 mylib2.cpp
	#g++ -o myapp1 myapp1.cpp libmylib.so.1  
	#g++ -o myapp2 myapp2.cpp libmylib.so.2  

libmylib.so.1: mylib1.cpp
	g++ -shared -o libmylib.so.1 mylib1.cpp

libmylib.so.2: mylib2.cpp
	g++ -shared -o libmylib.so.2 mylib2.cpp

myapp1: myapp1.cpp 
	g++ -o myapp1 myapp1.cpp libmylib.so.1 
	
myapp2: myapp2.cpp 
	g++ -o myapp2 myapp2.cpp libmylib.so.2 	

clean:
	rm libmylib.so.1 libmylib.so.2 myapp1 myapp2
