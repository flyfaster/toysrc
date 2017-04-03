//============================================================================
// Name        : boostshm.cpp
// Author      : Onega
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/program_options.hpp>
#include <boost/interprocess/sync/named_semaphore.hpp>
#include <boost/interprocess/detail/os_thread_functions.hpp>
#include <boost/scoped_array.hpp>
#define BOOST_CB_DISABLE_DEBUG // The Debug Support has to be disabled, otherwise the code produces a runtime error.
#include <boost/circular_buffer.hpp>
#include <iostream>
#include <cstring>
#include <cstdlib>
class ProfileTimer
{
public:
    ProfileTimer(): start_time_(boost::posix_time::microsec_clock::local_time())
    {    }
    ~ProfileTimer()
    {    }
     void restart()
     {
          start_time_ = boost::posix_time::microsec_clock::local_time();
     }
    double Elapsed()
    {
        boost::posix_time::time_duration d = boost::posix_time::microsec_clock::local_time() - start_time_;
        double result = d.ticks();
        return result /= d.ticks_per_second();
    }
private:
    boost::posix_time::ptime start_time_;
};
int spawn (char* program, char** arg_list)
{
  pid_t child_pid = fork ();
  if (child_pid != 0)
    return child_pid;
  else {
    execvp (program,  arg_list);
    std::cerr<<__FUNCTION__<<" an error occurred in execvp("<<program<<")\n";
    abort ();
  }
  return child_pid;
}

int start_consumer_process(int capacity, int message_size, int loop,
		char* process_name) {
	char capacity_buf[32], size_buf[32], loop_buf[32];
	sprintf(capacity_buf, "%d", capacity);
	sprintf(size_buf, "%d", message_size);
	sprintf(loop_buf, "%d", loop);
	char* arg_list[] = { process_name, /* argv[0], the name of the program.  */
	"--capacity", capacity_buf, "--size", size_buf, "--loop", loop_buf,
			"--role", "1", NULL /* The argument list must end with a NULL.  */
	};
	return spawn(process_name, arg_list);
}

int main(int argc, char *argv[])
{
	for (int i=0;i<argc; i++)
		std::cout<<argv[i]<<" ";
	std::cout<<std::endl <<"PID:"<<boost::interprocess::ipcdetail::get_current_process_id()
  	  <<std::endl;
	boost::program_options::variables_map vm;
	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
	    ("help,h", "produce help message")
	    ("capacity", boost::program_options::value<int>(), "message queue size")
	    ("size", boost::program_options::value<int>(), "max message size")
	    ("loop", boost::program_options::value<int>(), "loop count")
	    ("role", boost::program_options::value<int>(), "0: sender, 1: receiver. User only needs to start process with --role 0!")
	;
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
	boost::program_options::notify(vm);
	if (vm.count("role")==0 || vm.count("capacity")==0
			|| vm.count("size")==0 || vm.count("loop")==0
			|| vm.count("help")==1)
	{
		std::cout << desc <<std::endl;
		return __LINE__;
	}
	int capacity, message_size, loop, role;
	capacity=vm["capacity"].as<int>();
	message_size=vm["size"].as<int>();
	loop=vm["loop"].as<int>();
	role=vm["role"].as<int>();
	boost::scoped_array<char> buf_holder(new char[message_size]);
	char* buf=buf_holder.get();
   using namespace boost::interprocess;
   if(0 == role){  //Parent process
      struct shm_remove
      {
         shm_remove() {
        	 std::cout<<__FUNCTION__<<std::endl;
        	 shared_memory_object::remove("MySharedMemory");
			 named_semaphore::remove("sem_avail");
			 named_semaphore::remove("sem_empty");
         }
         ~shm_remove(){
        	 shared_memory_object::remove("MySharedMemory");
        	 std::cout<<__FUNCTION__<<std::endl;
         }
      } remover;
      named_semaphore sem_avail(create_only, "sem_avail", 0);
      named_semaphore sem_empty(create_only, "sem_empty", capacity);
      shared_memory_object shm (create_only, "MySharedMemory", read_write);
      shm.truncate(capacity*message_size);
      mapped_region region(shm, read_write);
      start_consumer_process(capacity, message_size, loop, argv[0]);
      ProfileTimer pt;
      for(int lp=0; lp<loop; lp++)
      {
		  for(int i = 0; i < capacity; ++i){
			  sem_empty.wait();
			  memset( (char*)region.get_address()+i*capacity, i, i%message_size);
			  sem_avail.post();
		  }
      }
      std::cout << __FUNCTION__ << " used time:"<<pt.Elapsed()
    		  <<" seconds in process "<<boost::interprocess::ipcdetail::get_current_process_id()
      	  	  <<std::endl;
   }
   else{
      shared_memory_object shm (open_only, "MySharedMemory", read_write);
      mapped_region region(shm, read_write);
      named_semaphore sem_avail(open_only, "sem_avail");
      named_semaphore sem_empty(open_only, "sem_empty");
      ProfileTimer pt;
      char *mem = static_cast<char*>(region.get_address());
      for(int lp=0; lp<loop; lp++)
      {
		  for(int i = 0; i < capacity; ++i){
			  sem_avail.wait();
			  memcpy(buf, mem+i*capacity, i); // emulate accessing shared memory
			  sem_empty.post();
		  }
      }
      std::cout << __FUNCTION__ << " used time:"<<pt.Elapsed()
    		  <<" seconds in process "<<boost::interprocess::ipcdetail::get_current_process_id()
      	  	  <<std::endl;
   }
   return 0;
}
/*
 * 05:55:49 **** Incremental Build of configuration Debug for project boostshm ****
make all
Building file: ../src/boostshm.cpp
Invoking: GCC C++ Compiler
g++ -I/usr/local/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/boostshm.d" -MT"src/boostshm.d" -o "src/boostshm.o" "../src/boostshm.cpp"
Finished building: ../src/boostshm.cpp
Building target: boostshm
Invoking: GCC C++ Linker
g++ -L/usr/local/lib -o "boostshm"  ./src/boostshm.o   -lboost_program_options -lrt -lpthread
Finished building target: boostshm
05:55:52 Build Finished (took 2s.352ms)
 */
