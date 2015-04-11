#pragma once
#include <string>
typedef std::basic_string<TCHAR> tchar_string;
class DumpPage : public wxPanel, public IOptionsListener
{
public:
    DumpPage(wxWindow* parent);
    ~DumpPage(void);
    void OnButton(wxCommandEvent& event);
    void OnRefreshProcess();
    //void UpdateProcessList();
    wxComboBox* m_ProcessList;
    wxComboBox* m_DumpPathList;
    wxButton* m_RefreshProcess;
    wxButton* m_DumpButton;
    wxButton* m_ClearButton;
    wxButton* m_ChooseDumpPath;
    wxTextCtrl* m_LogWnd;
    HANDLE dump_file;
    DWORD OpenDumpFile();
    tchar_string process_name;
    tchar_string dump_file_path;
    static DumpPage* dump_page;
    DWORD process_id;
    DWORD CreateProcessDump();
    void Log(const wxString& msg);
    virtual void OnOptionsChanged();
    void CrashServerStart();
    void OnLogEvent( wxCommandEvent &evt );
    DECLARE_EVENT_TABLE()

};
