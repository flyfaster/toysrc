//#include "afxwin.h"
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/gbsizer.h>
#include <wx/textctrl.h>
#include <wx/Panel.h>
#include <wx/fileconf.h>
#include <map>
#include <sstream>
#include <algorithm>
#include <list>
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include "toolhelp_iterator.h"
#include "SetEventApp.h"
#include "SetEventFrame.h"
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "Rpcrt4.lib")

LPCTSTR DUMP_PATH_CONFIG_VALUE = wxT("DumpPathHistory");
IMPLEMENT_APP_NO_MAIN(SetEventApp)


BEGIN_EVENT_TABLE(SetEventApp, wxApp)
	        //EVT_QUERY_END_SESSION (SetEventApp::OnQueryEndSession)
	        //EVT_END_SESSION (SetEventApp::OnEndSession)
	        EVT_CLOSE (SetEventApp::OnClose)
END_EVENT_TABLE()

SetEventApp::SetEventApp(void) : m_dump_path_history(8)
{
    m_main_wnd=NULL;
}

SetEventApp::~SetEventApp(void)
{
    delete wxConfigBase::Set(NULL);
}


bool SetEventApp::OnInit()
{
    wxString config_file_path;
    {
        TCHAR module_path_buf[MAX_PATH+1024];
        DWORD count = GetModuleFileName(NULL, module_path_buf, MAX_PATH);
        if (count>0)
        {
            _tcscat(module_path_buf, _T(".ini"));
            config_file_path = module_path_buf;
        }
        else
        {
            wxLogError(_T("Failed to get module path"));
            return false;
        }
    }
    config_file_path.Replace( _T("\\"), _T("/"));
    wxFileConfig *conf = new wxFileConfig(wxT("SetEvent.exe"), 
        wxT("Onega"),
        config_file_path, // wxT("D:/workspace/svn/mytoybox/SetEvent/Release/aaa.ini") 
        wxEmptyString, 
        wxCONFIG_USE_LOCAL_FILE); //wxCONFIG_USE_GLOBAL_FILE |  wxCONFIG_USE_RELATIVE_PATH
    wxConfigBase::Set(conf);

    wxString ignore_path_list = wxConfigBase::Get()->Read(OPTIONS_IgnorePathList);
    bool enable_ignore_path = (0!=wxConfigBase::Get()->Read(OPTIONS_EnableIgnoreList, true));;
    if (enable_ignore_path && ignore_path_list.length()==0)
    {
        std::basic_stringstream<TCHAR> ts;
        ts << wxT("C:\\Windows\n")
            << wxT("c:\\Program Files\n")
            << wxT("c:\\Documents and Settings\n")
            ;
        ignore_path_list = ts.str();
        ignore_path_list.Replace(wxT("\n"), wxT(";"));
        wxConfigBase::Get()->Write(OPTIONS_IgnorePathList, ignore_path_list);
        wxConfigBase::Get()->Flush();
    }

    LoadDumpPathHistory();
    //wxConfigBase::Get()->Write(wxT("StartTime"), wxDateTime::Now().FormatISODate() +_T(" ") + wxDateTime::Now().FormatISOTime());
    //wxConfigBase::Get()->Flush();
    GetProcessList(m_processes);
	SetExitOnFrameDelete(true);
	SetEventFrame* mainframe = new SetEventFrame;
	mainframe->Center();
	mainframe->Show();
    m_main_wnd = mainframe;
	SetTopWindow(mainframe);
	return true;
}

void SetEventApp::OnClose( wxCloseEvent& event )
{
    m_main_wnd->Close();
}

void SetEventApp::ExitMainLoop()
{
	 //instead of existing wxWidgets main loop, terminate the MFC one
	::PostQuitMessage(0);

}

void SetEventApp::GetProcessList( std::multimap<wxString, DWORD>& processes )
{
    wxString ignore_path_list = wxConfigBase::Get()->Read(OPTIONS_IgnorePathList);
    bool enable_ignore_path = (0!=wxConfigBase::Get()->Read(OPTIONS_EnableIgnoreList, true));;
    std::list<tchar_string> ignore_list;
    if (enable_ignore_path && ignore_path_list.Length())
    {
        typedef boost::tokenizer<boost::char_separator<TCHAR>,
            tchar_string::const_iterator,
            tchar_string > tokenizer;
        boost::char_separator<TCHAR> sep( _T(";"));
        tchar_string str = ignore_path_list.wx_str();
        tokenizer tokens(str, sep);
        tokenizer::iterator tok_iter = tokens.begin();
        tchar_string one_line;
        while(tok_iter!=tokens.end())
        {
            one_line = *tok_iter++;
            if (one_line.length())
            {
                ignore_list.push_back(one_line);
            }
        }

    }

    processes.clear();
    typedef std::pair <wxString, DWORD> Data_Pair;
    for(CProcessIterator i(0); i!=CProcessIterator::End; i++)
    {
        CModuleIterator j(i->th32ProcessID);
        CModuleIterator module_end;
        while(j != module_end)
        {
            std::basic_string<TCHAR> moduel_path = j->szExePath;
            if (moduel_path.find(_T(".exe"))!=moduel_path.npos)
            {
                bool skip = false;
                for (std::list<tchar_string>::const_iterator it=ignore_list.begin();
                    it!=ignore_list.end();
                    it++)
                {
                    if (boost::algorithm::icontains( moduel_path, (*it)  ))
                    {
                        skip = true;
                        break;
                    }
                }
                if (!skip)
                {
                    processes.insert(Data_Pair( wxString(moduel_path), i->th32ProcessID));
                }
            }
            ++j;
        }
    }
}

std::basic_string<TCHAR> SetEventApp::SystemErrorToString( DWORD error_code )
{
    LPVOID lpMsgBuf = NULL;
    TCHAR buf[256 + 32]; // OS error description is less than 256 chars.
    std::basic_string<TCHAR> error_msg;
    BOOL ret = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error_code,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and exit the process
    if ((!ret) || (lpMsgBuf == NULL))
    {
        DWORD ec = GetLastError();
        _stprintf_s(buf, sizeof(buf), _T("OS Error code: %d, FormatMessage error %d."), 
            error_code, ec);
    }
    else 
    {
        _stprintf_s(buf, sizeof(buf), _T("OS Error code: %d, description: %s"), 
            error_code, lpMsgBuf);
    }
    LocalFree(lpMsgBuf);
    error_msg = buf;  
    return error_msg;
}

void SetEventApp::LoadDumpPathHistory()
{
    wxString dumppathlist;
    dumppathlist = wxConfigBase::Get()->Read(DUMP_PATH_CONFIG_VALUE, wxEmptyString);
    typedef boost::tokenizer<boost::char_separator<TCHAR>,
        tchar_string::const_iterator,
        tchar_string > tokenizer;
    boost::char_separator<TCHAR> sep( _T(";"));
    tchar_string str = dumppathlist.wx_str();
    tokenizer tokens(str, sep);
    tokenizer::iterator tok_iter = tokens.begin();
    tchar_string one_line;
    while(tok_iter!=tokens.end())
    {
        one_line = *tok_iter++;
        if (one_line.length())
        {
            m_dump_path_history.push_back(one_line);
        }
    }
}

void SetEventApp::FlushConfig()
{
    std::basic_stringstream<TCHAR> ts;
    for(boost::circular_buffer<tchar_string>::const_iterator it=m_dump_path_history.begin();
        it!=m_dump_path_history.end();
        it++)
    {
        ts << *it << wxT(";");
    }
    wxConfigBase::Get()->Write(DUMP_PATH_CONFIG_VALUE, ts.str().c_str());
    wxConfigBase::Get()->Flush();
}

bool SetEventApp::AddDumpHistory( tchar_string& dump_path )
{
    if (dump_path.length())
    {
        if (std::find(m_dump_path_history.begin(),
            m_dump_path_history.end(), 
            dump_path)==m_dump_path_history.end())
        {
            m_dump_path_history.push_back(dump_path);
            FlushConfig();
            return true;
        }
    }
    return false;
}

void SetEventApp::AddOptionsListener( IOptionsListener* listener )
{
    m_options_listener.push_back(listener);
}

void SetEventApp::FireOptionsChanged()
{
    GetProcessList(m_processes);
    for_each(m_options_listener.begin(), 
        m_options_listener.end(), 
        boost::mem_fn(&IOptionsListener::OnOptionsChanged) 
        );
}
