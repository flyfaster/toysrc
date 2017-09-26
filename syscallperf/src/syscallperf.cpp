//============================================================================
// Name        : syscallperf.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <iostream>
#include <thread>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <string>
#include <boost/timer/timer.hpp>
using namespace std;

int main(int argc, char** argv)
{
    std::size_t loops = 10000000;
    unsigned num_cpus = std::thread::hardware_concurrency();

    if (argc > 1)
        num_cpus = boost::lexical_cast<unsigned>(argv[1]);

    std::cout << "Launching " << num_cpus << " threads\n";
    boost::timer::auto_cpu_timer t;
    std::mutex iomutex;
    std::vector<std::thread> threads(num_cpus);
    for (unsigned i = 0; i < num_cpus; ++i)
    {
        threads[i] = std::thread([&iomutex, i, loops] {
            auto t1 = std::chrono::high_resolution_clock::now();
            {
                pid_t data;
                for (std::size_t i = 0; i < loops; i++)
                {
                    data = syscall(SYS_gettid);
                    data = getpid();
                }
            }
            auto t2 = std::chrono::high_resolution_clock::now();
            auto int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
            std::size_t call_per_sec = loops * (std::size_t) 2000;
            call_per_sec /= (std::size_t) int_ms.count();
            {
                std::lock_guard<std::mutex> iolock(iomutex);
                std::cout << "Thread #" << i << " spent " << int_ms.count()
                          << " milliseconds.\n";
                std::cout << call_per_sec << " syscalls per second\n";
            }
        });
    }

    for (auto& t : threads)
    {
        t.join();
    }
    cout << "syscall number " << loops * 2 << endl;
    return 0;
}

//toysrc onzhang$ syscallperf/Release/syscallperf
//Launching 8 threads
//Thread #6 spent 2034 milliseconds.
//9832841 syscalls per second
//Thread #2 spent 2052 milliseconds.
//9746588 syscalls per second
//Thread #7 spent 2056 milliseconds.
//9727626 syscalls per second
//Thread #4 spent 2057 milliseconds.
//9722897 syscalls per second
//Thread #1 spent 2089 milliseconds.
//9573958 syscalls per second
//Thread #3 spent 2178 milliseconds.
//9182736 syscalls per second
//Thread #5 spent 2390 milliseconds.
//8368200 syscalls per second
//Thread #0 spent 2501 milliseconds.
//7996801 syscalls per second
//syscall number 20000000
// 2.502090s wall, 9.300000s user + 6.980000s system = 16.280000s CPU (650.7%)
//
//toysrc onzhang$ syscallperf/Release/syscallperf 1
//Launching 1 threads
//Thread #0 spent 1552 milliseconds.
//12886597 syscalls per second
//syscall number 20000000
// 1.553181s wall, 0.880000s user + 0.660000s system = 1.540000s CPU (99.2%)
//
//sysctl -n machdep.cpu.brand_string
//Intel(R) Core(TM) i7-6920HQ CPU @ 2.90GHz
