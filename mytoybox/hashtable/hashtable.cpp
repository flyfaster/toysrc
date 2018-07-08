#include <iostream>
#include <stdexcept>
#include <cstring>
#include <cstdio>
struct record {
	char key[8];
	int data;
	size_t gethash() const;
	bool empty() const {
		return (key[0]==0);
	}
	bool operator==(const record& other){
		if(memcmp(this, &other, sizeof(other))==0)
			return true;
		return false;
	}
};

size_t record::gethash() const {
	size_t hash = 0;
	for(int i=0; i<8; i++){
		if(key[i])
			hash = (hash<<4) + key[i]%16;
		else
			break;
	}
	return hash;
}

class hashtable {
public:
	hashtable(size_t capacity);
	~hashtable();
	void insert(const record& element);
	void remove(const record& element);
	void get(char* key, record& element);
	size_t capacity() const {
		return capacity_;
	}
	class iterator {
	public:
		typedef iterator self_type;
		typedef record value_type;
		typedef record& reference;
		typedef record* pointer;
		typedef std::forward_iterator_tag iterator_category;
		typedef int difference_type;
		iterator(pointer ptr, pointer end) :
				ptr_(ptr), end_(end) {
		}
		self_type operator++() {
			self_type i = *this;
			do {
				ptr_++;
			}
			while(ptr_!=end_ && ptr_->empty());
			return i;
		}
		self_type operator++(int junk) {
			do {
				ptr_++;
			}
			while(ptr_!=end_ && ptr_->empty());
			return *this;
		}
		reference operator*() {
			return *ptr_;
		}
		pointer operator->() {
			return ptr_;
		}
		bool operator==(const self_type& rhs) {
			return ptr_ == rhs.ptr_;
		}
		bool operator!=(const self_type& rhs) {
			return ptr_ != rhs.ptr_;
		}
	private:
		pointer ptr_;
		pointer end_;
	};
	iterator begin() {
		if (cnt_ <1) {
			return iterator(storage_ + capacity_, storage_ + capacity_);
		}
		for(int i=0; i<capacity_; i++)
		{
			if(storage_[i].empty()==false) {
				return iterator(storage_+i, storage_ + capacity_);
			}
		}
	}

	iterator end() {
		return iterator(storage_ + capacity_, storage_ + capacity_);
	}
private:
	record* storage_;
	size_t capacity_;
	size_t cnt_;
};

hashtable::hashtable(size_t capacity):storage_(nullptr), cnt_(0), capacity_(0) {
	// http://isocpp.org/wiki/faq/exceptions#ctors-can-throw
	if(capacity<2) {
		throw std::invalid_argument("expect capacity>=2"); // http://en.cppreference.com/w/cpp/error/invalid_argument
	}
	storage_ = new record[capacity];
	if (storage_)
		capacity_ = capacity;
}

hashtable::~hashtable(){
	cnt_ = 0;
	capacity_ = 0;
	delete[] storage_;
}

void hashtable::insert(const record& element) {
	if(cnt_>=capacity_)
		throw std::out_of_range("full");

	size_t hash = element.gethash();
	size_t slot = hash%capacity_;
	while(true) {
		if(storage_[slot].empty()) {
			cnt_++; //
			break;
		}
		if(strcmp(storage_[slot].key, element.key)==0 ) {
			// overwrite existing element
			break;
		}
		slot++;
		if(slot>=capacity_)
			slot=slot%capacity_;
	}
	storage_[slot]=element;

}

void hashtable::remove(const record& element) {
	// when remove element from slot, need to consider there is
	// other slots stored elements which conflicts with existing element.
	// move that element to this slot?
	// hashtable is suitable for insert/query, delete will be more slow.
	throw std::runtime_error("not implemented yet");
}
void hashtable::get(char* key, record& element) {
	size_t hash = element.gethash();
	size_t slot = hash%capacity_;
	size_t startslot = slot;
	while(true) {
		if(storage_[slot].empty()) {
			throw std::runtime_error("not found");
			break;
		}
		if( strcmp(storage_[slot].key, key)==0) {
			element = storage_[slot];
			break;
		}
		slot++;
		if(slot>=capacity_)
			slot=slot%capacity_;
		if(slot == startslot){
			throw std::runtime_error("not found");
		}
	}
}

void print(hashtable& mytable) {
	for(hashtable::iterator i=mytable.begin(); i!=mytable.end(); i++)
	{
		std::cout<< i->key << ","<<i->data <<std::endl;
	}
}

int main(int argc, char* argv[])
{
	hashtable mytable(32);
	std::cout << "print empty hashtable\n";
	print(mytable);
	for(int i=0; i<8; i++)
	{
		record rec;
		sprintf(rec.key, "%d", i);
		rec.data = i;
		mytable.insert(rec);
	}
	std::cout << "print hashtable with a few elements.\n";
	print(mytable);
	for(int i=0; i<8; i++)
	{
		record rec;
		sprintf(rec.key, "%d", i);
		mytable.get(rec.key, rec);
		std::cout << "data is "<< rec.data<<std::endl;
	}

	try{
		record rec;
		sprintf(rec.key, "%zu", mytable.capacity());
		mytable.get(rec.key, rec);
		std::cout << "expect exception when element does not exist."<<std::endl;
	} catch(const std::exception& ex){
		std::cout<< "exception: " << ex.what()<<std::endl;
	}

	try{
		for(int i=0; i<mytable.capacity()+1; i++)
		{
			record rec;
			sprintf(rec.key, "%d", i);
			rec.data = i*10;
			mytable.insert(rec);
		}
		std::cout << "expect exception when hashtable is full."<<std::endl;
	} catch(const std::exception& ex){
		std::cout<< "exception " << ex.what()<<std::endl;
	}

	std::cout << "print full hashtable\n";
	print(mytable);
	std::cout << argv[0] << " done\n";
	return 0;
}
