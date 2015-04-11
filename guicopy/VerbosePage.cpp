#include "guicopy_include.h"

#include "verbosepage.h"

BEGIN_EVENT_TABLE(VerbosePage, wxPanel)
EVT_BUTTON(wxID_ANY, VerbosePage::OnButton)
EVT_KEY_UP(VerbosePage::OnKeyUp)

END_EVENT_TABLE()



VerbosePage::VerbosePage(wxWindow* parent):wxPanel(parent, wxID_ANY)
{
	m_status = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, wxTE_MULTILINE|wxVSCROLL|wxSIMPLE_BORDER );
	wxGridBagSizer* sizer = new wxGridBagSizer();
	sizer->Add(m_status, wxGBPosition(0,0), wxDefaultSpan, wxEXPAND | wxALL);
	sizer->AddGrowableCol(0);
	sizer->AddGrowableRow(0);
	SetSizer(sizer);
	sizer->SetSizeHints(this);
	sizer->Fit(this);

	logger = new wxLogTextCtrl( m_status );
	m_logOld = wxLog::SetActiveTarget( logger );
	wxLog::SetTimestamp( "" );
}

VerbosePage::~VerbosePage()
{
#if wxUSE_LOG
	delete wxLog::SetActiveTarget(m_logOld);
#endif // wxUSE_LOG
}

wxXmlNode* VerbosePage::GetNode()
{
	return NULL;
}

void VerbosePage::Init( wxXmlNode* p )
{

}

void VerbosePage::OnButton( wxCommandEvent& ev )
{
	wxButton* active_button = dynamic_cast<wxButton*>(ev.GetEventObject());
	if (active_button)
	{
		if (active_button->GetLabelText()==wxT("Cancel"))
		{
			m_status->SetValue(wxEmptyString);
		}
	}
}

wxString VerbosePage::GetName()
{
	wxString type_name =  wxConvLocal.cMB2WX(typeid(*this).name());
	int pos = type_name.find(_T(' '));
	if (pos>0)
	{
		return type_name.substr(pos+1);
	}
	return type_name;
}

void VerbosePage::OnKeyUp( wxKeyEvent& event )
{
	if (event.GetKeyCode()==WXK_DELETE)
	{
		m_status->SetValue(wxEmptyString);
		//wxLogMessage("event.id=%d, event.type=%d, wxEVT_KEY_UP=%d, wxEVT_KEY_DOWN=%d, wxEVT_CHAR=%d", 
		//	event.GetId(), event.GetEventType(),
		//	wxEVT_KEY_UP, wxEVT_KEY_DOWN, wxEVT_CHAR);
		//event.GetId()
	}
	//event.Skip(true);
}