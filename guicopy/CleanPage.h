#pragma once

class CleanPage :
	public wxPanel, private Loki::PimplOf<CleanPage>::Owner
{
public:
	CleanPage(wxWindow* parent);
	~CleanPage(void);
	wxXmlNode* GetNode();
	void Init( wxXmlNode * child );
	void OnDelete();
	void OnButton(wxCommandEvent& ev);
private:
	DECLARE_EVENT_TABLE()
};
