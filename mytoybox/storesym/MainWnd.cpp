/*
 * MainWnd.cpp
 *
 *  Created on: Feb 2, 2015
 *      Author: onega
 */
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/datetime.h"
#include "wx/image.h"
#include "wx/bookctrl.h"
#include "wx/artprov.h"
#include "wx/imaglist.h"
#include "wx/sysopt.h"
#include "wx/gbsizer.h"
#include <wx/textctrl.h>
#include "wx/dirdlg.h"
#include <wx/filedlg.h>
#include "MainWnd.h"

IMPLEMENT_DYNAMIC_CLASS( MainWnd, wxFrame )

BEGIN_EVENT_TABLE(MainWnd, wxFrame)
END_EVENT_TABLE()

MainWnd::MainWnd() {

}

MainWnd::MainWnd(wxWindow* parent) :
		wxFrame(NULL, wxID_ANY, wxString("Select..."), wxDefaultPosition,
				wxSize(800, 600)) {
	m_select_bin = new wxButton(this, wxID_ANY, _T("&Binary"));
	m_select_symbol = new wxButton(this, wxID_ANY, _T("Symbol"));
	m_select_dumpsym = new wxButton(this, wxID_ANY, _T("dump_syms"));

	m_bin_path = new wxTextCtrl(this, wxID_ANY, wxEmptyString);
//	new wxTextCtrl(this, wxID_ANY, wxEmptyString,
//                wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
	m_sym_path = new wxTextCtrl(this, wxID_ANY, wxEmptyString);
	m_dumpsym_path = new wxTextCtrl(this, wxID_ANY, wxEmptyString);

	wxGridBagSizer *gbs = new wxGridBagSizer();

	m_select_bin->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
			wxCommandEventHandler(MainWnd::OnSelectBinary), NULL, this);
	m_select_symbol->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
			wxCommandEventHandler(MainWnd::OnSelectSymbol), NULL, this);
	m_select_dumpsym->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
			wxCommandEventHandler(MainWnd::OnSelectDumpSyms), NULL, this);
	int col = 0;
	int row = 0;
	int col_span = 3;

//    gbs->AddGrowableRow(3);
	gbs->Add(m_select_bin, wxGBPosition(row, col), wxGBSpan(1, 1),
			wxALIGN_CENTER | wxALL);
	col++;
	gbs->Add(m_bin_path, wxGBPosition(row, col), wxGBSpan(1, col_span),
			wxEXPAND | wxHORIZONTAL, 5);
	row++;
	col = 0;
	gbs->Add(m_select_symbol, wxGBPosition(row, col), wxGBSpan(1, 1),
			wxALIGN_CENTER | wxALL);
	col++;
	gbs->Add(m_sym_path, wxGBPosition(row, col), wxGBSpan(1, col_span),
			wxEXPAND | wxHORIZONTAL, 5);
	row++;
	col = 0;
	gbs->Add(m_select_dumpsym, wxGBPosition(row, col));
	col++;
	gbs->Add(m_dumpsym_path, wxGBPosition(row, col), wxGBSpan(1, col_span),
			wxEXPAND | wxHORIZONTAL);
	gbs->AddGrowableCol(1);
	this->SetSizer(gbs);
	gbs->SetSizeHints(this);
//    gbs->Fit(this);

}

MainWnd::~MainWnd() {
}

void MainWnd::OnSelectBinary(wxCommandEvent& event) {
	wxFileDialog* openFileDialog = new wxFileDialog(this,
			_("Select executable/shared library"), "", "", "*",
			wxFD_OPEN | wxFD_FILE_MUST_EXIST, wxDefaultPosition);

	if (openFileDialog->ShowModal() == wxID_OK) {
		m_bin_path->SetValue(openFileDialog->GetPath());
	}
}
void MainWnd::OnSelectSymbol(wxCommandEvent& event) {
	wxString dirHome;
	wxGetHomeDir(&dirHome);
	wxDirDialog dialog(this, _T("Choose symbol path"), dirHome,
			wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
	if (dialog.ShowModal() == wxID_OK) {
		m_sym_path->SetValue(dialog.GetPath());
	}
}

void MainWnd::OnSelectDumpSyms(wxCommandEvent& event) {
	wxFileDialog* openFileDialog = new wxFileDialog(this, _("Select dump_syms"),
			"", "", "*", wxFD_OPEN | wxFD_FILE_MUST_EXIST, wxDefaultPosition);

	if (openFileDialog->ShowModal() == wxID_OK) {
		m_dumpsym_path->SetValue(openFileDialog->GetPath());
	}
}
