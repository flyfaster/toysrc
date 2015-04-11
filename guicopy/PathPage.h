#pragma once

class PathPage :
	public wxPanel
{
public:
	PathPage(wxWindow* parent);
	~PathPage(void);

	void OnOK();
	void OnButton( wxCommandEvent& event );
	wxString GetName();

private:
	wxTextCtrl* m_path_current_path_input;
	wxTextCtrl* m_path_relative_path;
	wxTextCtrl* m_path_target_path;
	wxTextCtrl* m_cmd_path;
	wxButton* m_choose_cmd;
	wxButton* m_path_choose_current;
	wxButton* m_path_choose_target;
	DECLARE_EVENT_TABLE()
	static void path_to_components_que(LPCTSTR cur_dir_buf,tstringlist& qque);
	static tchar_string make_upper(tchar_string src);
	bool RunCmd();
};
