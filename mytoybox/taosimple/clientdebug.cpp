#include "taosimpleC.h"
#include "ace/Get_Opt.h"
#include "ace/Read_Buffer.h"
#include "ace/OS_NS_unistd.h"
#include "tao/IORTable/IORTable.h"
#include "tao/debug.h"
#include "orbsvcs/orbsvcs/Naming/Naming_Client.h"
#include "ace/OS_NS_stdio.h"
#include "ace/OS_NS_unistd.h"
#include "ace/OS_NS_string.h"
#include "ace/OS_NS_fcntl.h"
#include "ace/ARGV.h"
#include "ace/Argv_Type_Converter.h"
#include <iostream>
#include <deque>
#include <sstream>
ACE_CString read_ior (const ACE_TCHAR *filename)
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
    ACE_ERROR ((LM_ERROR,
                       ACE_TEXT ("Empty file %s\n"),
                       filename));
  else
    ior_ = data;
  ior_buffer.alloc ()->free (data);
  ACE_OS::close (f_handle);
  return ior_;
}
void printusage(const char* appname)
{
    std::cout<<"usage: " << appname
      <<"--iorfile (-o) <ior file>"<<std::endl
      <<"--name (-n) <registered name in naming server>"<<std::endl
      <<"--help (-h)"<<std::endl;
}
taosimple_var get_from_ns(CORBA::ORB_ptr orb, const char* namestr)
{
    taosimple_var client;
    CORBA::Object_var obj;
    TAO_Naming_Client naming_client_;
    if (naming_client_.init (orb) != 0)
    {
        std::cerr<<"Failed to initialize TAO_Naming_Client\n";
        return client;
    }
    CosNaming::Name server_name (2);
    server_name.length (2);
    server_name[0].id = CORBA::string_dup ("TAO_CORBA");
    server_name[1].id = CORBA::string_dup (namestr);
    obj = naming_client_->resolve (server_name);
    client = taosimple::_narrow (obj.in ());
    if (CORBA::is_nil (client.in ()))
    {
        ACE_ERROR((LM_ERROR, ACE_TEXT ("Nil Server\n")));
    }
    return client;
}
int ACE_TMAIN_throw(int argc, ACE_TCHAR *argv[])
{
  ACE_CString iorfile, namestr;
    ACE_Get_Opt get_opt(argc, argv);
    get_opt.long_option("iorfile", 'o', ACE_Get_Opt::ARG_REQUIRED);
    get_opt.long_option("name", 'n', ACE_Get_Opt::ARG_REQUIRED);
    get_opt.long_option("help", 'h', ACE_Get_Opt::NO_ARG);
    int option;
    while((option=get_opt())!=EOF)
    {
      switch(option)
      {
      case 'h':
	printusage(argv[0]);
	return 0;
	break;
      case 'o':
	iorfile = get_opt.opt_arg();
	break;
      case 'n':
	namestr = get_opt.opt_arg();
	break;
      }
    } 
  if(iorfile.length() + namestr.length() < 1){
    printusage(argv[0]);
    return -1;
  }
  ACE_CString ior_ = read_ior(iorfile.c_str());  
  CORBA::ORB_var orb_;
    char orblogfile[512], current_dir[512];
  getcwd(current_dir, sizeof(current_dir));
  sprintf(orblogfile, "-ORBLogFile %s/orbclientlog%03d.txt", current_dir, 1);
  ACE_ARGV_T <char> args_list;
  args_list.add(argv);  
//   args_list.add("-ORBDebug");
//   args_list.add("-ORBDebugLevel 10");
//   args_list.add("-ORBVerboseLogging 2");
//   args_list.add(orblogfile);
  int argcnt = args_list.argc ();
  ACE_Argv_Type_Converter command_line (argcnt, args_list.argv ());
  char ** new_argv = command_line.get_ASCII_argv ();
  for( int i = 0; argcnt != i; i++ ) {
		  printf( "new_argv[%d]: %s\n", i, new_argv[ i ] );
  }  
  orb_ = CORBA::ORB_init (argcnt, new_argv);
    taosimple_var client;  
    if(ior_.length())
    {
    CORBA::Object_var server_object =
    orb_->string_to_object (ior_.c_str ());
    if (CORBA::is_nil (server_object.in ()))
      ACE_ERROR_RETURN ((LM_ERROR,
			  ACE_TEXT ("invalid ior <%C>\n"),
			  ior_.c_str ()),
			-1);
    client = taosimple::_narrow (server_object.in ());
    }
    else if(namestr.length())
    {
      client = get_from_ns(orb_.in(), namestr.c_str());
    }
    if (CORBA::is_nil (client.in ()))
    {
      ACE_ERROR_RETURN ((LM_ERROR,ACE_TEXT ("Failed to connect to Server\n")), -1);
    }
  CORBA::String_var  message;
  client->send_message("aaa", message);
  std::cout << "Corba method return " << message.in() <<std::endl;  
  client->shutdown();
}

int ACE_TMAIN(int argc, ACE_TCHAR *argv[])
{
  try{
  return ACE_TMAIN_throw(argc, argv);
  }
    catch (const CORBA::UserException& userex)
    {
      userex._tao_print_exception ("User Exception");
      return -1;
    }
  catch (const CORBA::SystemException& sysex)
    {
      sysex._tao_print_exception ("System Exception");
      return -1;
    }
  catch (const ::CORBA::Exception &e)
    {
      e._tao_print_exception ("CORBA exception");
      return 1;
    } 
    catch(const std::exception &e)
    {
      std::cerr << argv[0] << "exception: "<<e.what()<<std::endl;
      return -1;
    }
}