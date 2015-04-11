#include "SystemInfo.h"
#include "toolhelp_iterator.h"
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/gbsizer.h>
#include <wx/textctrl.h>
#include <wx/Panel.h>
#include <map>
#include <sstream>
#include <iomanip>
#include <Psapi.h>
#include <DbgHelp.h>
#include <boost/algorithm/string.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/filters.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/keywords/file_name.hpp>
#include <boost/log/sinks.hpp>
#include "scoped_any.h"
#include "OptionsListener.h"
#include "DumpPage.h"
#include "SetEventApp.h"
#include "client/windows/tests/crash_generation_app/crash_generation_app.h"
#include <windows.h>
#include <tchar.h>
#include <Shlobj.h>
#include "client/windows/crash_generation/client_info.h"
#include "client/windows/crash_generation/crash_generation_server.h"
#include "client/windows/handler/exception_handler.h"
#include "client/windows/common/ipc_protocol.h"
#include "client/windows/tests/crash_generation_app/abstract_class.h"

DEFINE_EVENT_TYPE(wxEVT_MY_EVENT)
BEGIN_EVENT_TABLE(DumpPage, wxPanel)
EVT_BUTTON(wxID_ANY, DumpPage::OnButton)
EVT_COMMAND  (wxID_ANY, wxEVT_MY_EVENT, DumpPage::OnLogEvent)
END_EVENT_TABLE()
const wchar_t kPipeName[] = L"\\\\.\\pipe\\BreakpadCrashServices\\TestServer";
const size_t kMaximumLineLength = 1024;

void init_logging();

DumpPage::DumpPage(wxWindow* parent):wxPanel(parent, wxID_ANY)
{
	init_logging();
    dump_file=INVALID_HANDLE_VALUE;
    m_RefreshProcess = new wxButton(this, wxID_ANY, _T("Process"));
    m_DumpButton = new wxButton(this, wxID_ANY, _T("Dump"));
    m_ClearButton = new wxButton(this, wxID_ANY, _T("Clear"));
    m_ChooseDumpPath = new wxButton(this, wxID_ANY, _T("Dump To"));
    m_ProcessList = new wxComboBox(this, wxID_ANY, wxT(""), wxDefaultPosition,
        wxDefaultSize, 0, NULL, wxCB_DROPDOWN);
    m_DumpPathList = new wxComboBox(this, wxID_ANY, wxT(""), wxDefaultPosition,
        wxDefaultSize, 0, NULL, wxCB_DROPDOWN);
    m_LogWnd = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
        wxDefaultSize, wxTE_MULTILINE|wxVSCROLL|wxSIMPLE_BORDER );

    wxGridBagSizer* sizer = new wxGridBagSizer();
    int col = 0;
    int row = 0;
    sizer->Add(m_RefreshProcess, wxGBPosition(row, col));
    col++;
    sizer->Add(m_ProcessList, wxGBPosition(row, col), wxGBSpan(1, 3), wxEXPAND | wxRIGHT);
    row++;
    col=0;
    sizer->Add(m_ChooseDumpPath, wxGBPosition(row, col));
    col++;
    sizer->Add(m_DumpPathList, wxGBPosition(row, col), wxGBSpan(1, 3), wxEXPAND | wxRIGHT);
    col=0;
    row++;
    sizer->Add(m_DumpButton, wxGBPosition(row, col));
    col++;
    sizer->Add(m_ClearButton, wxGBPosition(row, col));
    col = 0;
    row++;
    sizer->Add(m_LogWnd, wxGBPosition(row, col), wxGBSpan(1, 4), wxEXPAND | wxALL);

    for(boost::circular_buffer<tchar_string>::const_iterator it=wxGetApp().m_dump_path_history.begin();
        it!=wxGetApp().m_dump_path_history.end();
        it++)
    {
        m_DumpPathList->AppendString((*it).c_str());
    }

    sizer->AddGrowableCol(3);
    sizer->AddGrowableRow(row);
    SetSizer(sizer);
    sizer->SetSizeHints(this);
    OnRefreshProcess();
    dump_page = this;
    CrashServerStart();
}

DumpPage::~DumpPage(void)
{
}

void DumpPage::OnButton( wxCommandEvent& event )
{
    if (event.GetEventObject()==m_RefreshProcess)
    {
        wxGetApp().GetProcessList(wxGetApp().m_processes);
        OnRefreshProcess();
        return;
    }
    if (event.GetEventObject() == m_ClearButton)
    {
        m_LogWnd->SetValue(wxEmptyString);
    }
    if (event.GetEventObject()==m_ChooseDumpPath)
    {
        wxString dirHome;
        wxGetHomeDir(&dirHome);
        wxDirDialog dialog(this, wxT("Choose dump file path"), dirHome,
            wxDD_DEFAULT_STYLE & ~wxDD_DIR_MUST_EXIST);
        if (dialog.ShowModal() == wxID_OK)
        {
                m_DumpPathList->SetValue(dialog.GetPath());
        }
    }
    wxString msg;
    if (event.GetEventObject()==m_DumpButton)
    {
        dump_file_path.clear();
        // check if Process name or process id is in m_ProcessList
        process_name = m_ProcessList->GetValue().wx_str();
        if (process_name.length()==0)
        {
            wxLogWarning(wxT("Warning, process name or ID is needed to create process dump file."));
            return;
        }
        tchar_string dump_path = m_DumpPathList->GetValue().wx_str();
        if (dump_path.length()==0)
        {
            Log(wxT("Please enter dump file path.\n"));
            return;
        }
        if (::wxDirExists(m_DumpPathList->GetValue())==false)
        {
            if (::wxMkDir(m_DumpPathList->GetValue()))
            {
                msg.Printf(wxT("Failed to create folder %s.\n"), dump_path.c_str());
                Log(msg);
                return;
            }
        }
        if (wxGetApp().AddDumpHistory(dump_path))
        {
            m_DumpPathList->AppendString(m_DumpPathList->GetValue());
        }

        wxDateTime m_start_time = wxDateTime::Now();
        process_id = _ttoi(process_name.c_str());
        DWORD ec = CreateProcessDump();
        wxTimeSpan duration = wxDateTime::Now() - m_start_time;
        if (ec == ERROR_SUCCESS)
        {
            msg.Printf(_T("Created dump file %s in %d seconds\n"), dump_file_path.c_str(), duration.GetSeconds().ToLong());
        }
        else
        {
            msg.Printf(_T("Create dump for %d return %08x, description %s.\n"), process_id, ec, wxGetApp().SystemErrorToString(ec).c_str());
        }
        Log(msg);
    }

}

void DumpPage::OnRefreshProcess()
{
    std::multimap<wxString, DWORD>& processes = wxGetApp().m_processes;
    m_ProcessList->Clear();
    TCHAR buf[MAX_PATH];
    wxString formated_name;
    for (std::multimap<wxString, DWORD>::const_iterator it=processes.begin();
        it!=processes.end();
        it++)
    {
        if (it->second == 0)
            continue;
        _stprintf_s(buf, _T("%04d  %s"), it->second, it->first.wx_str() );
        m_ProcessList->AppendString(buf);
    }
}


DWORD DumpPage::OpenDumpFile()
{
    DWORD ec = ERROR_SUCCESS;
    wxString dump_dir = m_DumpPathList->GetValue();
    TCHAR temp_path_buf[MAX_PATH+1024]={0};
    DWORD len = 0;
    if (dump_file_path.length()==0)
    {
        // figure dump file path
        if (len = dump_dir.length())
        {
            wxStrcpy(temp_path_buf, dump_dir.wx_str());
        }
        else
        {
            len = GetTempPath(sizeof(temp_path_buf)/sizeof*temp_path_buf, temp_path_buf);
        }
        if (len==0)
        {
            ec = GetLastError();
            return ec;
        }
        if (len >0 && temp_path_buf[len-1]!=_T('\\'))
        {
            temp_path_buf[len]=_T('\\');
            temp_path_buf[len+1]=0;
        }
        TCHAR pc_name[64]={0};
        DWORD pc_name_buf_size = sizeof(pc_name)/sizeof*pc_name-1;
        len = GetComputerName(pc_name, &pc_name_buf_size);
        if (len <=0)
        {
            ec = GetLastError();
            //std::cout << __FUNCTION__ << " error " << SystemErrorToString(ec) 
            //    << " when get computer name " << std::endl;
            return ec;
        }
        
        tchar_string::size_type pos = process_name.find_last_of(_T("\\"));
        tchar_string filename_only;
        if (pos!=tchar_string::npos)
        {
            filename_only = process_name.substr(pos+1);
        }
        else
            filename_only = process_name;
        SYSTEMTIME stnow;
        GetLocalTime(&stnow);
        std::basic_stringstream<TCHAR> ss;
        ss << temp_path_buf << filename_only 
            << _T("_") << pc_name << _T("_") << process_id << _T("_")
            << stnow.wYear 
            << std::setw(2) << std::setfill(_T('0')) << stnow.wMonth
            << std::setw(2) << std::setfill(_T('0')) << stnow.wDay
            << _T("T")
            << std::setw(2) << std::setfill(_T('0')) << stnow.wHour
            << std::setw(2) << std::setfill(_T('0')) << stnow.wMinute
            << std::setw(2) << std::setfill(_T('0')) << stnow.wSecond
            << _T(".dmp");
        dump_file_path = ss.str();
    }
    dump_file = CreateFile(dump_file_path.c_str(), 
        GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
    if(!dump_file)
    {
        ec = GetLastError();
        //std::cout << __FUNCTION__ << " error " << SystemErrorToString(ec) 
        //    << " when get create file " << dump_file_path << std::endl;
        return ec;
    }
    return ec;
}

DWORD DumpPage::CreateProcessDump()
{
    const MINIDUMP_TYPE dumptype = (MINIDUMP_TYPE) (MiniDumpWithFullMemory 
        | MiniDumpWithHandleData | MiniDumpWithUnloadedModules | MiniDumpWithProcessThreadData);
    HANDLE process_instance=INVALID_HANDLE_VALUE;
    scoped_handle safe_close_proc_inst;
    DWORD ec = ERROR_SUCCESS;
    if (process_id == 0 && process_name.length()==0)
        return E_INVALIDARG;
    if (process_name.length()==0)
    {
        process_instance = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,NULL,process_id);
        reset(safe_close_proc_inst, process_instance);
        if (process_instance==NULL)
        {
            ec = GetLastError();
            //std::cout << __FUNCTION__ << " error " << SystemErrorToString(ec) 
            //    << " when open process pid " << process_id << std::endl;
            return ec;
        }
        TCHAR szModName[MAX_PATH + 1024];
        DWORD name_len = GetModuleBaseName( process_instance, NULL, szModName, sizeof(szModName)/sizeof*szModName);
        if (name_len == 0)
        {
            ec = GetLastError();
            //std::cout << __FUNCTION__ << " error " << SystemErrorToString(ec)
            //    << " when get module name of pid " << process_id << std::endl;
            return ec;
        }
        process_name = szModName;
    }
    if (process_id == 0)
    {
        // need to find out process ID
        for(CProcessIterator i(0); i!=CProcessIterator::End; i++)
        {
            if (i->th32ProcessID == 0)
                continue;
            CModuleIterator j(i->th32ProcessID);
            CModuleIterator module_end;
            while(j != module_end)
            {
                tchar_string module_path = j->szExePath;
                tchar_string lower_process_name = boost::to_lower_copy(process_name);
                // 
                boost::to_lower(module_path);
                if (boost::find_first(module_path, lower_process_name))
                {
                    process_id = i->th32ProcessID;                    
                    break;
                }
                ++j;
            }
            if (process_id)
                break;
        }
    }
    if (process_id==0)
    {
        ec = E_INVALIDARG;
        return ec;
    }

    if (process_instance==INVALID_HANDLE_VALUE)
    {
        process_instance = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,NULL, process_id);
        reset(safe_close_proc_inst,process_instance);
    }
    if (process_instance==NULL)
    {
        ec = GetLastError();
        //std::cout << __FUNCTION__ << " error " << SystemErrorToString(ec) 
        //    << " when open process " << process_name << ", pid " << process_id << std::endl;
        return ec;
    }

    ec = OpenDumpFile();
    if (ec != ERROR_SUCCESS)
        return ec;

    if(!MiniDumpWriteDump(process_instance, process_id, dump_file, dumptype ,NULL,NULL,NULL))
    {
        ec = GetLastError();
        //std::cout << __FUNCTION__ << " error " << SystemErrorToString(ec) 
        //    << " when create dump file " << dump_file_path << std::endl;
    }
    else
    {
        //std::cout << __FUNCTION__ << " created dump file " << dump_file_path 
        //    << ", pid: " << process_id  << std::endl;
    }
    CloseHandle(dump_file);
    dump_file=INVALID_HANDLE_VALUE;
    return ec;
}

void DumpPage::Log( const wxString& msg )
{
    m_LogWnd->AppendText(msg);
    if (msg.EndsWith(_T("\n"))==false)
        m_LogWnd->AppendText(_T("\n"));
}

void DumpPage::OnOptionsChanged()
{
    OnRefreshProcess();
}
DumpPage* DumpPage::dump_page;

void LogString(const TCHAR* line)
{
	BOOST_LOG_TRIVIAL(info) << line;
    wxCommandEvent event( wxEVT_MY_EVENT, DumpPage::dump_page->GetId() );
    event.SetEventObject( DumpPage::dump_page );
    event.SetString( line );
    wxPostEvent(DumpPage::dump_page, event);
}
using namespace google_breakpad;
static void _cdecl ShowClientConnected(void* context,
                                       const ClientInfo* client_info) {

                                           TCHAR line[kMaximumLineLength];
                                           line[0] = _T('\0');
                                           int result = swprintf_s(line,
                                               kMaximumLineLength,
                                               L"Client connected:\t\t%d\r\n",
                                               client_info->pid());

                                           LogString(line);
}

static void _cdecl ShowClientCrashed(void* context,
                                     const ClientInfo* client_info,
                                     const wstring* dump_path) {
                                         TCHAR line[kMaximumLineLength];
                                         line[0] = _T('\0');
                                         int result = swprintf_s(line,
                                             kMaximumLineLength,
                                             TEXT("Client requested dump:\t%d\r\n"),
                                             client_info->pid());                                   

                                         LogString(line);

                                         CustomClientInfo custom_info = client_info->GetCustomInfo();
                                         if (custom_info.count <= 0) {
                                             return;
                                         }

                                         wstring str_line;
                                         for (size_t i = 0; i < custom_info.count; ++i) {
                                             if (i > 0) {
                                                 str_line += L", ";
                                             }
                                             str_line += custom_info.entries[i].name;
                                             str_line += L": ";
                                             str_line += custom_info.entries[i].value;
                                         }

                                         
                                         line[0] = _T('\0');
                                         result = swprintf_s(line,
                                             kMaximumLineLength,
                                             L"%s\n",
                                             str_line.c_str());
                                         LogString(line);
}

static void _cdecl ShowClientExited(void* context,
                                    const ClientInfo* client_info) {
                                        TCHAR line[kMaximumLineLength];
                                        line[0] = _T('\0');
                                        int result = swprintf_s(line,
                                            kMaximumLineLength,
                                            TEXT("Client exited:\t\t%d\r\n"),
                                            client_info->pid());

                                        LogString(line);
}

void DumpPage::CrashServerStart()
{
    wxString msg;
    CrashGenerationServer* crash_server = NULL;
    std::wstring dump_path = L"C:\\Dumps\\";
    dump_path = L"O:\\ra\\dump\\";
    int ret = SHCreateDirectoryEx(NULL, dump_path.c_str(), NULL);
    if ((ret!=ERROR_SUCCESS) && (ret!=ERROR_ALREADY_EXISTS))
    {
        msg.Printf(_T("Error %d create folder %s."), ret, dump_path.c_str());
        LogString(msg);
        dump_path=L"C:\\Dumps\\";
        ret = SHCreateDirectoryEx(NULL, dump_path.c_str(), NULL);
        if ((ret!=ERROR_SUCCESS) && (ret!=ERROR_ALREADY_EXISTS))
        {
            msg.Printf(_T("Error %d create folder %s."), ret, dump_path.c_str());
            LogString(msg);
            dump_path=L".";
        }
    }
    crash_server = new CrashGenerationServer(kPipeName,
        NULL,
        ShowClientConnected,
        NULL,
        ShowClientCrashed,
        NULL,
        ShowClientExited,
        NULL,
        true,
        &dump_path);
    if (!crash_server->Start()) {
        Log(_T(" unable to start dump server"));
        delete crash_server;
        crash_server = NULL;
    }
    else
    {
        msg.Printf(_T("Crash server started. Crash dump file will be written in %s. Pipe %s"), dump_path.c_str(), kPipeName);
        LogString(msg);
    }
}

void DumpPage::OnLogEvent( wxCommandEvent &evt )
{
    Log(evt.GetString());
}

// The function registers file sink in the logging library
void init_logging()
{namespace logging = boost::log;
//namespace fmt = boost::log::formatters;
namespace flt = boost::log::filters;
namespace sinks = boost::log::sinks;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace keywords = boost::log::keywords;
	boost::shared_ptr< logging::core > core = logging::core::get();

	boost::shared_ptr< sinks::text_file_backend > backend =
		boost::make_shared< sinks::text_file_backend >(
		// file name pattern
		keywords::file_name = "SetEvent_%Y%m%d.log",
		// rotate the file upon reaching 5 MiB size...
		keywords::rotation_size = 5 * 1024 * 1024,
		// ...or at noon, whichever comes first
		keywords::time_based_rotation = sinks::file::rotation_at_time_point(12, 0, 0)
		);

	// Wrap it into the frontend and register in the core.
	// The backend requires synchronization in the frontend.
	typedef sinks::synchronous_sink< sinks::text_file_backend > sink_t;
	boost::shared_ptr< sink_t > sink(new sink_t(backend));

	core->add_sink(sink);
}