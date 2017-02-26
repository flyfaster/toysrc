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
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <spawn.h>
// #if defined(__has_feature) && __has_feature(thread_sanitizer) 
// #define TSAN_ENABLED
// #endif 

static int callback(struct dl_phdr_info *info, size_t size, void *data)
{
    printf("name=%s (%d segments)\n", info->dlpi_name, info->dlpi_phnum);
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
const int iterations = 100000;
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

const char* get_self_path() {
  static char selfpath[PATH_MAX];
  if(selfpath[0])
    return selfpath;
  char path[PATH_MAX];
  pid_t pid = getpid();
  sprintf(path, "/proc/%d/exe", pid);
  if (readlink(path, selfpath, PATH_MAX) == -1)
    perror("readlink");
    return selfpath;
}

__attribute__((constructor))
static void set_thread_santinizer_suppressions() {
    printf("%s pid %d %s TSAN_OPTIONS=%s\n",get_self_path(), getpid(),  __func__, getenv("TSAN_OPTIONS"));
    //std::cout << get_self_path() << " pid " << getpid() << " " << __func__ << " TSAN_OPTIONS=" << getenv("TSAN_OPTIONS") << std::endl;
}
extern char **environ;
int main(int argc, char* argv[]) {
       pid_t pid = -1;
       
    if(getenv("TSAN_OPTIONS")==NULL) {
        std::string selfpath = get_self_path();
        size_t pos = selfpath.find_last_of('/');
        if (pos==std::string::npos) {
            std::cout << argv[0] << " failed to get self path: " << selfpath << std::endl;
            exit(EXIT_FAILURE);
        }
        std::string selfdir = selfpath.substr(0, pos+1);
        std::stringstream ss;
        ss << "suppressions=" << selfdir << "tsan.suppression";
        setenv("TSAN_OPTIONS", ss.str().c_str(), 1 );

        int status = posix_spawn(&pid, get_self_path(), NULL, NULL, argv, environ);
        if (status == 0) {
            printf("Child pid: %i\n", pid);
            if (waitpid(pid, &status, 0) != -1) {
                printf("Child exited with status %i\n", status);
            } else {
                perror("waitpid");
            }            
        } else {
            printf("posix_spawn: %s\n", strerror(status));
        }
        return 0;
#if 0        
        pid = fork();
        if (pid == -1) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0) {
            std::cout << argv[0] << " in child process " << getpid() << std::endl;
            return execv(argv[0], argv);            
        }
        else {
            int status;
            (void)waitpid(pid, &status, 0);
            return 0;
        }   
#endif        
    }
    std::cout <<argv[0] << " pid " << getpid() << " TSAN_OPTIONS=" << getenv("TSAN_OPTIONS") << std::endl;
    using namespace std;
        if(argc>1)
        sleep_when_busy = atoi(argv[1]);
    cout << argv[0] << " pid " << getpid() << " sleep_when_busy="<<sleep_when_busy << " boost::lockfree::queue is ";
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
    benchmark_queue(spsc_queue); // WARNING: ThreadSanitizer: data race (pid=13113) 
    benchmark_queue(mpmc_queue); // http://stackoverflow.com/questions/37552866/why-does-threadsanitizer-report-a-race-with-this-lock-free-example
    
#ifdef __linux__
	dl_iterate_phdr(callback, NULL);
#endif    
}

__attribute__((destructor))
static void mydestructor() {
    printf("%s pid %d\n",  __func__, getpid());
}