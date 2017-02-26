[onega@localhost ~]$ /home/onega/workspace/thread/lockfreetest 0
sleep_when_busy=0 boost::lockfree::queue is lockfree
template_producer start 
template_consumer start 
template_producer done with 4000000 objects in boost::lockfree::spsc_queue<int, boost::lockfree::capacity<102400ul>, boost::parameter::void_>&
template_producer end in 10542 milliseconds
template_consumer done with 4000000 objects in boost::lockfree::spsc_queue<int, boost::lockfree::capacity<102400ul>, boost::parameter::void_>&
template_consumer end in 10542 milliseconds
template_producer start 
template_consumer start 
template_producer done with 4000000 objects in boost::lockfree::queue<int, boost::lockfree::capacity<32768ul>, boost::parameter::void_, boost::parameter::void_>&
template_producer end in 10907 milliseconds
template_consumer done with 4000000 objects in boost::lockfree::queue<int, boost::lockfree::capacity<32768ul>, boost::parameter::void_, boost::parameter::void_>&
template_consumer end in 10907 milliseconds
name= (9 segments)
name= (4 segments)
name=/usr/local/lib/libboost_thread.so.1.59.0 (7 segments)
name=/usr/local/lib/libboost_system.so.1.59.0 (7 segments)
name=/home/onega/gcc-6.3.0/lib64/libstdc++.so.6 (7 segments)
name=/usr/lib64/libm.so.6 (9 segments)
name=/home/onega/gcc-6.3.0/lib64/libgcc_s.so.1 (6 segments)
name=/usr/lib64/libpthread.so.0 (9 segments)
name=/usr/lib64/libc.so.6 (10 segments)
name=/usr/lib64/librt.so.1 (9 segments)
name=/lib64/ld-linux-x86-64.so.2 (7 segments)
[onega@localhost ~]$ time  for i in `seq 1 5`; do /home/onega/workspace/thread/lockfreetest 0 > /dev/null ; done;

real	1m27.745s
user	2m52.728s
sys	0m0.040s
[onega@localhost ~]$ 

[onega@localhost ~]$ /home/onega/workspace/thread/lockfreetest 1
sleep_when_busy=1 boost::lockfree::queue is lockfree
template_consumer start 
template_producer start 
template_producer done with 4000000 objects in boost::lockfree::spsc_queue<int, boost::lockfree::capacity<102400ul>, boost::parameter::void_>&
template_producer end in 8242 milliseconds
template_consumer done with 4000000 objects in boost::lockfree::spsc_queue<int, boost::lockfree::capacity<102400ul>, boost::parameter::void_>&
template_consumer end in 8243 milliseconds
template_producer start 
template_consumer start 
template_producer done with 4000000 objects in boost::lockfree::queue<int, boost::lockfree::capacity<32768ul>, boost::parameter::void_, boost::parameter::void_>&
template_producer end in 10252 milliseconds
template_consumer done with 4000000 objects in boost::lockfree::queue<int, boost::lockfree::capacity<32768ul>, boost::parameter::void_, boost::parameter::void_>&
template_consumer end in 10249 milliseconds
name= (9 segments)
name= (4 segments)
name=/usr/local/lib/libboost_thread.so.1.59.0 (7 segments)
name=/usr/local/lib/libboost_system.so.1.59.0 (7 segments)
name=/home/onega/gcc-6.3.0/lib64/libstdc++.so.6 (7 segments)
name=/usr/lib64/libm.so.6 (9 segments)
name=/home/onega/gcc-6.3.0/lib64/libgcc_s.so.1 (6 segments)
name=/usr/lib64/libpthread.so.0 (9 segments)
name=/usr/lib64/libc.so.6 (10 segments)
name=/usr/lib64/librt.so.1 (9 segments)
name=/lib64/ld-linux-x86-64.so.2 (7 segments)
[onega@localhost ~]$ time /home/onega/workspace/thread/lockfreetest 1 > /dev/null

real	0m16.350s
user	0m18.400s
sys	0m5.078s
[onega@localhost ~]$ time  for i in `seq 1 5`; do /home/onega/workspace/thread/lockfreetest 1 > /dev/null ; done;

real	1m22.568s
user	1m33.674s
sys	0m23.794s
