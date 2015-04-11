#include <iostream>
#include "tao/Utils/ORB_Manager.h"
#include "orbsvcs/Naming/Naming_Client.h"
#include "ace/Get_Opt.h"
#include "ace/Read_Buffer.h"
#include "tao/Intrusive_Ref_Count_Handle_T.h"
#include "taosimpleS.h"
#include "tao/IORTable/IORTable.h"
#include "tao/debug.h"
#include <orbsvcs/orbsvcs/CosNamingC.h>
// #include <tao/Strategies/advanced_resource.h>
#include "ace/OS_NS_stdio.h"
#include "ace/OS_NS_unistd.h"
#include "ace/OS_NS_string.h"
#include "ace/OS_NS_fcntl.h"
#include "ace/ARGV.h"
#include "ace/Argv_Type_Converter.h"
#include "ace/Signal.h"
#include <sys/syscall.h>
#include <string>
#include <time.h>

struct CorbaRuntime{
    CORBA::ORB_var orb_;
  TAO_ORB_Manager orb_manager_;
  PortableServer::POAManager_var poamgr;
  int init (const char* servant_name,
            int argc,
            ACE_TCHAR *argv[]);
  int run (void) { return orb_manager_.run ();  }
  void shutdown() { return orb_->shutdown();}
  int write_ior(const char* ior_file, const char* ior_str);
  int unbind(CosNaming::Name& name);
  int bind_iortable(const char* name, const char* ior_str);
};
int CorbaRuntime::bind_iortable(const char* name, const char* ior_str)
{
    CORBA::Object_var tableobj=orb_->resolve_initial_references("IORTable");
    IORTable::Table_var adapter=IORTable::Table::_narrow(tableobj.in());
    adapter->bind("taosimpleS", ior_str);  
    return 0;
}

int CorbaRuntime::unbind(CosNaming::Name& name)
{
    CORBA::Object_var poaobj=orb_->resolve_initial_references("RootPOA");
    PortableServer::POA_var rootpoa=PortableServer::POA::_narrow(poaobj.in());
    CORBA::Object_var rootnamingobj=orb_->resolve_initial_references("NameService");
    CosNaming::NamingContext_var rootnc=CosNaming::NamingContext::_narrow(rootnamingobj.in());
    if(CORBA::is_nil(rootnc.in()))
    {
      std::cerr<< " check if tao_cosnaming is started\n";
      return -1;
    }
    try{
      rootnc->unbind(name);
    } catch(CosNaming::NamingContext::NotFound&)
    {
      std::cout<<"Name is not found\n";
    }    
    return 0;
}

int CorbaRuntime::write_ior(const char* ior_file, const char* ior_str)
{
      FILE *fh = ACE_OS::fopen (ior_file, "w");
      if (fh == 0)
	ACE_ERROR_RETURN ((LM_ERROR,
			    ACE_TEXT ("Unable to open %s for writing (%p)\n"),
			    ior_file,
			    ACE_TEXT ("fopen")),
			  -1);
      ACE_OS::fprintf (fh, "%s", ior_str);
      ACE_OS::fclose (fh);
      return 0;
}

int CorbaRuntime::init(const char* servant_name, int argc, ACE_TCHAR* argv[])
{
  if (this->orb_manager_.init_child_poa (argc,
                                         argv,
                                         "child_poa") == -1)
    ACE_ERROR_RETURN ((LM_ERROR,
                       ACE_TEXT ("%p\n"),
                       ACE_TEXT ("init_child_poa")),
                      -1);
  orb_ = this->orb_manager_.orb ();
  return 0;
}


class taosimpleS : public POA_taosimple
{
public:
  CorbaRuntime* corbart_;
  taosimpleS (CorbaRuntime* corbart):corbart_(corbart)
  {std::cout<<__FUNCTION__<<std::endl;
  set_ior_file("taosimpleS.ior");
  }
  virtual ~taosimpleS (void)
    {std::cout<<__FUNCTION__<<std::endl;}
    virtual CORBA::Boolean send_message (const char *mesg, char*& outstr)
    {
        if (mesg == 0)
            return false;
	char buf[256];
	time_t rawtime;
	time(&rawtime);
	struct tm* tmdata=localtime(&rawtime);
	sprintf(buf, "%s %s in thread %d at %s", mesg, outstr, syscall(SYS_gettid), (const char*)asctime(tmdata));
        CORBA::String_var str = CORBA::string_dup (buf);
        if (str.in () == 0)
            throw CORBA::NO_MEMORY ();
        outstr = str._retn ();
	std::cout << __FUNCTION__ << " " <<outstr<<std::endl;
        return true;
    }
  /// Shutdown the server.
  virtual void shutdown (void)
  {
    ACE_DEBUG ((LM_DEBUG,ACE_TEXT ("%s start\n"), __FUNCTION__));
    CosNaming::Name name(2);
    name.length(2);
    name[0].id=CORBA::string_dup("TAO_CORBA");
    name[1].id=CORBA::string_dup("taosimpleS");
    corbart_->unbind(name);
  }

  int init (const char *servant_name,
            int argc,
            ACE_TCHAR *argv[]);
 
void set_ior_file(const char* filepath)
{
  strcpy(ior_output_file_, filepath);
}
public:
//   CORBA::ORB_var orb_;
//   TAO_ORB_Manager orb_manager_;
  PortableServer::POAManager_var poamgr;

  ACE_TCHAR ior_output_file_[512];
  void operator= (const taosimpleS&);
};

int taosimpleS::init (const char *servant_name, int argc, ACE_TCHAR *argv[])
{

  CORBA::String_var str  =
    corbart_->orb_manager_.activate_under_child_poa (servant_name,this);
						 // this->servant_.in ());  
  if (this->ior_output_file_[0])
    {
      corbart_->write_ior(ior_output_file_, str.in());
      ACE_DEBUG ((LM_DEBUG,ACE_TEXT ("The IOR is: <%C>\n IOR file %s\n"), str.in (), ior_output_file_));
    }
    else
      ACE_DEBUG ((LM_DEBUG,ACE_TEXT ("The IOR is: <%C>\n"), str.in ()));
 
    corbart_->bind_iortable("taosimpleS", str.in());
    CORBA::Object_var poaobj=corbart_->orb_->resolve_initial_references("RootPOA");
    PortableServer::POA_var rootpoa=PortableServer::POA::_narrow(poaobj.in());
    CORBA::Object_var rootnamingobj=corbart_->orb_->resolve_initial_references("NameService");
    CosNaming::NamingContext_var rootnc=CosNaming::NamingContext::_narrow(rootnamingobj.in());
    if(CORBA::is_nil(rootnc.in()))
    {
      std::cerr<<argv[0] << " check if tao_cosnaming is started\n";
      std::cerr<<"start tao_cosnaming in multi-cast mode\n";
      std::cerr << "$ACE_ROOT/TAO/orbsvcs/Naming_Service/tao_cosnaming -m 1\n";
      std::cerr<<"specify port number used by tao_cosnaming\n";
      std::cerr<<"tao_cosnaming -ORBEndPointiiop://127.0.0.1:12345\n"; 
      return __LINE__;
    }
    CosNaming::Name name(1);
    name.length(1);
    name[0].id=CORBA::string_dup("TAO_CORBA");
    CosNaming::NamingContext_var servernc;
    try{
    CORBA::Object_var serverncobj=rootnc->resolve(name);
    servernc=CosNaming::NamingContext::_narrow(serverncobj.in());
    } catch(CosNaming::NamingContext::NotFound&)
    {
      servernc=rootnc->bind_new_context(name);
    }
    corbart_->orb_manager_.activate_poa_manager ();
    name[0].id=CORBA::string_dup("taosimpleS");
    CORBA::Object_var object = _this ();
    servernc->rebind(name, object.in());
    std::cout<<"register to Naming Service successfully" << name[0].id <<std::endl;
    std::cout<<"check with command: LD_LIBRARY_PATH=$ACE_ROOT/lib $ACE_ROOT/bin/tao_nslist\n";
  return 0;
}
taosimpleS* server;
CorbaRuntime* rt;
static void mysignalhandler(int sig)
{
  std::cout<<__FUNCTION__<<"("<<sig<<")\n";
  if(rt)
    rt->shutdown();
  rt=0;
}
int main(int argc, char **argv) {
    std::cout << argv[0] << std::endl;
    ACE_Get_Opt get_opt(argc, argv);
    get_opt.long_option("iorfile", 'o', ACE_Get_Opt::ARG_REQUIRED);
    get_opt.long_option("help", 'h', ACE_Get_Opt::NO_ARG);
    int option;
    try
    {
        rt = new CorbaRuntime;
	rt->init("aaa", argc, argv);
        server = new taosimpleS(rt);
        typedef TAO_Intrusive_Ref_Count_Handle<taosimpleS> Servant_var;
        Servant_var servant_ = server;
        while((option=get_opt())!=EOF)
        {
            switch(option)
            {
            case 'h':
                std::cout<<"usage: " << argv[0]
                         <<"--iorfile (-o) <ior file>"<<std::endl
                         <<"--help (-h)"<<std::endl;
                return 0;
                break;
            case 'o':
                server->set_ior_file(get_opt.opt_arg());
                break;
            }
        }
        char orblogfile[512], current_dir[512];
        getcwd(current_dir, sizeof(current_dir));
        sprintf(orblogfile, "-ORBLogFile %s/orbserverlog%03d.txt", current_dir, 1);
        ACE_ARGV_T <char> args_list;
        args_list.add(argv);
        args_list.add("-ORBObjRefStyle url"); // port number is visible in
//     args_list.add("-ORBDottedDecimalAddresses 1");
        args_list.add("-ORBDebug");
        args_list.add("-ORBDebugLevel 10");
        args_list.add("-ORBVerboseLogging 2");
        args_list.add(orblogfile);
        int argcnt = args_list.argc ();
        ACE_Argv_Type_Converter command_line (argcnt, args_list.argv ());
        char ** new_argv = command_line.get_ASCII_argv ();
        if (server->init ("taosimpleS",
                          argcnt,
                          new_argv) == -1)
            return 1;
        ACE_Sig_Action sig1(mysignalhandler, SIGQUIT);
        ACE_Sig_Action sig2(mysignalhandler, SIGINT);
        rt->run ();
//         server->servant_._retn();
//         delete server;
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
    return 0;
}
