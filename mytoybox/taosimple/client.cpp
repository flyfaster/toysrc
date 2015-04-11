#include "taosimpleC.h"
#include "ace/Get_Opt.h"
#include "ace/Read_Buffer.h"
#include "ace/OS_NS_unistd.h"
#include "tao/IORTable/IORTable.h"
#include "tao/debug.h"
#include "ace/OS_NS_stdio.h"
#include "ace/OS_NS_unistd.h"
#include "ace/OS_NS_string.h"
#include "ace/OS_NS_fcntl.h"
#include <iostream>
#include <deque>
#include <sstream>
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

int ACE_TMAIN(int argc, ACE_TCHAR *argv[])
{
  if(argc<2){
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("Usage: %s <IOR file> <number of clients>\n"), argv[0]));
  return -1;
  }
  ACE_CString ior_ = read_ior(argv[1]);  
  CORBA::ORB_var orb_;
  orb_ = CORBA::ORB_init (argc, argv);
  std::deque<taosimple_var> taoclients;
  int numclients=100;
  if(argc>2)
    numclients = atoi(argv[2]);
  std::stringstream ss;
  ss << "netstat -anp >> "<<argv[0]<<"before"<<numclients<<".txt 2>&1";
  system(ss.str().c_str());
  ss.str(std::string());
  for(int i=0; i<numclients; i++)
  {
    taosimple_var client;  
    CORBA::Object_var server_object =
    orb_->string_to_object (ior_.c_str ());
    if (CORBA::is_nil (server_object.in ()))
      ACE_ERROR_RETURN ((LM_ERROR,
			  ACE_TEXT ("invalid ior <%C>\n"),
			  ior_.c_str ()),
			-1);
    client = taosimple::_narrow (server_object.in ());
    if (CORBA::is_nil (client.in ()))
    {
      ACE_ERROR_RETURN ((LM_ERROR,
			  ACE_TEXT ("Nil Server\n")),
			-1);
    }
    taoclients.push_back(client);
  }
  for(std::deque<taosimple_var>::iterator it=taoclients.begin();
      it!=taoclients.end();
      it++)
      {
  CORBA::String_var  message;
  (*it)->send_message("aaa", message);
  std::cout << "Corba method return " << message.in() <<std::endl;  
      }
  //client->shutdown();
    ss << "netstat -anp >> "<<argv[0]<<"after"<<numclients<<".txt 2>&1";
  system(ss.str().c_str());
}
