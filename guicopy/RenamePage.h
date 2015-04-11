#pragma once
#include <Loki/Pimpl.h>

class RenamePage :
	public wxPanel, private Loki::PimplOf<RenamePage>::Owner
{
public:
	RenamePage(wxWindow* parent);
	~RenamePage(void);
	void OnButton(wxCommandEvent& ev);
	DECLARE_EVENT_TABLE()

};
