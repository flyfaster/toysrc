//============================================================================
// Name        : mapallocator.cpp
// Description : test allocator with map
//============================================================================
#include <iostream>
#include <limits>
#include <map>
#include <stdlib.h>
#include <new>
#include <typeinfo>
template<typename T> struct myallocator: public std::allocator<T> {
	typedef std::allocator<T> base;
	typedef typename base::size_type size_type;
	typedef typename base::difference_type difference_type;
	typedef typename base::pointer pointer;
	typedef typename base::const_pointer const_pointer;
	typedef typename base::reference reference;
	typedef typename base::const_reference const_reference;
	typedef typename base::value_type value_type;
	myallocator() throw () {
		std::cout << __FUNCTION__ << std::endl;
	}
	myallocator(const myallocator& a) throw () :
			base(a) {
		std::cout << __FUNCTION__ << std::endl;
	}
	template<typename X> myallocator(const myallocator<X>&) throw () {
		std::cout << __FUNCTION__ << std::endl;
	}
	~myallocator() throw () {
		std::cout << __FUNCTION__ << std::endl;
	}
	template<typename X> struct rebind {
		typedef myallocator<X> other;
	};
	pointer allocate(size_type s, const void* hint = 0) {
		std::cout << __FUNCTION__ << "(" << s << ") size of T is " << sizeof(T)
				<< "\n";
		// record alloc request eg. ++num_allocs ;
		//return base::allocate(sz,hint) ;
		if (0 == s)
			return NULL;
		pointer temp = (pointer) malloc(s * sizeof(T));
		if (temp == NULL)
			throw std::bad_alloc();
		return temp;
	}
	void deallocate(pointer p, size_type n) {
		std::cout << __FUNCTION__ << "(" << n << ")\n";
		// record dealloc request eg. --num_allocs ;
		//return base::deallocate(p,n) ;
		free(p);
	}
	size_type max_size() const throw () {
		return std::numeric_limits<size_t>::max() / sizeof(T);
	}

	void construct(pointer p, const T& val) {
		std::cout << __FUNCTION__ << typeid(val).name() << "\n";
		new ((void *) p) T(val);
	}

	void destroy(pointer p) {
		std::cout << __FUNCTION__ << std::endl;
		p->~T();
	}
};
template<typename T> inline bool operator==(const myallocator<T>&,
		const myallocator<T>&) {
	std::cout << __FUNCTION__ << std::endl;
	return true;
}
template<typename T> inline bool operator!=(const myallocator<T>&,
		const myallocator<T>&) {
	std::cout << __FUNCTION__ << std::endl;
	return false;
}

struct address {
	int x, y;
	bool operator<(const address& ot) const {
		if (x < ot.x)
			return true;
		if (x > ot.x)
			return false;
		if (y < ot.y)
			return true;
		if (y > ot.y)
			return false;
		return false;
	}
}__attribute__((aligned(4),packed));
struct data {
	int block, data, repair_block;
}__attribute__((aligned(4),packed));

int main() {

	std::cout << "sizeof(std::pair<address,int> is "
			<< sizeof(std::pair<address, int>) << "\n";
	std::cout << "sizeof(std::pair<address,data> is "
			<< sizeof(std::pair<address, data>) << "\n";
	{
		std::map<address, data, std::less<address>,
				myallocator<std::pair<address, data> > > testmap;
		for (int i = 0; i < 8; i++) {
			address ad;
			ad.x = ad.y = i;
			data dd;
			dd.block = dd.data = i;
			testmap[ad] = dd;
		}
		testmap.clear();
	}
	{
		std::map<address, int, std::less<address>,
				myallocator<std::pair<address, int> > > testmap;
		for (int i = 0; i < 8; i++) {
			address ad;
			ad.x = ad.y = i;
			data dd;
			dd.block = dd.data = i;
			testmap[ad] = i;
		}
		testmap.clear();
	}
}

/*
g++ -o myallocator -g myallocator.cpp

./myallocator

sizeof(std::pair<address,int> is 12
sizeof(std::pair<address,data> is 20
myallocator
allocate(1) size of T is 56
myallocator
constructSt4pairIK7address4dataE
~myallocator
allocate(1) size of T is 56
myallocator
constructSt4pairIK7address4dataE
~myallocator
allocate(1) size of T is 56
myallocator
constructSt4pairIK7address4dataE
~myallocator
allocate(1) size of T is 56
myallocator
constructSt4pairIK7address4dataE
~myallocator
allocate(1) size of T is 56
myallocator
constructSt4pairIK7address4dataE
~myallocator
allocate(1) size of T is 56
myallocator
constructSt4pairIK7address4dataE
~myallocator
allocate(1) size of T is 56
myallocator
constructSt4pairIK7address4dataE
~myallocator
allocate(1) size of T is 56
myallocator
constructSt4pairIK7address4dataE
~myallocator
myallocator
destroy
~myallocator
deallocate(1)
myallocator
destroy
~myallocator
deallocate(1)
myallocator
destroy
~myallocator
deallocate(1)
myallocator
destroy
~myallocator
deallocate(1)
myallocator
destroy
~myallocator
deallocate(1)
myallocator
destroy
~myallocator
deallocate(1)
myallocator
destroy
~myallocator
deallocate(1)
myallocator
destroy
~myallocator
deallocate(1)
~myallocator
myallocator
allocate(1) size of T is 48
myallocator
constructSt4pairIK7addressiE
~myallocator
allocate(1) size of T is 48
myallocator
constructSt4pairIK7addressiE
~myallocator
allocate(1) size of T is 48
myallocator
constructSt4pairIK7addressiE
~myallocator
allocate(1) size of T is 48
myallocator
constructSt4pairIK7addressiE
~myallocator
allocate(1) size of T is 48
myallocator
constructSt4pairIK7addressiE
~myallocator
allocate(1) size of T is 48
myallocator
constructSt4pairIK7addressiE
~myallocator
allocate(1) size of T is 48
myallocator
constructSt4pairIK7addressiE
~myallocator
allocate(1) size of T is 48
myallocator
constructSt4pairIK7addressiE
~myallocator
myallocator
destroy
~myallocator
deallocate(1)
myallocator
destroy
~myallocator
deallocate(1)
myallocator
destroy
~myallocator
deallocate(1)
myallocator
destroy
~myallocator
deallocate(1)
myallocator
destroy
~myallocator
deallocate(1)
myallocator
destroy
~myallocator
deallocate(1)
myallocator
destroy
~myallocator
deallocate(1)
myallocator
destroy
~myallocator
deallocate(1)
~myallocator

*/
