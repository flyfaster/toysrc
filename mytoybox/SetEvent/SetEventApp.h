#pragma once
#include <string>
#include <boost/circular_buffer.hpp>
#include <list>
#include "OptionsListener.h"
typedef std::basic_string<TCHAR> tchar_string;


class SetEventApp : public wxApp
{
public:
	SetEventApp(void);
	~SetEventApp(void);
	bool OnInit();

    void LoadDumpPathHistory();

    void ExitMainLoop();
    void OnClose( wxCloseEvent& event );
    void FlushConfig();
    bool AddDumpHistory(tchar_string& dump_path);
    wxFrame* m_main_wnd;
    boost::circular_buffer<tchar_string> m_dump_path_history;
    std::multimap<wxString, DWORD> m_processes;
    static void GetProcessList(std::multimap<wxString, DWORD>& processes);
    static std::basic_string<TCHAR> SystemErrorToString(DWORD error_code);
    std::list<IOptionsListener*> m_options_listener;
    void AddOptionsListener(IOptionsListener* listener);
    void FireOptionsChanged();
    DECLARE_EVENT_TABLE()

};

extern LPCTSTR DUMP_PATH_CONFIG_VALUE;

DECLARE_APP(SetEventApp);