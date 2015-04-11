#include "SystemInfo.h"
#include "toolhelp_iterator.h"
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/gbsizer.h>
#include <wx/textctrl.h>
#include <wx/Panel.h>
#include <map>
#include <psapi.h>
#include "OptionsListener.h"
#include "SetEventFrame.h"
#include "SetEventApp.h"
#include <list>
#include <string>
#include <sstream>
#include <atlsecurity.h>
#include "ProcessList.h"
#include "EventPage.h"

int get_event_list(DWORD process_id, std::list<std::wstring>& event_list);
int get_handle_list(DWORD process_id, std::list<std::wstring>& event_list);


BEGIN_EVENT_TABLE(EventPage, wxPanel)
EVT_BUTTON(wxID_ANY, EventPage::OnButton)
EVT_COMBOBOX(wxID_ANY, EventPage::OnComboBoxSelected)
END_EVENT_TABLE()


EventPage::EventPage(wxWindow* parent):wxPanel(parent, wxID_ANY)
{
    m_EventNameCtrl = new wxTextCtrl(this, wxID_ANY);
    m_ExitButton = new wxButton(this, wxID_ANY, _T("Exit"));
    m_SetButton = new wxButton(this, wxID_ANY, _T("SetEvent"));
    m_ClearButton = new wxButton(this, wxID_ANY, _T("Clear"));
    m_ProcessButton = new wxButton(this, wxID_ANY, _T("Process information"));
    m_RefreshProcess = new wxButton(this, wxID_ANY, _T("Process"));
    wxStaticText *event_label = new wxStaticText(this, wxID_ANY, _T("Event Name"));
    m_LogWnd = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
        wxDefaultSize, wxTE_MULTILINE|wxVSCROLL|wxSIMPLE_BORDER );
    m_ProcessList = new wxComboBox(this, wxID_ANY, wxT(""), wxDefaultPosition,
        wxDefaultSize, 0, NULL, wxCB_DROPDOWN);
    m_EventList = new wxComboBox(this, wxID_ANY);
    wxStaticText* eventlist_label = new wxStaticText(this, wxID_ANY, _T("Event"));

    wxGridBagSizer* sizer = new wxGridBagSizer();
    int col = 0;
    int row = 0;
    sizer->Add(m_RefreshProcess, wxGBPosition(row, col));
    col++;
    sizer->Add(m_ProcessList, wxGBPosition(row, col), wxGBSpan(1, 3), wxEXPAND | wxRIGHT);
    row++;
    col=0;
    sizer->Add(eventlist_label, wxGBPosition(row, col));
    col++;
    sizer->Add(m_EventList, wxGBPosition(row, col), wxGBSpan(1, 3), wxEXPAND | wxRIGHT);
    col=0;
    row++;
    sizer->Add(event_label, wxGBPosition(row, col));
    col++;
    sizer->Add(m_EventNameCtrl, wxGBPosition(row, col), wxGBSpan(1, 3), wxEXPAND | wxRIGHT);
    col = 0;
    row++;
    sizer->Add(m_SetButton, wxGBPosition(row, col));
    col++;
    sizer->Add(m_ExitButton, wxGBPosition(row, col));
    col++;
    sizer->Add(m_ClearButton, wxGBPosition(row, col));
    col++;
    sizer->Add(m_ProcessButton, wxGBPosition(row, col));
    col=0;
    row++;
    sizer->Add(m_LogWnd, wxGBPosition(row, col), wxGBSpan(1, 4), wxEXPAND | wxALL);
    sizer->AddGrowableCol(3);
    sizer->AddGrowableRow(row);
    SetSizer(sizer);
    sizer->SetSizeHints(this);
    //sizer->Fit(this);

    logger = new wxLogTextCtrl( m_LogWnd );
    m_logOld = wxLog::SetActiveTarget( logger );
    //wxLog::SetTimestamp( NULL );
    RefreshProcessList2();
}

EventPage::~EventPage(void)
{
    delete wxLog::SetActiveTarget(m_logOld);

}

void EventPage::OnButton( wxCommandEvent& event )
{
    if (event.GetEventObject()==m_RefreshProcess)
    {
        wxGetApp().GetProcessList(wxGetApp().m_processes);
        RefreshProcessList2();
        return;
    }

    if (event.GetEventObject()==m_ClearButton)
    {
        m_LogWnd->SetValue(wxEmptyString);
    }

    if (event.GetEventObject() == m_ProcessButton)
    {
        // refresh event list
        int current_sel = m_ProcessList->GetCurrentSelection();

        wxString current_process = m_ProcessList->GetString(current_sel);
        int processID = _ttoi(current_process);
        if (processID==0)
        {
            wxLogMessage(_T("Failed to get process id from %s"), current_process);
            return;
        }
        ProcessInformation(processID);
    }

    if ( event.GetEventObject() == m_SetButton )
    {
        HANDLE hevt = CreateEvent(NULL, FALSE, FALSE, m_EventNameCtrl->GetValue());
        DWORD dw = GetLastError(); 
        if (hevt==NULL)
        {
            LPVOID lpMsgBuf;
            wxLogError( _T("Failed to create event:%s"), m_EventNameCtrl->GetValue());
            FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                dw,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPTSTR) &lpMsgBuf,
                0, NULL );

            // Display the error message and exit the process
            wxLogError((LPCTSTR)lpMsgBuf);
            LocalFree(lpMsgBuf);

        }
        else
        {
            if (dw == ERROR_ALREADY_EXISTS)
            {
                SetEvent(hevt);
                wxLogMessage(_T("Set event %s successfully"), m_EventNameCtrl->GetValue());

            }
            else
            {
                wxLogError(_T("Event %s does not exist"), m_EventNameCtrl->GetValue());
            }
        }
        CloseHandle(hevt);
    }

    if (event.GetEventObject()==m_ExitButton)
    {
        Destroy();
        wxGetApp().ExitMainLoop();
    }
}

void EventPage::RefreshProcessList2()
{
    m_ProcessList->Clear();
    m_EventList->Clear();
    std::multimap<wxString, DWORD>& processes = wxGetApp().m_processes;
    //wxGetApp().GetProcessList(processes);
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

void EventPage::OnComboBoxSelected( wxCommandEvent& event )
{
    if (event.GetEventObject()==m_ProcessList)
    {
        // refresh event list
        int current_sel = m_ProcessList->GetCurrentSelection();

        wxString current_process = m_ProcessList->GetString(current_sel);
        int processID = _ttoi(current_process);
        if (processID==0)
        {
            wxLogMessage(_T("Failed to get process id from %s"), current_process);
            return;
        }
        // fill event name
        size_t name_start = current_process.rfind(_T('\\'));
        if (name_start != wxString::npos)
        {
            wxString process_name = current_process.substr(name_start+1);
            wxString dump_event_name;
            dump_event_name.Printf(wxT("%s-%d-DumpEvent"), process_name, processID);
            m_EventNameCtrl->SetValue(dump_event_name);
        }

        return;
    }
    if (event.GetEventObject()==m_EventList)
    {
        int sel = m_EventList->GetCurrentSelection();
        wxString selected_event = m_EventList->GetString(sel);
        LPCTSTR os_name_prefix = _T("\\BaseNamedObjects\\");
        if (selected_event.find(os_name_prefix)==0)
        {// remove the prefix, because CreateEvent will fail if it exists.
            selected_event = selected_event.substr(_tcslen(os_name_prefix));
        }
        m_EventNameCtrl->SetValue(selected_event);
    }
}

void EventPage::ProcessInformation( int processID )
{
    DWORD handle_count = 0;
    std::basic_stringstream<TCHAR> ts;
    SYSTEMTIME st;
    GetLocalTime(&st);
    TCHAR buf[1024];
    _stprintf_s(buf, _T("Current time: %04d%02d%02d %02d:%02d:%02d.%03d"), st.wYear, st.wMonth,
        st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    ts << buf << std::endl;
    ProcessInfo pis;
    int ecret = ProcessList::GetProcessInfo(processID, pis);
    if (ecret)
    {
        _stprintf_s(buf, _T("Get process information error %d"), ecret);
        ts << buf << std::endl;
    }
    ts << _T("ImagePathName: ") << pis.module_path<< std::endl;
    ts << _T("Command line: ") << pis.command_line<< std::endl;
    ts << _T("Working directory: ") << pis.working_directory<< std::endl;
    ts <<  std::endl;
    ts << _T("Environment:") << std::endl << pis.environment << std::endl;
    ts << std::endl;
    // print process handle count
    HANDLE hprocess = OpenProcess(PROCESS_QUERY_INFORMATION| PROCESS_VM_READ, FALSE, processID);
    if (hprocess)
    {
        CAccessToken tok;
        tok.GetProcessToken(TOKEN_QUERY|TOKEN_READ, hprocess);
        CSid user;
        tok.GetUser(&user);			
        _stprintf_s(buf,_T("Process running under account:%s\n"), user.AccountName());
        ts << buf << std::endl;
        HMODULE modules[1024];
        DWORD size_needed = 1024;
        BOOL ret = EnumProcessModules(hprocess, modules, size_needed, &size_needed);
        if (!ret)
        {
            ret = GetLastError();
            _stprintf_s(buf,_T("ERROR %d when enum process modules of %d"), ret,
                processID);
            ts << buf << std::endl;
        }
        else
        {
            int module_count = size_needed/sizeof *modules;
            _stprintf_s(buf,_T("Modules count %d of process %d"), module_count,  processID);
            ts << buf << std::endl;
            for (int i=0; i<module_count; i++)
            {
                TCHAR module_name_buf[MAX_PATH+1024];
                GetModuleFileNameEx(hprocess, modules[i], module_name_buf,
                    sizeof(module_name_buf)/sizeof*module_name_buf);
                ts << module_name_buf << std::endl;
            }
            ts << _T("") << std::endl;
        }
        GetProcessHandleCount(hprocess, &handle_count);

        // memory usage information
        PROCESS_MEMORY_COUNTERS_EX pmce;
        memset(&pmce, 0, sizeof(pmce));
        pmce.cb = sizeof(pmce);
        GetProcessMemoryInfo(hprocess, (PPROCESS_MEMORY_COUNTERS)&pmce, sizeof(pmce));
        _stprintf_s(buf,_T("PageFaultCount:%d, PrivateUsage:%d"), pmce.PageFaultCount,
            pmce.PrivateUsage);
        ts << buf << std::endl;
        _stprintf_s(buf,_T("PeakWorkingSetSize:%d bytes, WorkingSetSize:%d bytes"),
            pmce.PeakWorkingSetSize, pmce.WorkingSetSize);
        ts << buf << std::endl;
        _stprintf_s(buf,_T("QuotaPeakPagePoolUsage:%d, QuotaPagedPoolUsage:%d")
            ,pmce.QuotaPeakPagedPoolUsage, pmce.QuotaPeakPagedPoolUsage);
        ts << buf << std::endl;

        _stprintf_s(buf,_T("QuotaPeakNonPagedPoolUsage:%d, QuotaNonPagedPoolUsage:%d")
            , pmce.QuotaPeakNonPagedPoolUsage,
            pmce.QuotaPeakNonPagedPoolUsage);
        ts << buf << std::endl;
        _stprintf_s(buf,_T("PeakPagefileUsage:%d, PageFileUsage:%d")			
            , pmce.PeakPagefileUsage, pmce.PagefileUsage);
        ts << buf << std::endl;
        ts << _T("") << std::endl;
        CloseHandle(hprocess);
    }
    else
    {
        DWORD dw = GetLastError();
        _stprintf_s(buf,_T("Failed to open process %d, error %d"), processID, dw);
        ts << buf << std::endl;
    }
    wxLogMessage(wxT("%s"), ts.str().c_str());
    ts.str(wxT(""));
    // print thread time information
    ThreadTimesToStream(processID, ts);
    ts << _T("") << std::endl;


    m_EventList->Clear();
    std::list<std::wstring> event_list;
    get_event_list(processID, event_list);
    for (std::list<std::wstring>::const_iterator it=event_list.begin();
        it!=event_list.end();
        it++)
    {
        wxString ev_name = (*it).c_str();
        m_EventList->AppendString(ev_name);
    }
    event_list.clear();
    // print handle list
    _stprintf_s(buf,wxT("Process handle count: %d"), handle_count);
    ts << buf << std::endl;

    get_handle_list(processID, event_list);
    for (std::list<std::wstring>::const_iterator it=event_list.begin();
        it!=event_list.end();
        it++)
    {
        ts <<  (*it) << std::endl;
    }
    wxLogMessage(wxT("%s"), ts.str().c_str());

}

void EventPage::PrintThreadTimes( DWORD process_id )
{
    std::basic_stringstream<TCHAR> ts;
    ThreadTimesToStream(process_id, ts);

    wxLogMessage(wxT("%s"), ts.str().c_str());

}

void EventPage::ThreadTimesToStream( DWORD process_id, std::basic_stringstream<TCHAR>& ts )

{
    CThreadIterator k(process_id, new CSingleProcessThreadIteratorFilter(process_id));
    FILETIME ct, et, kt, ut;
    SYSTEMTIME sct, set, skt, sut;
    TCHAR buf[1024];
    while(k != CThreadIterator::End)
    {
        //wxLogMessage()
        HANDLE ht = OpenThread(READ_CONTROL|THREAD_QUERY_INFORMATION , FALSE, k->th32ThreadID);
        DWORD dw = GetLastError(); 
        if (ht==NULL)
        {
            LPVOID lpMsgBuf;
            _stprintf_s(buf, _T("Failed to create event:%s"), m_EventNameCtrl->GetValue());
            ts << buf << std::endl;
            FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                dw,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPTSTR) &lpMsgBuf,
                0, NULL );
            // Display the error message and exit the process
            ts << ((LPCTSTR)lpMsgBuf) << std::endl;
            LocalFree(lpMsgBuf);
            continue;
        }
        BOOL ret = GetThreadTimes(ht, &ct, &et, &kt, &ut);
        if (FALSE==ret)
        {
            _stprintf_s(buf, wxT("GetThreadTimes error: %d for thread %d"), GetLastError(), k->th32ThreadID);
            ts << buf << std::endl;
        }
        else
        {
            FileTimeToSystemTime(&ct, &sct);
            FileTimeToSystemTime(&et, &set);
            FileTimeToSystemTime(&kt, &skt);
            FileTimeToSystemTime(&ut, &sut);
            _stprintf_s(buf, wxT("Thread (%d) user time: %02d:%02d:%02d:%03d, kernel time: %02d:%02d:%02d:%03d, create time: %02d:%02d:%02d.%03d"),
                k->th32ThreadID, 
                sut.wHour, sut.wMinute, sut.wSecond, sut.wMilliseconds,
                skt.wHour, skt.wMinute, skt.wSecond, skt.wMilliseconds,
                sct.wHour, sct.wMinute, sct.wSecond, sct.wMilliseconds);
            ts << buf << std::endl;
        }
        CloseHandle(ht);
        ++k;
    }
}

void EventPage::OnOptionsChanged()
{
    RefreshProcessList2();
}
