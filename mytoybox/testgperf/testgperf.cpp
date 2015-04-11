#include <iostream>
#include <inttypes.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#define NSEC_PER_SEC 1000000000ULL

#include "profiler.h"

uint64_t bentime_nsec(void) //internal API 
{        
	int rc;
        struct timespec tp;
        uint64_t stamp;

        //get timestamp by Linux API
        rc = clock_gettime(CLOCK_MONOTONIC, &tp);
        if (rc == -1)
                return 0;
        stamp = (uint64_t)tp.tv_sec * NSEC_PER_SEC;
        stamp += tp.tv_nsec;
        return stamp;
}

int delay_timer_nsec(uint64_t time_nsec) //internal API 
{
    int rc;
    struct timespec tp;
    uint64_t stamp;
    stamp = bentime_nsec() + time_nsec;
    tp.tv_sec = stamp / NSEC_PER_SEC;
    tp.tv_nsec = stamp - (tp.tv_sec * NSEC_PER_SEC);
    //call Linux sleep
    while (1) {
      rc = clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &tp, NULL); // resolution is not high
      if (rc == 0)
	      return 0;
      if (rc == EINTR)
	      continue;
      return -1;
    }
    return 0;
}

void forceleak()
{
//  std::cout << __FUNCTION__ << std::endl;
    char* p = new char[1];   // new might return 0x80000000, say.
    delete p;
    new char[1];             // new might return 0x80000000 again
    // This last new is a leak, but doesn't seem it: p looks like it points to it
    // HEAPCHECK=normal can report this leak
}

uint64_t rdtsc(void)
{ // for arm see http://neocontra.blogspot.com/2013/05/user-mode-performance-counters-for.html
    uint64_t x;
    __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
    return x;
}

void testcall(int cnt)
{
  for(int i=0; i<cnt; i++)
    forceleak();
}

int main(int argc, char* argv[])
{
  std::cout <<"add -lprofiler -ltcmalloc to the link-time step for your executable\n";
  std::cout <<"env CPUPROFILE=/tmp/mybin.prof " << argv[0] << std::endl;
  std::cout <<"env HEAPCHECK=normal " << argv[0] << std::endl;
  std::cout <<"example usage\nLD_LIBRARY_PATH=/home/onega/src/gperftools/gperftools-2.3.90/.libs:/usr/local/lib HEAPCHECK=normal CPUPROFILE=/tmp/mybin.prof PPROF_PATH=/home/onega/src/gperftools/gperftools-2.3.90/src/pprof "
	    << argv[0] << std::endl;
  forceleak();
  uint64_t start, end, pcstart, pcend;
  int loops = 1000;
  pcstart = rdtsc(); start = bentime_nsec();
  for(int i=0; i<loops;i++)
    delay_timer_nsec(1);
  end = bentime_nsec(); pcend = rdtsc();
  std::cout << "end-start=" << end-start <<" nsec for " << loops <<" loops, "<< (end-start)/loops <<" nsec per call of delay_timer_nsec" <<std::endl;
  std::cout << "cpu freq=" << (pcend-pcstart)*1000/(end-start) << " MHz\n";

  loops = 100;
  pcstart = rdtsc(); start = bentime_nsec();
  for(int i=0; i<loops;i++)
    rdtsc();
  end = bentime_nsec(); pcend = rdtsc();
  std::cout << "end-start=" << end-start <<" nsec for " << loops <<" loops, "<< (end-start)/loops <<" nsec per call of rdtsc" <<std::endl;
  std::cout << "cpu freq=" << (pcend-pcstart)*1000/(end-start) << " MHz\n";
  
  loops = 1000;
  pcstart = rdtsc(); start = bentime_nsec();
  for(int i=0; i<loops;i++)
    rdtsc();
  end = bentime_nsec(); pcend = rdtsc();
  std::cout << "end-start=" << end-start <<" nsec for " << loops <<" loops, "<< (end-start)/loops <<" nsec per call of rdtsc" <<std::endl;
std::cout << "cpu freq=" << (pcend-pcstart)*1000/(end-start) << " MHz\n";

  loops = 10000;
  pcstart = rdtsc(); start = bentime_nsec();
  for(int i=0; i<loops;i++)
    rdtsc();
  end = bentime_nsec(); pcend = rdtsc();
  std::cout << "end-start=" << end-start <<" nsec for " << loops <<" loops, "<< (end-start)/loops <<" nsec per call of rdtsc" <<std::endl;
  std::cout << "cpu freq=" << (pcend-pcstart)*1000/(end-start) << " MHz\n";
  
  loops = 1000;
  pcstart = rdtsc(); start = bentime_nsec();
  for(int i=0; i<loops;i++)
    bentime_nsec();
  end = bentime_nsec(); pcend = rdtsc();
  std::cout << "end-start=" << end-start <<" nsec for " << loops <<" loops, "<< (end-start)/loops <<" nsec per call of bentime_nsec" <<std::endl;
  std::cout << "cpu freq=" << (pcend-pcstart)*1000/(end-start) << " MHz\n";
  
  for(int i=0; i<100; i++)
    testcall(2);
  
  
  std::cout << "kill -SIGUSR1 " << getpid() << std::endl;
  int signaled = 0;  
//   while(!signaled)
//     signaled=sleep(1000);
  std::cout << argv[0] << " finished\n";
  return 0;
}

/*
g++ -o testgperf -L/home/onega/src/gperftools/gperftools-2.3.90/.libs testgperf.cpp -lprofiler -ltcmalloc

 LD_LIBRARY_PATH=/home/onega/src/gperftools/gperftools-2.3.90/.libs:/usr/local/lib HEAPCHECK=normal CPUPROFILE=/tmp/mybin.prof PPROF_PATH=/home/onega/src/gperftools/gperftools-2.3.90/src/pprof ./testgperf
 
 LD_LIBRARY_PATH=/home/onega/src/gperftools/gperftools-2.3.90/.libs:/usr/local/lib PPROF_PATH=/home/onega/src/gperftools/gperftools-2.3.90/src/pprof ./testgperf

[onega@localhost testgperf]$ LD_LIBRARY_PATH=/home/onega/src/gperftools/gperftools-2.3.90/.libs:/usr/local/lib /home/onega/src/gperftools/gperftools-2.3.90/src/pprof --text ./testgperf /tmp/mybin.prof 
Using local file ./testgperf.
Using local file /tmp/mybin.prof.
Total: 13 samples
      12  92.3%  92.3%       12  92.3% 0x00007fffe75fe998
       1   7.7% 100.0%        1   7.7% __clock_nanosleep
       0   0.0% 100.0%       12  92.3% __clock_gettime
       0   0.0% 100.0%       13 100.0% __libc_start_main
       0   0.0% 100.0%       13 100.0% _start
       0   0.0% 100.0%       12  92.3% bentime_nsec
       0   0.0% 100.0%       13 100.0% delay_timer_nsec
       0   0.0% 100.0%       12  92.3% get_timestamp
       0   0.0% 100.0%       13 100.0% main
       0   0.0% 100.0%       13 100.0% vast_delay_timer_nsec 
 
 sudo yum install kdesdk-kcachegrind
 
 LD_LIBRARY_PATH=/home/onega/src/gperftools/gperftools-2.3.90/.libs:/usr/local/lib /home/onega/src/gperftools/gperftools-2.3.90/src/pprof --callgrind ./testgperf /tmp/mybin.prof > /tmp/mybin.callgrind
 
 kcachegrind /tmp/mybin.callgrind 2>/dev/null &
 
 
*/