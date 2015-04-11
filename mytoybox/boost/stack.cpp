//  Copyright (C) 2009 Tim Blechmann
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//[stack_example
#include <boost/thread/thread.hpp>
#include <boost/lockfree/stack.hpp>
#include <iostream>

#include <boost/atomic.hpp>
#include <boost/timer/timer.hpp>
boost::atomic_int producer_count(0);
boost::atomic_int consumer_count(0);

boost::lockfree::stack<int> stack(128);

const int producer_thread_count = 4;
const int consumer_thread_count = 4;
const int iterations = 1000000*4/producer_thread_count;

void producer(void)
{
    for (int i = 0; i != iterations; ++i) {
        int value = ++producer_count;
        while (!stack.push(value))
            ;
    }
}

boost::atomic<bool> done (false);

void consumer(void)
{
    int value;
    while (!done) {
        while (stack.pop(value))
            ++consumer_count;
    }

    while (stack.pop(value))
        ++consumer_count;
}

int main(int argc, char* argv[])
{
    using namespace std;
    cout << "boost::lockfree::stack is ";
    if (!stack.is_lock_free())
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
//]
/*
[onega@localhost boost]$ g++ -o stack -lboost_thread -lboost_timer -lboost_system stack.cpp 
[onega@localhost boost]$ LD_LIBRARY_PATH=/usr/local/lib ./stack
boost::lockfree::stack is lockfree
1 producers produced 4000000 objects.
1 consumers consumed 4000000 objects.
 1.270351s wall, 2.050000s user + 0.000000s system = 2.050000s CPU (161.4%)
[onega@localhost boost]$ g++ -o stack -lboost_thread -lboost_timer -lboost_system stack.cpp 
[onega@localhost boost]$ LD_LIBRARY_PATH=/usr/local/lib ./stack
boost::lockfree::stack is lockfree
4 producers produced 4000000 objects.
1 consumers consumed 4000000 objects.
 1.306669s wall, 1.680000s user + 0.280000s system = 1.960000s CPU (150.0%)
[onega@localhost boost]$ g++ -o stack -lboost_thread -lboost_timer -lboost_system stack.cpp 
[onega@localhost boost]$ LD_LIBRARY_PATH=/usr/local/lib ./stack
boost::lockfree::stack is lockfree
1 producers produced 4000000 objects.
4 consumers consumed 4000000 objects.
 3.062760s wall, 5.320000s user + 0.050000s system = 5.370000s CPU (175.3%)
[onega@localhost boost]$ g++ -o stack -lboost_thread -lboost_timer -lboost_system stack.cpp 
[onega@localhost boost]$ LD_LIBRARY_PATH=/usr/local/lib ./stack
boost::lockfree::stack is lockfree
4 producers produced 4000000 objects.
4 consumers consumed 4000000 objects.
 1.205505s wall, 2.010000s user + 0.050000s system = 2.060000s CPU (170.9%)
*/

