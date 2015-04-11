//  Copyright (C) 2009 Tim Blechmann
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
// 
// I made some modification to check performance of different producers and consumers.
// g++ -o queue -lboost_thread -lboost_system -lboost_timer queue.cpp
// LD_LIBRARY_PATH=/usr/local/lib ./queue
//[queue_example
#include <boost/thread/thread.hpp>
#include <boost/lockfree/queue.hpp>
#include <iostream>

#include <boost/atomic.hpp>
#include <boost/timer/timer.hpp>
boost::atomic_int producer_count(0);
boost::atomic_int consumer_count(0);

boost::lockfree::queue<int> queue(128);


const int producer_thread_count = 1;
const int consumer_thread_count = 1;
const int iterations = 1000000*4/producer_thread_count;

void producer(void)
{
    for (int i = 0; i != iterations; ++i) {
        int value = ++producer_count;
        while (!queue.push(value))
            ;
    }
}

boost::atomic<bool> done (false);
void consumer(void)
{
    int value;
    while (!done) {
        while (queue.pop(value))
            ++consumer_count;
    }

    while (queue.pop(value))
        ++consumer_count;
}

int main(int argc, char* argv[])
{
    using namespace std;
    cout << "boost::lockfree::queue is ";
    if (!queue.is_lock_free())
        cout << "not ";
    cout << "lockfree" << endl;

    boost::thread_group producer_threads, consumer_threads;
    boost::timer::auto_cpu_timer t;
    for (int i = 0; i != producer_thread_count; ++i)
        producer_threads.create_thread(producer);

    for (int i = 0; i != consumer_thread_count; ++i)
        consumer_threads.create_thread(consumer);

    producer_threads.join_all();
    done = true;

    consumer_threads.join_all();

    cout << producer_thread_count << " producers produced " << producer_count << " objects." << endl;
    cout << consumer_thread_count << " consumers consumed " << consumer_count << " objects." << endl;
}
/* 
[onega@localhost examples]$ LD_LIBRARY_PATH=/usr/local/lib ./queue
boost::lockfree::queue is lockfree
1 producers produced 4000000 objects.
1 consumers consumed 4000000 objects.
 1.800411s wall, 3.090000s user + 0.010000s system = 3.100000s CPU (172.2%)

[onega@localhost examples]$ LD_LIBRARY_PATH=/usr/local/lib ./queue
boost::lockfree::queue is lockfree
4 producers produced 4000000 objects.
1 consumers consumed 4000000 objects.
 1.848629s wall, 2.970000s user + 0.150000s system = 3.120000s CPU (168.8%)
 
[onega@localhost examples]$ LD_LIBRARY_PATH=/usr/local/lib ./queue
boost::lockfree::queue is lockfree
4 producers produced 4000000 objects.
4 consumers consumed 4000000 objects.
 2.099167s wall, 3.580000s user + 0.030000s system = 3.610000s CPU (172.0%) 
 
[onega@localhost examples]$ uname -a
Linux localhost.localdomain 3.10.0-123.el7.x86_64 #1 SMP Mon Jun 30 12:09:22 UTC 2014 x86_64 x86_64 x86_64 GNU/Linux

 
*/
//]
