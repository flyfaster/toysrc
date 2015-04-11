#include "StdAfx.h"
#include <wx/wxprec.h>
#include <wx/gbsizer.h>
#include <wx/frame.h>
#include <wx/notebook.h>
#include <wx/menu.h>
#include <wx/menuitem.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <boost/bind.hpp>
#include <boost/scoped_ptr.hpp>
#include "http_client_factory.h"
#include "MainFrame.h"
#include "StockDB.h"
#include "ImportDailyFrame.h"
#include "MainApp.h"
#include "VerboseDialog.h"
#ifndef USE_LOKI_PIMPL
struct PrivateMainFrame:public wxEvtHandler
#else
template<>
struct Loki::ImplOf<MainFrame>:public wxEvtHandler
#endif
{
	wxFrame* m_frame;
	wxMenuBar* m_menubar;
	wxMenu* m_file_menu;
	wxMenu* m_debug_menu;
	wxTextCtrl* m_symbols;
	wxButton* m_add_symbol;
	void OnCloseWindow(wxCloseEvent& ev);
	void OnOpenCSV(wxCommandEvent& ev);
	void OnDump(wxCommandEvent& ev);
	void OnAddSymbols(wxCommandEvent& wxevt);
	void CreateMenus();
};

#ifdef USE_LOKI_PIMPL
typedef struct Loki::ImplOf<MainFrame> PrivateMainFrame;
#endif

void PrivateMainFrame::OnAddSymbols( wxCommandEvent& wxevt )
{
	wxString sym = m_symbols->GetValue();
	std::string ssym = sym.mbc_str();
	wxGetApp().GetHttpClientFactory()->prepare_nasdaq_request(ssym);
}

void PrivateMainFrame::OnCloseWindow( wxCloseEvent& ev )
{
	wxMessageDialog *dial = new wxMessageDialog(NULL,
		wxT("Are you sure to quit?"), wxT("Question"),
		wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);

	int ret = dial->ShowModal();
	dial->Destroy();

	if (ret == wxID_YES) {
		ev.Skip();
		wxApp::GetInstance()->OnExit();
		//m_frame->Destroy();
		//m_frame = NULL;
		//wxGetApp().OnExit();
	} else {
		ev.Veto();
	}
}
void PrivateMainFrame::OnDump(wxCommandEvent& ev)
{
	wxLogVerbose( wxString::FromAscii(__FUNCTION__));
	MainApp* papp = (MainApp*)wxApp::GetInstance();
	papp->Dump();

}
void PrivateMainFrame::OnOpenCSV(wxCommandEvent& ev)
{
	//wxFileDialog fileDialog( NULL, _("Open database"), wxEmptyString,
	//	wxEmptyString,
	//	_("History files (*.csv)|*.csv|All files (*.*)|*.*"),
	//	wxFD_OPEN|wxFD_FILE_MUST_EXIST);
	//if (fileDialog.ShowModal() == wxID_OK)
	//{
	//	SingleStockDB::Instance().ImportDailyQuotes(_T("ADSK"), fileDialog.GetPath());
	//}
	boost::scoped_ptr<ImportDailyFrame> import_daily (new ImportDailyFrame());
	import_daily->DoModal();
	if (import_daily->GetDailyFile().length() && import_daily->GetSymbol().length())
	{
		SingleStockDB::Instance().ImportDailyQuotes(import_daily->GetSymbol().c_str(), 
													import_daily->GetDailyFile().c_str());
	}
}

void PrivateMainFrame::CreateMenus()
{
	m_menubar = new wxMenuBar;
	m_file_menu = new wxMenu;
	m_file_menu->Append(wxID_OPEN, _T("&Open..."), _T("Load daily quotes csv"), 
		wxITEM_NORMAL);
	m_menubar->Append(m_file_menu, _T("&File"));
	m_frame->SetMenuBar(m_menubar);
	m_frame->Connect(wxID_OPEN, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(PrivateMainFrame::OnOpenCSV));
	VerboseDialog* pv = (VerboseDialog*)VerboseDialog::Instance();
	pv->AppendMenu(m_file_menu, m_frame, 10001);

	m_debug_menu = new wxMenu;
	m_menubar->Append(m_debug_menu, _T("&Debug"));
	const int ID_DEBUG_DUMP = __LINE__;
	m_debug_menu->Append(ID_DEBUG_DUMP, _T("Dump"), _T("Dump internal status"), wxITEM_NORMAL);
	m_frame->Connect(ID_DEBUG_DUMP, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(PrivateMainFrame::OnDump));
}

wxFrame* MainFrame::GetFrame(void)
{
	return d->m_frame;
}

MainFrame::MainFrame( void )
{
#ifndef USE_LOKI_PIMPL
	d = new PrivateMainFrame;
#endif
	d->m_frame = new wxFrame(NULL, wxID_ANY, wxString(_T("Stockclient")), wxDefaultPosition,
		wxSize(800, 600));
	d->m_frame->Connect(wxID_ANY, wxEVT_CLOSE_WINDOW, 
		wxCloseEventHandler( PrivateMainFrame::OnCloseWindow));
	d->CreateMenus();
	d->m_symbols = new wxTextCtrl(d->m_frame, wxID_ANY, wxString(_T("VRGY")), wxDefaultPosition, wxDefaultSize);
	d->m_add_symbol = new wxButton(d->m_frame, wxID_ANY, _T("Add symbols(,)"));
	d->m_add_symbol->Connect(wxEVT_COMMAND_BUTTON_CLICKED, 
		wxCommandEventHandler( PrivateMainFrame::OnAddSymbols), NULL, d);

	wxGridBagSizer* gbsizer = new wxGridBagSizer();
	int row = 0;
	int col = 0;
	gbsizer->Add(d->m_add_symbol, wxGBPosition(row, col));
	col++;
	gbsizer->Add(d->m_symbols, wxGBPosition(row, col), wxDefaultSpan, wxEXPAND | wxRIGHT);
	gbsizer->AddGrowableCol(1);
	d->m_frame->SetSizer(gbsizer);
	gbsizer->SetSizeHints(d->m_frame);
}

MainFrame::~MainFrame( void )
{
#ifndef USE_LOKI_PIMPL
	delete d;
#endif
}

typedef Loki::SingletonHolder<MainFrame, Loki::CreateUsingNew, Loki::PhoenixSingleton> SingleMainFrame;

MainFrame& MainFrame::Instance()
{
	return SingleMainFrame::Instance();
}


