#pragma once

class VerbosePage : public wxPanel//, public IPage
{
public:
	VerbosePage(wxWindow* parent);
	~VerbosePage();
	wxString GetName();
	wxXmlNode* GetNode();
	void Init(wxXmlNode* p);
	void OnKeyUp(wxKeyEvent& event);
	void OnButton(wxCommandEvent& event);
private:

	wxTextCtrl* m_status;
	wxLogTextCtrl* logger;
	wxLog *m_logOld;
	DECLARE_EVENT_TABLE()
};
