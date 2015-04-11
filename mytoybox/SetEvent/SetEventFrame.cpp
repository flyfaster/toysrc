//#include <afxtempl.h>
#include "SystemInfo.h"
#include "toolhelp_iterator.h"
#include <loki/Pimpl.h>
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/gbsizer.h>
#include <wx/textctrl.h>
#include <wx/Panel.h>
#include <wx/notebook.h>
#include <wx/confbase.h>
#include <map>
#include <psapi.h>
#include "OptionsListener.h"
#include "EventPage.h"
#include "DumpPage.h"
#include "OptionsPage.h"
#include "ErrorPage.h"
#include "ComputerPage.h"
#include "SetEventFrame.h"
#include "RegistryPage.h"
#include "AboutPage.h"
#include "SetEventApp.h"
#include <list>
#include <string>
#include <sstream>
#include <atlsecurity.h>
#include "ProcessList.h"
#include "32.xpm"
#include "64.xpm"
BEGIN_EVENT_TABLE(SetEventFrame, wxFrame)
EVT_CLOSE(SetEventFrame::OnCloseWindow)
END_EVENT_TABLE()

SetEventFrame::SetEventFrame( void ): wxFrame(NULL, wxID_ANY, wxString(_T("SetEventApp")), wxDefaultPosition,
											  wxSize(800,600))
{
    m_book = new wxNotebook(this, wxID_ANY);
    m_event_page = new EventPage(m_book);
    m_book->AddPage(m_event_page, wxT("Event"));

    DumpPage* m_dump_page;
    m_dump_page = new DumpPage(m_book);
    m_book->AddPage( m_dump_page, wxT("Dump") );

    m_error_page = new ErrorPage(m_book);
    m_book->AddPage(m_error_page, wxT("Lookup Error Code"));

    m_opt_page = new OptionsPage(m_book);
    m_book->AddPage(m_opt_page, wxT("Options"));

    ComputerPage* m_computer_page;
    m_computer_page = new ComputerPage(m_book);
    m_book->AddPage(m_computer_page, wxT("Computer"));

	RegistryPage* reg_page;
	reg_page = new RegistryPage(m_book);
	m_book->AddPage(reg_page, wxT("Registry"));
    AboutPage* m_about;
    m_about = new AboutPage(m_book);
    m_book->AddPage(m_about, wxT("About"));


    wxGetApp().AddOptionsListener(m_dump_page);
    wxGetApp().AddOptionsListener(m_event_page);

	wxIcon main_icon(main32_xpm);
	this->SetIcon(main_icon);
}
SetEventFrame::~SetEventFrame(void)
{

}

void SetEventFrame::OnCloseWindow( wxCloseEvent& event )
{
    m_opt_page->OnSaveConfig();
    wxConfigBase::Get()->Flush();
	Destroy();
    wxGetApp().ExitMainLoop();
}


