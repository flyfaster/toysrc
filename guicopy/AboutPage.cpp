#include "guicopy_include.h"
#include "mydlg.h"
#include "AboutPage.h"

template<>
struct Loki::ImplOf<AboutPage>
{
    wxTextCtrl* m_ConsoleWnd;
    void CreateUI(wxWindow* control_container);
};

void Loki::ImplOf<AboutPage>::CreateUI( wxWindow* control_container )
{
    m_ConsoleWnd = new wxTextCtrl(control_container, wxID_ANY, wxEmptyString, wxDefaultPosition,
        wxDefaultSize, wxTE_MULTILINE|wxVSCROLL|wxSIMPLE_BORDER );
    std::basic_stringstream<TCHAR> ts;
    ts  << __argv[0] << _T(" is a toy tool. Use it at your own risk.") << std::endl
        << _T("Build environment: \n")
        << _T("VC++ 2008 \n")
        << _T("wxWidgets 2.9.1, boost 1.44, cryptopp561, loki-0.1.7, python3.1\n")
        << std::endl
        ;

    m_ConsoleWnd->SetValue(ts.str().c_str());

    wxBoxSizer* szier = new wxBoxSizer(wxVERTICAL);
    szier->Add(
        m_ConsoleWnd,
        1,            // make vertically stretchable
        wxEXPAND |    // make horizontally stretchable
        wxALL,        //   and make border all around
        10 );         // set border width to 10
    control_container->SetSizerAndFit(szier);
}

AboutPage::AboutPage(wxWindow* parent) : wxPanel(parent, wxID_ANY)
{
    d->CreateUI(this);
}

AboutPage::~AboutPage(void)
{
}
