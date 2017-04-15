//============================================================================
// Name        : invalidfree.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Check if compiler/debugger can detect incorrect dtor
//============================================================================
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <boost/current_function.hpp>
#include <boost/version.hpp>
#include <boost/config.hpp>
using namespace std;

class A {
public:
	A() { cout << BOOST_CURRENT_FUNCTION << endl; }
	~A() { cout << BOOST_CURRENT_FUNCTION << endl; }
	void test() {cout << BOOST_CURRENT_FUNCTION << endl; }
};

class B: public A {
public:
	B() { cout << BOOST_CURRENT_FUNCTION << endl; }
	~B() { cout << BOOST_CURRENT_FUNCTION << endl; }
	virtual void test() {cout << BOOST_CURRENT_FUNCTION << endl; }
};

class C: public B {
public:
	C() { cout << BOOST_CURRENT_FUNCTION << endl; }
	~C() { cout << BOOST_CURRENT_FUNCTION << endl; }
	virtual void test() {cout << BOOST_CURRENT_FUNCTION << endl; }
};

int main() {
	std::cout << "Compiler: " << BOOST_COMPILER
		<< ", Platform: " << BOOST_PLATFORM 
		<< ", Library: " << BOOST_STDLIB
		<< ", Boost " << BOOST_LIB_VERSION
		<< ", pid " << getpid() << endl;
	B* objb = new C;
	objb->test();
	delete objb;	// -Wdelete-non-virtual-dtor

	cout << endl << "calling mismatch dtor" << endl;
	A* obj = new C;
	obj->test();
	delete obj; // gcc/clang/gdb/valgrind does not report error here
	return 0;
}
/*
g++ -o ginvalidfree -g invalidfree.cpp
./ginvalidfree 
Compiler: GNU C++ version 5.4.0 20160609, Platform: linux, Library: GNU libstdc++ version 20160609, Boost 1_58, pid 3153
A::A()
B::B()
C::C()
virtual void C::test()
B::~B()
A::~A()

calling mismatch dtor
A::A()
B::B()
C::C()
void A::test()
A::~A()

clang++ -o minvalidfree -Weverything -fsanitize=memory -fPIE -pie -fno-omit-frame-pointer -g -O2 invalidfree.cpp

clang++ -o ainvalidfree -Weverything -fsanitize=address -fPIE -pie -fno-omit-frame-pointer -g -O2 invalidfree.cpp

clang++ -o cinvalidfree -Weverything -fPIE -pie -fno-omit-frame-pointer -g -O2 invalidfree.cpp

g++ 5.4.0 does not report error
valgrind memcheck or massif does not report error
valgrind --tool=massif ./cinvalidfree
cppcheck-1.78 does not report error
gdb does not report error

*/
