#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/gbsizer.h>
#include <wx/textctrl.h>
#include <wx/Panel.h>
#include <wx/socket.h>
#include <sstream>
#include <iomanip>
#include "ProcessList.h"
#include "AboutPage.h"

AboutPage::AboutPage(wxWindow* parent) : wxPanel(parent, wxID_ANY)
{
    m_ConsoleWnd = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
        wxDefaultSize, wxTE_MULTILINE|wxVSCROLL|wxSIMPLE_BORDER );
    std::basic_stringstream<TCHAR> ts;
    ts  << _T("SetEvent.ext is a toy tool. Use it at your own risk.") << std::endl
        << _T("Build environment: \n")
        << _T("VC++ ") << _MSC_VER << _T("\n")
        << _T("MFC, wxWidgets 2.9.1, boost 1.44\n")
        << _T("Smart Cleanup from http://msdn.microsoft.com/en-us/magazine/cc164130.aspx\n")
        << _T("Process, modules and threads iterator from http://pages.videotron.com/gseguin/toolhelp_iterator.htm\n")
        << _T("Walkobjects from http://forum.sysinternals.com/forum_posts.asp?TID=3577&PN=2\n")
		<< _T("Loki from http://loki-lib.sourceforge.net/\n")
        << std::endl
        ;
	wxIPV4address addr;
	addr.Hostname(wxGetFullHostName());
	wxString ipAddr = addr.IPAddress();
	ts << _T("Host name: ") << wxGetFullHostName() << _T(" IP: ") << ipAddr << std::endl;

    ProcessInfo pis;
    int ecret = ProcessList::GetProcessInfo(GetCurrentProcessId(), pis);	
	ts << _T("Process ID: ") << wxGetProcessId() << std::endl;
    ts << _T("ImagePathName: ") << pis.module_path<< std::endl;
    ts << _T("Command line: ") << pis.command_line<< std::endl;
    ts << _T("Working directory: ") << pis.working_directory<< std::endl;
    ts <<  std::endl;
    ts << _T("Environment:") << std::endl << pis.environment << std::endl;
    ts << std::endl;

    m_ConsoleWnd->SetValue(ts.str().c_str());
    
    wxBoxSizer* szier = new wxBoxSizer(wxVERTICAL);
    szier->Add(
        m_ConsoleWnd,
        1,            // make vertically stretchable
        wxEXPAND |    // make horizontally stretchable
        wxALL,        //   and make border all around
        10 );         // set border width to 10
    SetSizerAndFit(szier);
}

AboutPage::~AboutPage(void)
{
}
