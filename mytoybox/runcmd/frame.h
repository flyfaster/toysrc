#pragma once
#include <wx/wx.h>
#include <wx/gbsizer.h>

class MyFrame: public wxFrame {
public:
	MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
	void OnQuit(wxCommandEvent& event);
	void OnRun(wxCommandEvent&);
	void OnCopy(wxCommandEvent&);
	void OnClear(wxCommandEvent&);
	void OnSelectWorkingDir(wxCommandEvent& event);
	void OnSelectOutputPath(wxCommandEvent& event);
	void OnSelectCommand(wxCommandEvent& event);
private:
	wxTextCtrl *m_working_dir;
	wxTextCtrl *m_envs;
	wxTextCtrl *m_cmd;
	wxTextCtrl *m_output_path;
	wxTextCtrl *m_cmd_output;
	wxButton *m_choose_working_dir;
	wxButton *m_update_env;
	wxButton *m_choose_cmds;
	wxButton *m_choose_output_path;

	wxButton *m_run;
	wxButton *m_copy;
	wxButton *m_reset;
    enum
    {
        ID_Hello = 1
    };

};
