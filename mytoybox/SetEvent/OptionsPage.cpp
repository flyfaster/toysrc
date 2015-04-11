#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/gbsizer.h>
#include <wx/textctrl.h>
#include <wx/Panel.h>
#include <wx/confbase.h>
#include <sstream>
#include <map>
#include <boost/tokenizer.hpp>
#include "OptionsPage.h"
#include "SetEventApp.h"

LPCTSTR OPTIONS_IgnorePathList=wxT("IgnorePathList");
LPCTSTR OPTIONS_EnableIgnoreList=wxT("EnableIgnoreList");

BEGIN_EVENT_TABLE(OptionsPage, wxPanel)
EVT_BUTTON(wxID_ANY, OptionsPage::OnButton)
END_EVENT_TABLE()
OptionsPage::OptionsPage(wxWindow* parent):wxPanel(parent, wxID_ANY)
{
    m_process_ignore_list =new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
        wxDefaultSize, wxTE_MULTILINE|wxVSCROLL|wxSIMPLE_BORDER );
    m_disable_process_path = new wxCheckBox(this, wxID_ANY, wxT("Disable process in folders"));
    m_OK = new wxButton(this, wxID_ANY, _T("OK"));


    wxGridBagSizer* sizer = new wxGridBagSizer();
    int col = 0;
    int row = 0;

    sizer->Add(m_disable_process_path, wxGBPosition(row, col));
    col++;
    sizer->Add(m_process_ignore_list, wxGBPosition(row, col), wxGBSpan(1, 4), wxEXPAND | wxALL);


    row++;
    col=0;
    sizer->Add(m_OK, wxGBPosition(row, col));

    sizer->AddGrowableCol(1);
    sizer->AddGrowableRow(0);

    SetSizer(sizer);
    sizer->SetSizeHints(this);
    
    wxString ignore_path_list = wxConfigBase::Get()->Read(OPTIONS_IgnorePathList);
    bool enable_ignore_path = (0!=wxConfigBase::Get()->Read(OPTIONS_EnableIgnoreList, true));

    m_disable_process_path->SetValue(enable_ignore_path);
    if (enable_ignore_path && ignore_path_list.length())
    {
        ignore_path_list.Replace(wxT(";"), wxT("\n"));
        m_process_ignore_list->SetValue(ignore_path_list);
    }
    if (enable_ignore_path && ignore_path_list.length()==0)
    {
        m_disable_process_path->SetValue(true);
        std::basic_stringstream<TCHAR> ts;
        ts << wxT("C:\\Windows\n")
            << wxT("c:\\Program Files\n")
            << wxT("c:\\Documents and Settings\n")
            ;
        m_process_ignore_list->SetValue(ts.str());
    }

}

OptionsPage::~OptionsPage(void)
{
    OnSaveConfig();
    wxConfigBase::Get()->Flush();
}

void OptionsPage::OnSaveConfig()
{
    wxConfigBase::Get()->Write(OPTIONS_EnableIgnoreList, m_disable_process_path->GetValue());
    wxString ignore_list = m_process_ignore_list->GetValue();
    ignore_list.Replace(wxT("\n"), wxT(";"));
    wxConfigBase::Get()->Write(OPTIONS_IgnorePathList,ignore_list);
}

void OptionsPage::OnButton( wxCommandEvent& event )
{
    if (event.GetEventObject()==m_OK)
    {
        OnSaveConfig();
        wxConfigBase::Get()->Flush();
        wxGetApp().FireOptionsChanged();
    }
}
