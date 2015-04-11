#pragma once

class ComputerPage : public wxPanel, private Loki::PimplOf<ComputerPage>::Owner
{
public:
    ComputerPage(wxWindow* parent);

	wxSizer* CreateButtons(wxWindow* parent);

	~ComputerPage(void);
    void OnButton(wxCommandEvent& event);
	void OnRestartMachine(wxCommandEvent& event);
	void OnShutdownMachine(wxCommandEvent& event);
    void OnClearLog(wxCommandEvent& event);
    void OnProcessButtonClicked(wxCommandEvent& evt);
    void OnNamespaceClicked(wxCommandEvent& evt);
    void OnClassButtonClicked(wxCommandEvent& evt);
    void SaveConfig();
    void OnSystemInfo(wxCommandEvent& evt);
    void OnWmiQuery(wxCommandEvent& evt);
    void DisplayDuration(LPCSTR name, wxDateTime& start_time);
    void PrintEnumWbemClassObject(struct IEnumWbemClassObject* pEnumObj);
    wxString GetWmiNamespace();
    HRESULT ExecQuery(LPCTSTR wmi_query);
    wxButton *m_machine_btn;
    wxTextCtrl* m_machine_input;
    wxStaticText* m_user_label;
    wxStaticText* m_pwd_label;
    wxTextCtrl* m_user_input;
    wxTextCtrl* m_pwd_input;
	wxButton* m_shutdown_btn;
    wxButton* m_restart_btn;
    wxButton* m_process_btn;
    wxButton* m_system_btn;
    wxButton* m_clear_btn;
    wxTextCtrl* m_LogWnd;
    wxTextCtrl* m_query_input;
    wxButton* m_query_btn;
    wxComboBox* m_namespace_list;
    wxButton* m_namespace_btn;
    wxButton* m_class_btn;
    DECLARE_EVENT_TABLE()

};
