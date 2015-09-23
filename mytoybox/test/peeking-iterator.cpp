// Below is the interface for Iterator, which is already defined for you.
// **DO NOT** modify the interface for Iterator.
class Iterator {
    struct Data;
	Data* data;
public:
	Iterator(const vector<int>& nums);
	Iterator(const Iterator& iter);
	virtual ~Iterator();
	// Returns the next element in the iteration.
	int next();
	// Returns true if the iteration has more elements.
	bool hasNext() const;
};


class PeekingIterator : public Iterator {
public:
	PeekingIterator(const vector<int>& nums) : Iterator(nums) {
	    // Initialize any member here.
	    // **DO NOT** save a copy of nums and manipulate it directly.
	    // You should only use the Iterator interface methods.
	    //data = &nums;
	}
    
    std::deque<int> cache;
    // Returns the next element in the iteration without advancing the iterator.
	int peek() {
        while(checknext() && cache.size()<1)
            cache.push_back(getnext());
        if(cache.size())
            return *cache.rbegin();
        return 0;
	}
	
	int getnext() {
	    return Iterator::next();
	}
    int checknext() const {
        return Iterator::hasNext();
    }
	// hasNext() and next() should behave the same as in the Iterator interface.
	// Override them if needed.
	int next() {
	    if (cache.size()) {
	        int ret = *cache.begin();
	        cache.pop_front();
	        return ret;
	    }
	    return getnext();
	}

	bool hasNext() const {
	    if(cache.size()) return true;
	    return checknext();
	}
};
