#include "taosimpleC.h"
#include "ace/Get_Opt.h"
#include "ace/Read_Buffer.h"
#include "ace/OS_NS_unistd.h"
//#include "tao/IORTable/IORTable.h"
#include "tao/debug.h"
#include "tao/PortableServer/PortableServer.h"
#include "orbsvcs/Naming/Naming_Client.h"
#include "ace/OS_NS_stdio.h"
#include "ace/OS_NS_unistd.h"
#include "ace/OS_NS_string.h"
#include "ace/OS_NS_fcntl.h"
#include <iostream>
#include <deque>
#include <sstream>
#include "taosimpleS.h"

// $ACE_ROOT/TAO/docs/tutorials/Quoter/AMI/client.cpp:
/*
 * onega@onega-VirtualBox:~/projects/AMI$ export TAO_ROOT=$ACE_ROOT/TAO
onega@onega-VirtualBox:~/projects/AMI$ export CIAO_ROOT=$TAO_ROOT/CIAO
onega@onega-VirtualBox:~/projects/AMI$ export DANCE_ROOT=$TAO_ROOT/DAnCE
onega@onega-VirtualBox:~/projects/AMI$ $ACE_ROOT/MPC/mpc.pl -type make Quoter_AMI.mpc

 * 
 */
class AMI_Handler_i : public POA_AMI_taosimpleHandler
{
public:
  AMI_Handler_i (int *response_count):response_count_(response_count)
  {};

  void send_message (CORBA::Boolean ami_return_val, const char* msg)
  {
    std::cout << __FUNCTION__ << " "<< msg<< " " << *response_count_ <<std::endl;
    (*this->response_count_)++;
  }

  void send_message_excep (::Messaging::ExceptionHolder * excep_holder)
  {
    try{
      excep_holder->raise_exception();
    } catch (CORBA::SystemException& e)
    {
    std::cerr << __FUNCTION__ << " Exception:"<<e<<std::endl;
    }
    catch (CORBA::UserException& e)
    {
    std::cerr << __FUNCTION__ << " Exception:"<<e<<std::endl;
    }
    (*this->response_count_)++;
  }
private:
  int *response_count_;
};


ACE_CString read_ior (ACE_TCHAR *filename)
{
  ACE_CString ior_;  
  ACE_HANDLE f_handle = ACE_OS::open (filename, 0);
  if (f_handle == ACE_INVALID_HANDLE)
  {
    ACE_ERROR ((LM_ERROR,
                       ACE_TEXT ("Unable to open %s for writing (%s)\n"),
                       filename,
                       ACE_TEXT ("open")));
    return ior_;
  }
  ACE_Read_Buffer ior_buffer (f_handle, true);
  char *data = ior_buffer.read ();
  if (data == 0)
  {
    ACE_ERROR ((LM_ERROR,
                       ACE_TEXT ("Empty file %s\n"),
                       filename));
    return ior_;
  }
  ior_ = data;
  ior_buffer.alloc ()->free (data);
  ACE_OS::close (f_handle);
  return ior_;
}
void print_status()
{
  FILE *fp=fopen("/proc/self/status", "r");
  char buf[1024];
  while(fgets(buf, sizeof(buf), fp)!=NULL)
  {
    std::cout<< buf;
  }
  fclose(fp);
}
void run_cmd(const char* cmdline)
{
  FILE* stream = popen(cmdline, "r");
  char buf[1024];
  while (stream && !feof(stream))
  {
    if(fgets(buf, sizeof(buf), stream)!=NULL)
      std::cout<<buf;
    usleep(10);
  }
  pclose(stream);
}
int ACE_TMAIN(int argc, ACE_TCHAR *argv[])
{
  std::cout << __FUNCTION__ << std::endl;
  if(argc<2){
  ACE_DEBUG ((LM_DEBUG,ACE_TEXT ("Usage: %s <IOR file> <number of clients>\n"), argv[0]));
  return -1;
  }
  
  CORBA::ORB_var orb_;
  orb_ = CORBA::ORB_init (argc, argv);
  ACE_DEBUG ((LM_DEBUG,ACE_TEXT ("%s after ORB_init\n"), argv[0]));
  
      CORBA::Object_var poa_object =
      orb_->resolve_initial_references ("RootPOA");
    PortableServer::POA_var poa =
      PortableServer::POA::_narrow (poa_object.in ());
    PortableServer::POAManager_var poa_manager =
      poa->the_POAManager ();
    poa_manager->activate ();
   ACE_DEBUG ((LM_DEBUG,ACE_TEXT ("%s after the_POAManager activate\n"), argv[0]));
 print_status();
  
  std::deque<taosimple_var> taoclients;
  int numclients=8;
  if(argc>2)
    numclients = atoi(argv[2]);
  std::stringstream ss;
  ss << "netstat -anp >> "<<argv[0]<<"before"<<numclients<<".txt 2>&1";
  //system(ss.str().c_str());
  ss.str(std::string());
  
    ACE_CString ior_ = read_ior(argv[1]);  

    CORBA::Object_var naming_context_object=orb_->resolve_initial_references("NameService");
    CosNaming::NamingContext_var naming_context=CosNaming::NamingContext::_narrow(naming_context_object);
    CosNaming::Name name(2);
    name.length(2);
    name[0].id = CORBA::string_dup("TAO_CORBA");
    name[1].id = CORBA::string_dup(argv[1]);
    
    
 std::deque<PortableServer::Servant_var<AMI_Handler_i> > servants;
 int replycount=0;
  for(int i=0; i<numclients; i++)
  {
 //   PortableServer::Servant_var<AMI_Handler_i> replyHandler_servant=new AMI_Handler_i(&replycount);
    //PortableServer::ObjectId_var oid=poa
    taosimple_var client;  
    CORBA::Object_var server_object;
    if (ior_.length())
    {
    server_object = orb_->string_to_object (ior_.c_str ());
    ACE_DEBUG ((LM_ERROR,
			  ACE_TEXT ("invalid ior <%C>\n"),
			  ior_.c_str ()));
    }
    if (CORBA::is_nil (server_object.in ()))
    {      
      server_object = naming_context->resolve(name);
      if (CORBA::is_nil (server_object.in ()))
	ACE_ERROR_RETURN ((LM_ERROR,
			  ACE_TEXT ("failed to resolve from NameService\n")),
			-1);
	///
	ACE_DEBUG ((LM_ERROR,
			  ACE_TEXT ("successfully resolve %s from NameService\n"),
			  argv[1]));
    }
    client = taosimple::_narrow (server_object.in ());
    if (CORBA::is_nil (client.in ()))
    {
      ACE_ERROR_RETURN ((LM_ERROR,
			  ACE_TEXT ("Nil Server\n")),
			-1);
    }
    taoclients.push_back(client);
  }
  ACE_DEBUG ((LM_DEBUG,ACE_TEXT ("%s after string_to_object\n"), argv[0]));
  
  AMI_Handler_i* amihandler= new AMI_Handler_i(&replycount);
  for(std::deque<taosimple_var>::iterator it=taoclients.begin();
      it!=taoclients.end();
      it++)
      {
  CORBA::String_var  message;
 // (*it)->send_message("aaa", message);
  (*it)->sendc_send_message(amihandler->_this(), "aa", "bbb");
  //std::cout << "Corba method return " << message.in() <<std::endl;  
      }
  
    while(replycount<taoclients.size())
    {
      if(orb_->work_pending())
	orb_->perform_work();
    }
    std::cout << "all done\n";
}
