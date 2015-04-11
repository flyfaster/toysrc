#pragma once

class EventPage:public wxPanel, public IOptionsListener
{
public:
    EventPage(wxWindow* parent);
    ~EventPage(void);
    void OnButton(wxCommandEvent& event);
    void OnComboBoxSelected( wxCommandEvent& event );

    void RefreshProcessList2();

    void ProcessInformation( int processID );
    void PrintThreadTimes(DWORD process_id);

    void ThreadTimesToStream( DWORD process_id, std::basic_stringstream<TCHAR>& ts );
    virtual void OnOptionsChanged();
private:
    wxTextCtrl* m_EventNameCtrl;
    wxComboBox* m_ProcessList;
    wxComboBox* m_EventList;
    wxButton* m_RefreshProcess;
    wxButton* m_SetButton;
    wxButton* m_ExitButton;
    wxButton* m_ClearButton;
    wxButton* m_ProcessButton;
    wxLogTextCtrl* logger;
    wxTextCtrl* m_LogWnd;
    wxLog *m_logOld;
public:
    DECLARE_EVENT_TABLE()

};
