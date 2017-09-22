
#include <iostream>
#include <string>
#include <alloca.h>
#include <memory>
using namespace std;

struct base {
virtual void action()=0;
virtual ~base()=default;
};
struct myclassx : public base
{
    myclassx() { std::cout << "myclassx ctor\n"; }
    ~myclassx() { std::cout << "myclassx dtor\n"; }
    void action() override
    {
        std::cout << "myclassx action\n";
    }
    int dd=888;
};

struct myclassy : public base
{
    myclassy() { std::cout << "myclassy ctor\n"; }
    ~myclassy() { std::cout << "myclassy dtor\n"; }
    void action() override
    {
        std::cout << "myclassy action\n";
    }
    double dd=0;
};

template <typename T> void call_dtor(T* obj)
{
	obj->~T();
}

int main(int argc, char**)
{
	cout << "allocate object on stack via placement new operator and call its dtor via std::unique_ptr\n";
	std::unique_ptr<base, std::function<void(base*)>> obj;
	if (argc>1)
	{
		void* buf = alloca(sizeof(myclassx));
		obj = std::unique_ptr<base, std::function<void(base*)>>(new(buf) myclassx, call_dtor<base>);
	} else
	{
		void* buf = alloca(sizeof(myclassy));
		obj = std::unique_ptr<base, std::function<void(base*)>>(new(buf) myclassx, call_dtor<base>);
	}
	obj->action();
	return 0;
}

