//#include <wx/wxprec.h>
//#include <wx/App.h>
//#include <wx/ListBox.h>
//#include <wx/TextCtrl.h>
//#include <wx/Panel.h>
//#include <wx/App.h>
//#include <wx/Frame.h>
//#include <wx/Menu.h>
//#include <wx/Button.h>
//#include <wx/stattext.h>
//#include <wx/msgdlg.h>
//#include <wx/dirdlg.h>
//#include <wx/textdlg.h>
//#include <wx/dir.h>
//#include <wx/thread.h>
//#include <wx/radiobox.h>
//#include <wx/datetime.h>
#include "guicopy_include.h"
#include "mainapp.h"
#include "mydlg.h"
#include "ConfigPage.h"

BEGIN_EVENT_TABLE(ConfigPage, wxPanel)
EVT_BUTTON(wxID_ANY, ConfigPage::OnButton)
END_EVENT_TABLE()

ConfigPage::ConfigPage(wxWindow* parent) : wxPanel(parent, wxID_ANY)
{
    m_btnConfigFile = new wxButton(this, wxID_ANY, _T("Config file..."));

    m_txtConfigFile = new wxTextCtrl(this, wxID_ANY);


    m_btnLoad = new wxButton(this, wxID_ANY, _T("Load"));
    m_btnSave = new wxButton(this, wxID_ANY, _T("Save"));

    wxGridBagSizer *sizer = new wxGridBagSizer;
    sizer->Add(m_btnConfigFile, wxGBPosition(0,0));
    sizer->Add(m_txtConfigFile, wxGBPosition(0,1), wxDefaultSpan, wxEXPAND | wxRIGHT);
    sizer->Add(m_btnLoad, wxGBPosition(1, 0));
    sizer->Add(m_btnSave, wxGBPosition(1,1));
    sizer->AddGrowableCol(1);

    SetSizer(sizer);
    sizer->SetSizeHints(this);
    sizer->Fit(this);
}

void ConfigPage::Load()
{
	SingleThreadsCol::Instance().LoadConfiguration();
}

void ConfigPage::Save()
{
    if (m_txtConfigFile->GetValue().length()==0)
        return;
	SingleThreadsCol::Instance().SaveConfiguration();
}

void ConfigPage::OnButton( wxCommandEvent& event )
{
    if (event.GetEventObject()==m_btnSave)
    {
        Save();
    }
    if (event.GetEventObject()==m_btnLoad)
    {
        Load();
    }
    if (event.GetEventObject()==m_btnConfigFile)
    {
        wxFileDialog filechooser(this, _T("Choose configuration xml file"),
            wxGetCwd(), wxGetApp().GetDefaultConfigFileName(), _T("*.xml"));
        if (filechooser.ShowModal() == wxID_OK)
        {
            m_txtConfigFile->SetValue(filechooser.GetPath());
        }
    }
}

wxXmlNode* ConfigPage::GetNode()
{
    wxXmlNode* pNode = new wxXmlNode(wxXML_ELEMENT_NODE, GetName());
    wxXmlNode* workingdir = new wxXmlNode(wxXML_ELEMENT_NODE, _T("WorkingDirectory"));
    pNode->AddChild(workingdir);
    workingdir->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxT(""), m_txtConfigFile->GetValue()));

    return pNode;
};

void ConfigPage::Init( wxXmlNode* p )
{

}

wxString ConfigPage::GetFileName()
{
    wxString ret = m_txtConfigFile->GetValue();
    if (ret.length()==0)
        ret = wxGetApp().GetDefaultConfigFileName();
    return ret;
}
