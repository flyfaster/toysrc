#pragma once
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/dialog.h>
#include <loki/Pimpl.h>
//#include <loki/Singleton.h>
class wxMenu;

class STOCKCLIENT_API VerboseDialog : public wxDialog
#ifdef USE_LOKI_PIMPL
	, private Loki::PimplOf<VerboseDialog>::Owner
#endif
{
public:
	VerboseDialog(wxWindow* parent=NULL);
	~VerboseDialog(void);
	static wxWindow* Instance();
	void AppendMenu(wxMenu* main_menu, wxWindow* host_window, int menu_id);
#ifndef USE_LOKI_PIMPL
	struct VerboseDialogPimpl* d;
#endif
};
