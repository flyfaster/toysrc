#include "guicopy_include.h"
#include "mydlg.h"
#include "mainapp.h"
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#ifdef _MSC_VER
#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib, "rpcrt4.lib")
#pragma comment(lib, "comctl32.lib")
#endif


IMPLEMENT_APP(PathUtilApp)

tchar_string to_tchar_string(const std::string& src )
{
#ifdef _UNICODE
    std::wstring s;
    s.assign(src.begin(), src.end());
    return s;
#else
    return src;
#endif
}

tchar_string to_tchar_string(const std::wstring& src )
{
#ifdef _UNICODE
    return src;
#else
    std::string s;
    s.assign(src.begin(), src.end());
    return s;
#endif
}

std::string to_string(const tchar_string& src )
{
#ifdef _UNICODE
    std::string s;
    s.assign(src.begin(), src.end());
    return s;
#else
    return src;
#endif

}
bool PathUtilApp::IsDirExists(wxString filename)
{
	std::string ansi_filename = wxConvLocal.cWX2MB(filename.c_str());
	return (boost::filesystem::exists(boost::filesystem::path(ansi_filename, boost::filesystem::native)));

}
bool PathUtilApp::OnInit()
{//return false;
    //if (this->argc>1)
    //{
    //    RunConsole();
    //    return false;
    //}
	int buf_length = MAX_PATH + 1024;
	boost::scoped_array<TCHAR> path_buf(new TCHAR[buf_length]);
	DWORD ret = GetModuleFileName(NULL, path_buf.get(), buf_length);
	if (ret<=0)
		return false;
	ModulePath = path_buf.get();
	size_t path_separator_pos = ModulePath.rfind('\\');
    if (path_separator_pos!=tchar_string::npos)
	{
		ModuleName = ModulePath.substr(path_separator_pos+1);
		ModuleDirectory = ModulePath.substr(0, path_separator_pos+1);
	}
	else
	{	// path separator not found, this should not happen on windows
		return false;
	}

	if(! ParseOptions() )
		return false;

	CreateShm();
	SingleThreadsCol::Instance().Center(wxBOTH);
	SingleThreadsCol::Instance().Show(true);
	SetTopWindow(&SingleThreadsCol::Instance());

	return true;
}

int PathUtilApp::OnExit()
{
	return 0;
}

void PathUtilApp::Dump( const tchar_string msg )
{
	wxLogDebug(msg.c_str());
	SingleThreadsCol::Instance().Dump(msg);
	tout << msg << std::endl;
    OutputDebugString(msg.c_str());
}

void PathUtilApp::OnKeyDown( wxKeyEvent& event )
{
	SingleThreadsCol::Instance().OnKeyDown(event);
}

void PathUtilApp::HandleEvent( wxEvtHandler *handler, wxEventFunction func, wxEvent& event ) const
{
	// required when wxUSE_EXCEPTIONS = 1
	if (handler && func)
		(handler->*func)(event);
}

PathUtilApp::PathUtilApp()
{
	m_attach_console = FALSE;

}

void PathUtilApp::SetSizer( wxWindow* wnd, wxSizer* sizer )
{
	wnd->SetSizer(sizer);
	sizer->SetSizeHints(wnd);
	sizer->Fit(wnd);
}

wxCloseEvent evt;
void PathUtilApp::Close()
{
	//SingleThreadsCol::Instance().Close(true);
    Dump(to_tchar_string(__FUNCTION__ " start"));
    evt.SetCanVeto(false);
    evt.SetEventObject(&SingleThreadsCol::Instance());
    wxPostEvent(&SingleThreadsCol::Instance(), evt);
    Dump(to_tchar_string(__FUNCTION__ " end"));
}

BEGIN_EVENT_TABLE(PathUtilApp, wxApp)
EVT_KEY_DOWN(PathUtilApp::OnKeyDown)
EVT_CHAR(PathUtilApp::OnKeyDown)
EVT_KEY_UP(PathUtilApp::OnKeyDown)
END_EVENT_TABLE()

LPCTSTR PathUtilApp::GetDefaultConfigFileName()
{
	return wxT("PathUtil.xml");
}

long long PathUtilApp::GetDirSize( const tchar_string sdir_path )
{
	using namespace boost::filesystem;
	__int64 total_size = 0;
	tpath dir_path(sdir_path, boost::filesystem::native);
	tdirectory_iterator end_itr; // default construction yields past-the-end
	for ( tdirectory_iterator itr( dir_path );
		itr != end_itr;
		++itr )
	{
		if ( is_directory(itr->path()) )
		{
			total_size += GetDirSize(itr->path().directory_string());
		}
		else 
		{
			if (is_regular_file(itr->status())) 
				total_size += file_size(itr->path());
		}
	}

	return total_size;
}

bool PathUtilApp::ParseOptions()
{
    if (__argc<2)
    {
        return true; // run without arguments
    }
    AttachToConsole();
	boost::program_options::options_description options("Allowed options");
	options.add_options()
		("help,h", "print help messages")
		("copy-source-dir", boost::program_options::value<std::string>(),"set source directory")
		("copy-dest-dir", boost::program_options::value<std::string>(), "set destination directory")
		("dir-size", boost::program_options::value<std::string>(), "list size of sub-directory of given directory")
        ("src", boost::program_options::value<std::string>(), "specify source file/folder")
        ("dst", boost::program_options::value< std::string>(), "specify destination")
		;
	boost::program_options::variables_map vm;
    try
    {
        boost::program_options::store(boost::program_options::parse_command_line(__argc, __argv, options), vm);
        boost::program_options::notify(vm);
    }
    catch (const std::exception& e)
    {
        std::cout << __FUNCTION__ << " exception " << e.what() << std::endl;
        return false;
    }
	if (vm.count("help"))
	{
		std::cout << options << std::endl;
		return false;
	}
	if (vm.count("dir-size"))
	{
		using namespace boost::filesystem;
		std::string work_dir = vm["dir-size"].as<std::string>();
		std::cout << "check size of " << work_dir << std::endl;
		tpath dir_path(to_tchar_string(work_dir), boost::filesystem::native);
		bool is_dir = boost::filesystem::is_directory(dir_path);
		if (!is_dir)
		{
			std::cout << work_dir << " is not a directory." << std::endl;
			return false;
		}
		tdirectory_iterator end_itr; // default construction yields past-the-end
		for ( tdirectory_iterator itr( dir_path );
			itr != end_itr;
			++itr )
		{
			if ( is_directory(itr->path()) )
			{
				__int64 dir_size = GetDirSize(itr->path().directory_string());
				tout << itr->path().directory_string() 
					<< wxT(" size is ") << dir_size/(1024*1024.0) << wxT(" MB.") <<std::endl;
			}
			else 
			{
				continue;
			}
		}

	}
    if (vm.count("src") && vm.count("dst"))
    {
        std::string src = vm["src"].as<std::string>();
        std::string dst = vm["dst"].as<std::string>();
        std::cout << "Copy " << src << " to " << dst << std::endl;
        // case 1 src is file name, dst does not end with "\\"
        // case 2 src is file name, dst ends with "\\"
        // case 3 src is folder, dst does not exist
        // case 4 src is folder, dst exists
        boost::filesystem::path src_path(src, boost::filesystem::native);
        boost::filesystem::path dst_path(dst, boost::filesystem::native);
        try 
        {
            copy_changed_file(src_path, dst_path);
        }
        catch(const std::exception& e)
        {
            std::cout << __FUNCTION__ << " exception " << e.what() << std::endl;
        }
    }
	if (vm.size())
		return false;

	return true;
}

PathUtilApp::~PathUtilApp()
{
	if (m_attach_console)
	{
		FreeConsole();
		m_attach_console = FALSE;
		//simulate return pressing
		INPUT input;
		memset(&input,0,sizeof(input));
		input.type=INPUT_KEYBOARD;
		input.ki.wVk=VK_RETURN;
		SendInput(1,&input,sizeof(input));
		input.ki.dwFlags=KEYEVENTF_KEYUP;
		SendInput(1,&input,sizeof(input));
	}
}

tchar_string PathUtilApp::GetModulePath()
{
	return ModulePath;
}

tchar_string PathUtilApp::GetMemoryString( const tchar_string& mem_name )
{
    Dump( to_tchar_string( __FUNCTION__));
	using namespace boost::interprocess;
	int mem_size = 16*1024;
	const TCHAR* content = NULL;
	try
	{
		windows_shared_memory remote_mem(open_only, to_string(mem_name).c_str(),
			read_write);
		mapped_region region(remote_mem, read_write, 0, mem_size, 0);
		content = reinterpret_cast<const TCHAR*>(region.get_address());
		if (content)
			return content;
        else
            Dump( to_tchar_string(__FUNCTION__ " content is empty"));

	}
	catch (const std::exception& e)
	{
        wxLogError( wxString::FromAscii(__FUNCTION__ " Failed to open %s, error %s"), mem_name.c_str(), 
            wxString::FromAscii(e.what()));
	}
	return wxT("");
}

bool PathUtilApp::UpdateMemoryString( const tchar_string& mem_name, const tchar_string& content )
{
	using namespace boost::interprocess;
	int mem_size = 16*1024;
	TCHAR* content_buf = NULL;
	try
	{
		windows_shared_memory remote_mem(open_only, to_string(mem_name).c_str(),
			read_write);
		mapped_region region(remote_mem, read_write, 0, mem_size, 0);
		content_buf = reinterpret_cast<TCHAR*>(region.get_address());
		if (content_buf)
		{
			_tcscpy(content_buf, content.c_str());
			return true;
		}
	}
	catch (const std::exception& e)
	{
        wxLogError(wxT("%s(%s, %s) exception %s"), wxString::FromAscii(__FUNCTION__),
            mem_name.c_str(), content.c_str(), wxString::FromAscii(e.what()));
	}
	return false;
}

int mem_size = 16*1024;
void PathUtilApp::CreateShm()
{
	using namespace boost::interprocess;
	TCHAR* content_buf = NULL;
	try
	{
		TCHAR mem_name[MAX_PATH];
		_stprintf(mem_name,  wxT("%s.%d"), ModuleName.c_str(), GetCurrentProcessId());
		m_shm.reset(new windows_shared_memory(create_only, 
			to_string(ModuleName).c_str(),
			read_write, 
			mem_size)) ;
        wxLogInfo(wxT("%s create shared memory %s"), wxString::FromAscii(__FUNCTION__), ModuleName.c_str());
		mapped_region region(*m_shm.get(), read_write, 0, mem_size, 0);
		content_buf = reinterpret_cast<TCHAR*>(region.get_address());
		if (content_buf)
		{
			ExportProperty(to_tchar_string("mem_size"), mem_size);
			//return true;
		}
		else
		{
			wxLogError( wxT("Failed to create windows shared memory with name:%s"), 
                ModuleName.c_str());
		}
	}
	catch (const std::exception& e)
	{
        wxLogError( wxString::FromAscii(e.what()));
	}
}

void PathUtilApp::ExportProperty( const tchar_string& name, int& value_storage )
{
	// export it
	//using namespace boost::interprocess;
	//tstringstream ss;
 //   m_properties[name] = &value_storage;
	//for(PropertySet::const_iterator it=m_properties.begin();
	//	it!=m_properties.end();
	//	it++)
	//{
	//	ss << it->first << _T("=") << (*it->second) << std::endl;
	//}
	//TCHAR* content_buf = NULL;
	//mapped_region region(*m_shm.get(), read_write, 0, mem_size, 0);
	//content_buf = reinterpret_cast<TCHAR*>(region.get_address());
	//if (content_buf)
	//{
	//	_tcscpy(content_buf, ss.str().c_str());
	//	//return true;
	//}
	//else
	//{
	//	wxLogError( _T("Failed to access windows shared memory with name:%s"), 
	//		m_shm->get_name());
	//}
}

void PathUtilApp::RefreshProperties()
{
	using namespace boost::interprocess;
	TCHAR* content_buf = NULL;
	tchar_string s1;
	tchar_string name;
	tchar_string vl;
	bool parse_error = true;
	mapped_region region(*m_shm.get(), read_write, 0, mem_size, 0);
	content_buf = reinterpret_cast<TCHAR*>(region.get_address());
	if (content_buf)
	{
		tstringstream ss;
		ss << content_buf;
		while(true)
		{
			parse_error = true;
			s1.clear();
			std::getline(ss, s1);
			tchar_string::size_type pos = s1.find(_T('='));
			if (pos!=std::string::npos)
			{
				name = s1.substr(0,pos);
				vl = s1.substr(pos+1);
				//PropertySet::iterator it=m_properties.find(name);
				//if (it!=m_properties.end() && vl.length())
				//{
				//	*(it->second) = _ttoi(vl.c_str());
				//	parse_error = false;
				//}
			}
			if (parse_error)
			{
				wxLogError( _T("%s error: invalid string %s"), 
                    wxString::FromAscii(__FUNCTION__), 
					s1.c_str());
			}
			if (ss.eof())
				break;
		}
		
	}
	else
	{
		wxLogError( wxT("Failed to access windows shared memory with name:%s"), 
			m_shm->get_name());
	}

}

void PathUtilApp::copy_changed_file(boost::filesystem::path src_path, boost::filesystem::path dst_path)
{
    using namespace boost::filesystem;
    if (boost::filesystem::exists(src_path)==false)
    {
        std::cout << "Source file (" << src_path << ") does not exist.\n";
        return;
    }
    std::cout << __FUNCTION__ "(" << src_path <<"," << dst_path<<")" << std::endl;
    std::time_t src_last_modify = boost::filesystem::last_write_time(src_path);
    boost::uintmax_t src_size = boost::filesystem::file_size(src_path);

    if (boost::filesystem::exists(dst_path)==false )
    {
        std::cout << __FUNCTION__ " not exist " << dst_path << std::endl;
        if ( boost::ends_with(dst_path.string(), "/")==false)
        {
            copy_file(src_path,dst_path,copy_option::overwrite_if_exists);
            last_write_time(dst_path, src_last_modify);
            return;
        }
        else
        {
            // mkdir and copy file
            bool bc = create_directories(dst_path);
            if (!bc)
            {
                std::cout << __FUNCTION__ " failed to create " << dst_path << std::endl;
                return;
            }
            path dst_file_path(dst_path.string()+src_path.filename());
            copy_file(src_path,dst_file_path,copy_option::overwrite_if_exists);
            last_write_time(dst_file_path, src_last_modify);
            return;
        }
    }
    else
    {
        if (boost::filesystem::is_directory(dst_path))
        {
            path dst_file_path(dst_path.normalize().string()+"/"+src_path.filename());
            copy_changed_file(src_path, dst_file_path);
            return;
        }
        if (boost::filesystem::is_regular_file(dst_path))
        {
            std::time_t dst_last_modify = last_write_time(dst_path);
            boost::uintmax_t dst_size = file_size(dst_path);
            if (src_last_modify==dst_last_modify && src_size==dst_size)
            {
                std::cout << __FUNCTION__ " Skip " << src_path.string()<<std::endl;
            }
            else
            {
                std::cout <<__FUNCTION__ " now copy " << src_path.string() << " to " << dst_path<<std::endl;
                copy_file(src_path,dst_path,copy_option::overwrite_if_exists);
                last_write_time(dst_path, src_last_modify);
            }
            return;
        }
    }
}

void PathUtilApp::AttachToConsole()
{
    m_attach_console = AttachConsole(ATTACH_PARENT_PROCESS);
    if (!m_attach_console)
    {
        std::basic_stringstream<TCHAR> ss;
        ss << _T("AttachConsole return ") << GetLastError();
        OutputDebugString(ss.str().c_str());
    }
    else
    {
        HANDLE output_handle = GetStdHandle(STD_ERROR_HANDLE);
        if (output_handle==INVALID_HANDLE_VALUE)
        {
            std::basic_stringstream<TCHAR> ss;
            ss << _T("GetStdHandle return ") << GetLastError();
            OutputDebugString(ss.str().c_str());
        }
        else
        {
            std::cout.sync_with_stdio(true);
            int m_nCRTOut= _open_osfhandle( (intptr_t)output_handle, 0 );
            if (m_nCRTOut==-1)
            {
                std::basic_stringstream<TCHAR> ss;
                ss << _T("_open_osfhandle(") <<(int) output_handle << _T(") return ") << errno << _T(", ")
                    << strerror( errno);
                OutputDebugString(ss.str().c_str());
            }
            else
            {
                FILE* m_fpCRTOut = _fdopen( m_nCRTOut, "w" );
                if (!m_fpCRTOut)
                {
                    std::basic_stringstream<TCHAR> ss;
                    ss << _T("_fdopen(") <<(int) m_nCRTOut << _T(") return ") << errno << _T(", ")
                        << strerror( errno);
                    OutputDebugString(ss.str().c_str());

                }
                else
                {
                    *stdout = *m_fpCRTOut;
                    setvbuf( stdout, NULL, _IONBF, 0 );
                }
            }
        }
        std::cout.clear();
    }
    std::cout << __FUNCTION__ << std::endl;
}

