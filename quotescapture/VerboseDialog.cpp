#include "stdafx.h"
#include <wx/wx.h>
#include <wx/button.h>
#include <wx/textctrl.h>
#include <wx/statbmp.h>
#include <wx/sizer.h>
#include <wx/gbsizer.h>
#include <wx/textctrl.h>
#include <wx/log.h>
#include <wx/menu.h>
#include <wx/menuitem.h>

#include "VerboseDialog.h"
#include "MainApp.h"

#ifndef USE_LOKI_PIMPL
struct VerboseDialogPimpl : public wxEvtHandler
#else
template<> struct Loki::ImplOf<VerboseDialog> : public wxEvtHandler
#endif
{
	void OnClearLog(wxCommandEvent& wxevt);
	void OnSaveLog(wxCommandEvent& wxevt);
	void CreateUI(wxWindow* parent);
	void OnComboboxSelected (wxCommandEvent &event);
	void OnCloseWindow( wxCloseEvent& ev );
	void OnShowWindowSelected(wxCommandEvent& ev);
	void OnMaxLogLength(wxCommandEvent& ev);
	void Init();
	wxTextCtrl* m_log_output_wnd;
	wxLogTextCtrl* m_logger;
	wxLog* m_logOld;
	wxButton* m_clear_btn;
	wxButton* m_save_btn;
	wxComboBox* m_log_level;
	static wxWindow* m_single_instance;
	wxMenuItem* m_ctrl_menu;
};

#ifdef USE_LOKI_PIMPL
typedef Loki::ImplOf<VerboseDialog> VerboseDialogPimpl;
#endif

void VerboseDialogPimpl::OnMaxLogLength( wxCommandEvent& ev )
{
	wxString content = m_log_output_wnd->GetValue();
	if (content.length()<1024*1024*3)
		return;
	size_t pos = content.find(wxT('\n'), 1024*1024);
	if (pos>0)
	{
		m_log_output_wnd->Remove(0, (long)pos+1);
	}
}

void VerboseDialogPimpl::Init()
{
	m_log_output_wnd = NULL;
	m_logger = NULL;
	m_logOld = NULL;
	m_clear_btn = NULL;
	m_save_btn=NULL;
	m_log_level = NULL;
	m_ctrl_menu=NULL;
}
void VerboseDialogPimpl::OnShowWindowSelected(wxCommandEvent& wxev)
{
		m_single_instance->Show(m_ctrl_menu->IsChecked());
}
void VerboseDialogPimpl::OnCloseWindow(wxCloseEvent &ev)
{
	if (m_ctrl_menu)
	{
		m_single_instance->Show(false);
		m_ctrl_menu->Check(false);
	}
	MainApp* papp = (MainApp*)wxApp::GetInstance();
	if (m_single_instance == papp->GetTopWindow())
	{
		m_single_instance->Destroy();
		m_single_instance = NULL;
	}
}
void VerboseDialogPimpl::OnSaveLog( wxCommandEvent& wxevt )
{
	wxLogVerbose( wxString::FromAscii(__FUNCTION__));
}

void VerboseDialogPimpl::OnComboboxSelected( wxCommandEvent &event )
{
	wxLog::SetLogLevel(m_log_level->GetSelection());
	wxLogVerbose(wxString::FromAscii(__FUNCTION__));
}

void VerboseDialogPimpl::OnClearLog( wxCommandEvent& wxevt )
{
	m_log_output_wnd->Clear();
	wxLogMessage( wxString::FromAscii(__FUNCTION__));
}
wxWindow* VerboseDialogPimpl::m_single_instance;
void VerboseDialogPimpl::CreateUI( wxWindow* parent )
{
	m_log_level = new wxComboBox(parent, wxID_ANY);
	m_log_level->AppendString(wxT("Fatal Error"));
	m_log_level->AppendString(wxT("Error"));
	m_log_level->AppendString(wxT("Warning"));
	m_log_level->AppendString(wxT("Message"));
	m_log_level->AppendString(wxT("Status"));
	m_log_level->AppendString(wxT("Info"));
	m_log_level->AppendString(wxT("Debug"));
	m_log_level->AppendString(wxT("Trace"));
	m_log_level->AppendString(wxT("Progress"));
	m_log_level->SetSelection(wxLOG_Info);
	m_log_level->Connect(wxEVT_COMMAND_COMBOBOX_SELECTED, 
		wxCommandEventHandler(VerboseDialogPimpl::OnComboboxSelected), NULL, this);

	m_log_output_wnd = new wxTextCtrl(parent, wxID_ANY, wxEmptyString, wxDefaultPosition,
		wxSize(600, 400), wxTE_MULTILINE|wxVSCROLL|wxSIMPLE_BORDER);
	m_log_output_wnd->SetMaxLength(1024*1024*4);
	m_log_output_wnd->Connect(wxEVT_COMMAND_TEXT_MAXLEN, 
		wxCommandEventHandler(VerboseDialogPimpl::OnMaxLogLength), NULL, this);
	m_log_output_wnd->Connect(wxEVT_COMMAND_TEXT_UPDATED,
		wxCommandEventHandler(VerboseDialogPimpl::OnMaxLogLength), NULL, this);
	//wxEVT_COMMAND_TEXT_MAXLEN

	wxGridBagSizer* sizer=new wxGridBagSizer();
	sizer->Add(m_log_output_wnd, wxGBPosition(0,0), wxGBSpan(1, 6), wxEXPAND|wxALL);
	sizer->AddGrowableCol(5);
	sizer->AddGrowableRow(0);
	m_clear_btn = new wxButton(parent, wxID_ANY, wxT("Clear"));
	m_save_btn = new wxButton(parent, wxID_ANY, wxT("Save"));
	m_clear_btn->Connect(wxEVT_COMMAND_BUTTON_CLICKED, 
		wxCommandEventHandler( VerboseDialogPimpl::OnClearLog), NULL, this);
	m_save_btn->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler( VerboseDialogPimpl::OnSaveLog), NULL, this);
	int row = 1, column = 0;
	
	wxStaticText* log_level_lable = new wxStaticText(parent, wxID_ANY, wxT("Log Level:"));
	sizer->Add(log_level_lable, wxGBPosition(row, column++));

	sizer->Add(m_log_level, wxGBPosition(row, column));
	column++;
	sizer->Add(m_save_btn, wxGBPosition(row, column), wxDefaultSpan, wxCENTER);
	column++;
	sizer->Add(m_clear_btn, wxGBPosition(row,column), wxDefaultSpan, wxCENTER);
	parent->SetSizer(sizer);
	sizer->SetSizeHints(parent);
	sizer->Fit(parent);
	m_logger = new wxLogTextCtrl(m_log_output_wnd);
	m_logOld = wxLog::SetActiveTarget(m_logger);
	wxLog::SetTimestamp(wxT("%H:%M:%S"));

}

VerboseDialog::VerboseDialog(wxWindow* parent)
: wxDialog( parent, wxID_ANY, wxT("Verbose Dialog"), wxDefaultPosition, wxDefaultSize,
		   wxCAPTION | wxSYSTEM_MENU | wxDIALOG_NO_PARENT | wxCLOSE_BOX | wxRESIZE_BORDER)
{
#ifndef USE_LOKI_PIMPL
	d = new VerboseDialogPimpl;
#endif
	d->Init();
	wxLog::SetVerbose(true);
	wxLog::SetLogLevel(wxLOG_Info);
	d->CreateUI(this);
	d->m_single_instance = this;
	wxString title;
	title.Printf(wxT("%s - [%d] - Verbose"), wxString::FromAscii(__argv[0]),
		GetCurrentProcessId());
	SetTitle(title);
	Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(VerboseDialogPimpl::OnCloseWindow),NULL, d);
}

VerboseDialog::~VerboseDialog(void)
{
	VerboseDialogPimpl::m_single_instance = NULL;
	wxLog::SetActiveTarget(d->m_logOld);
	wxLog::SetLogLevel(wxLOG_Error);
}

wxWindow* VerboseDialog::Instance()
{
	if (!MainApp::Stopping() && !VerboseDialogPimpl::m_single_instance)
	{
		VerboseDialog* tmp = new VerboseDialog(NULL);
	}
	return VerboseDialogPimpl::m_single_instance;
}

void VerboseDialog::AppendMenu( wxMenu* main_menu, wxWindow* host_window, int menu_id )
{
	d->m_ctrl_menu = main_menu->AppendCheckItem(menu_id, _T("&Show Log Dialog..."), _T("Display log dialog"));

	host_window->Connect(menu_id, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(VerboseDialogPimpl::OnShowWindowSelected),NULL, d);
	d->m_ctrl_menu->Check (this->IsVisible());

}
