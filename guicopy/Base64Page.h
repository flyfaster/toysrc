#pragma once

class Base64Page :
	public wxPanel, private Loki::PimplOf<Base64Page>::Owner
{
public:
	Base64Page(wxWindow* parent);
	~Base64Page(void);
	void OnButton( wxCommandEvent& ev );
	DECLARE_EVENT_TABLE()

};
