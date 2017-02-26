#include <boost/thread/thread.hpp>
#include <boost/lockfree/spsc_queue.hpp>
#include <boost/lockfree/queue.hpp>
#include <iostream>
#include <boost/atomic.hpp>
#include <math.h> 
#include <iostream>
#include <vector>
#include <list>
#include <iterator>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <random>
#include <algorithm>
#include <boost/type_index.hpp>
#include <forward_list>
#include <deque>
#include <sstream>
#include <link.h>
#include <thread>
static int callback(struct dl_phdr_info *info, size_t size, void *data)
{
    int j;
   printf("name=%s (%d segments)\n", info->dlpi_name, info->dlpi_phnum);
//    for (j = 0; j < info->dlpi_phnum; j++)
//          printf("\t\t header %2d: address=%10p\n", j,
//              (void *) (info->dlpi_addr + info->dlpi_phdr[j].p_vaddr));
    return 0;
}

struct start_end {
	start_end(const std::string& msg): msg_(msg){ 
		std::cout << msg_ << " start " << std::endl;
		boost::typeindex::type_id_with_cvr<decltype(this)>().pretty_name();
	};
	~start_end(){
		auto t2 = std::chrono::high_resolution_clock::now();
        auto int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - start_);        
        if(int_ms.count()>2)
        	std::cout << msg_ << " end in " << int_ms.count() << " milliseconds" << std::endl;
        else {
        	auto int_us = std::chrono::duration_cast<std::chrono::microseconds>(t2 - start_);
        	std::cout << msg_ << " end in " << int_us.count() << " microseconds" << std::endl;
        }
	}
	std::string msg_;
	std::chrono::time_point<std::chrono::high_resolution_clock> start_= std::chrono::high_resolution_clock::now();
};

int producer_count = 0;
int sleep_when_busy = 0;
boost::atomic_int consumer_count (0);
boost::lockfree::spsc_queue<int, boost::lockfree::capacity<102400> > spsc_queue;
boost::lockfree::queue<int, boost::lockfree::capacity<1024*32> > mpmc_queue;
const int iterations = 4000000;
#define PI 3.14159265

template<typename ContainerType>
void template_producer(ContainerType& spsc_queue) {
    start_end	show_func_start_end(__func__);    
    auto complexmath=[](int value) {
          double param, result;
        param = value%180+1;
        double radians = param * PI / 180.0;
        result = tan ( radians )+sin(radians) +cos(radians) +log(param) + log10(param) + log2(value);
        result += logb(result);
        return lrint(result)+1;
    };
    for (int i = 0; i != iterations; ++i) {
        int value = ++producer_count;
        value = complexmath(value);
        while (!spsc_queue.push(value))
            if(sleep_when_busy) usleep(sleep_when_busy);
    }
    std::cout << __func__ << " done with " << producer_count << " objects in " << boost::typeindex::type_id_with_cvr<ContainerType>().pretty_name() << std::endl;
}

boost::atomic<bool> done (false);
template<typename ContainerType>
void template_consumer(ContainerType& spsc_queue) {
    start_end	show_func_start_end(__func__);    
    auto complexmath=[](int value) {
          double param, result;
        param = value%180+1;
        double radians = param * PI / 180.0;
        result = tan ( radians )+sin(radians) +cos(radians) +log(param) + log10(param) + log2(value);
        return lrint(result)+1;
    };
    int value;
    while (!done) {
        while (spsc_queue.pop(value)) {
            complexmath(value);
            ++consumer_count;
        }
        if(sleep_when_busy) usleep(sleep_when_busy);
    }

    while (spsc_queue.pop(value)) {
        complexmath(value);
        ++consumer_count;
    }
    std::cout << __func__ << " done with "<< consumer_count << " objects in " << boost::typeindex::type_id_with_cvr<ContainerType>().pretty_name() << std::endl;
}

int main(int argc, char* argv[]) {
    using namespace std;
        if(argc>1)
        sleep_when_busy = atoi(argv[1]);
    cout <<"sleep_when_busy="<<sleep_when_busy << " boost::lockfree::queue is ";
    if (!spsc_queue.is_lock_free()) cout << "not ";
    cout << "lockfree" << endl;
        
    auto dummyf=[](int ) {};
    mpmc_queue.consume_all(dummyf);
    spsc_queue.consume_all(dummyf);
    done = false; consumer_count=0;  producer_count = 0;
    auto benchmark_queue=[](auto& activecontainer) {
        activecontainer.consume_all([](int  ) {} );
        done = false; consumer_count=0;  producer_count = 0;
        std::thread producer_thread([&activecontainer](){ template_producer<decltype(activecontainer)>(activecontainer);    });
        std::thread consumer_thread([&activecontainer]() {template_consumer<decltype(activecontainer)>(activecontainer);    });
        //std::thread producer_thread(template_producer<boost::lockfree::spsc_queue<int, boost::lockfree::capacity<102400> >>, spsc_queue);
        // std::thread consumer_thread(template_consumer<decltype(spsc_queue)>, spsc_queue);
        producer_thread.join();
        done = true;
        consumer_thread.join();
    };
    benchmark_queue(spsc_queue);
    benchmark_queue(mpmc_queue);
    
#ifdef __linux__
	dl_iterate_phdr(callback, NULL);
#endif    
}