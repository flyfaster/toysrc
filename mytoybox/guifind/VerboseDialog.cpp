/*
 * VerboseDialog.cpp
 *
 *  Created on: 8 Apr 2010
 *      Author: onega
 */
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include <wx/gbsizer.h>
#include <wx/textctrl.h>
#include "VerboseDialog.h"

BEGIN_EVENT_TABLE(VerboseDialog, wxDialog)
	EVT_BUTTON(wxID_ANY, VerboseDialog::OnButton)
	EVT_CLOSE(VerboseDialog::OnClose)
END_EVENT_TABLE()


VerboseDialog::VerboseDialog(wxWindow *parent)
    : wxDialog(parent, wxID_ANY, wxString(_T("Log dialog")),
    		wxDefaultPosition, wxSize(800, 600),
    		wxRESIZE_BORDER )
{
	m_btnClose = new wxButton(this, wxID_CANCEL, _T("&Close"));
	m_btnClear = new wxButton(this, wxID_ANY, _T("Clear"));

	m_status = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
					wxDefaultSize, wxTE_MULTILINE|wxVSCROLL|wxSIMPLE_BORDER );
	wxGridBagSizer* sizer = new wxGridBagSizer();
	sizer->Add(m_status, wxGBPosition(0,0), wxGBSpan(1, 2), wxEXPAND | wxALL);
	sizer->AddGrowableCol(0);
	sizer->AddGrowableRow(0);
	sizer->AddGrowableCol(1);
	sizer->Add(m_btnClose, wxGBPosition(1, 0));
	sizer->Add(m_btnClear, wxGBPosition(1, 1));
	SetSizer(sizer);
	sizer->SetSizeHints(this);
	sizer->Fit(this);

	logger = new wxLogTextCtrl( m_status );
	m_logOld = wxLog::SetActiveTarget( logger );
	wxLog::SetTimestamp( NULL );

}

VerboseDialog::~VerboseDialog() {
	delete wxLog::SetActiveTarget(m_logOld);
}

void VerboseDialog::OnClose(wxCloseEvent & event)
{
	Hide();
}



void VerboseDialog::OnButton(wxCommandEvent & event)
{
	if (event.GetEventObject()==m_btnClear)
		m_status->SetValue(wxEmptyString);
	if (event.GetEventObject()==m_btnClose)
		Hide();
}


