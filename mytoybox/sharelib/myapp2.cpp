#include <iostream>
int testfunc(int a);

int main()
{
std::cout << "got " << testfunc(10)<<std::endl;
return 0;	
}

//[onega@localhost sharelib]$ g++ -shared -o libmylib.so.1 mylib1.cpp 
//[onega@localhost sharelib]$ g++ -shared -o libmylib.so.2 mylib2.cpp 
//[onega@localhost sharelib]$ g++ -o myapp1 myapp1.cpp libmylib.so.1
//[onega@localhost sharelib]$ g++ -o myapp2 myapp2.cpp libmylib.so.2
//[onega@localhost sharelib]$ ./myapp1
//got 3
//[onega@localhost sharelib]$ ./myapp2
//got 30
