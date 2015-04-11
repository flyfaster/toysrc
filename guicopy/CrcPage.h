#pragma once

class CrcPage :
	public wxPanel
{
public:
	CrcPage(wxWindow* parent);
	~CrcPage(void);
	wxString GetName();
	void OnOK();
	
private:
	wxTextCtrl* m_file_name;
	wxTextCtrl* m_checksum;
	wxComboBox* m_checksum_type;
	wxButton* m_choose_file;
	wxButton* m_copy_checksum;
	DECLARE_EVENT_TABLE()
	void OnButton(wxCommandEvent& eventobj);
};
