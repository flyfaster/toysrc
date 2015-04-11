#include "StdAfx.h"
#include <string>
#include <wx/frame.h>
#include <wx/dialog.h>
#include <wx/panel.h>
#include <wx/gbsizer.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/combobox.h>
#include <wx/button.h>
#include <wx/filedlg.h>
#include <wtypes.h>
#include "SqliteSelectCallback.h"
#include "ImportDailyFrame.h"
#include "StockDB.h"

struct PrivateImportDailyFrame : public wxDialog
//template<>
//struct Loki::ImplOf<ImportDailyFrame> :public wxDialog
{
	PrivateImportDailyFrame();
	~PrivateImportDailyFrame();
	std::string m_symbol;
	std::string m_filename;
	void OnCloseWindow(wxCloseEvent& ev);
	void OnOK(wxCommandEvent& ev);
	void OnCancel(wxCommandEvent& ev);
	void OnChooseFile(wxCommandEvent& ev);
	wxTextCtrl* file_path_input;

	wxComboBox* symbol_input;
	static const int CHOOSE_DAILY_FILE_BUTTON_ID = 1000;
};
//typedef Loki::ImplOf<ImportDailyFrame> PrivateImportDailyFrame;

PrivateImportDailyFrame::PrivateImportDailyFrame() : wxDialog(NULL, wxID_ANY, wxT("Import daily quotes"),
											 wxDefaultPosition, wxDefaultSize)
{
	wxGridBagSizer *gbsizer=new wxGridBagSizer();

	wxStaticText* symbol_label = new wxStaticText(this, wxID_ANY, wxT("Symbol"));
	symbol_input = new wxComboBox(this, wxID_ANY);
	wxButton* file_selection = new wxButton(this, CHOOSE_DAILY_FILE_BUTTON_ID, wxT("Daily data..."));
	file_path_input = new wxTextCtrl(this, wxID_ANY);
	wxPanel* button_pane = new wxPanel(this, wxID_ANY);
	wxButton* ok_button = new wxButton(button_pane, wxID_OK, wxT("OK"));
	wxButton* cancel_button = new wxButton(button_pane, wxID_CANCEL, wxT("Cancel"));
	wxGridSizer* grid_sizer = new wxGridSizer(2, 1);
	grid_sizer->Add(ok_button);
	grid_sizer->Add(cancel_button);
	button_pane->SetSizer(grid_sizer);
	grid_sizer->SetSizeHints(button_pane);
	grid_sizer->Fit(button_pane);

	int row = 0;
	int col = 0;
	gbsizer->Add(symbol_label, wxGBPosition(row, col));
	col++;
	gbsizer->Add(symbol_input, wxGBPosition(row, col));
	row++;
	col=0;
	gbsizer->Add(file_selection, wxGBPosition(row, col));
	col++;
	gbsizer->Add(file_path_input, wxGBPosition(row, col), wxDefaultSpan, wxEXPAND|wxRIGHT);
	row++;
	col=0;
	gbsizer->Add(button_pane, wxGBPosition(row, col), wxGBSpan(1, 2), wxEXPAND|wxLEFT|wxRIGHT);
	gbsizer->AddGrowableCol(1);

	SetSizer(gbsizer);
	gbsizer->SetSizeHints(this);
	gbsizer->Fit(this);
	Connect(wxID_ANY, wxEVT_CLOSE_WINDOW, 
		wxCloseEventHandler( PrivateImportDailyFrame::OnCloseWindow));
	Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PrivateImportDailyFrame::OnOK));
	Connect(CHOOSE_DAILY_FILE_BUTTON_ID, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PrivateImportDailyFrame::OnChooseFile));

	shared_SqliteSelectCallback result = SingleStockDB::Instance().Query("Select symbol from symbol_code");
	int count = result->GetRecordCount();
	for(int i=0; i<count; i++)
	{
		symbol_input->AppendString( wxString::FromAscii(result->GetFieldValue(i,0).c_str()));
	}
}

PrivateImportDailyFrame::~PrivateImportDailyFrame()
{

}

void PrivateImportDailyFrame::OnOK(wxCommandEvent& ev)
{
	m_filename = file_path_input->GetValue().ToAscii();
	m_symbol = symbol_input->GetValue().ToAscii();
	//if (m_symbol.length() && m_filename.length())
	//	Show(false);
	//else
		Destroy();
	//Destroy();
}

void PrivateImportDailyFrame::OnCancel(wxCommandEvent& ev)
{
	Destroy();
}

void PrivateImportDailyFrame::OnChooseFile(wxCommandEvent& ev)
{
	wxFileDialog fileDialog( NULL, _("Open database"), wxEmptyString,
		wxEmptyString,
		_("History files (*.csv)|*.csv|All files (*.*)|*.*"),
		wxFD_OPEN|wxFD_FILE_MUST_EXIST);
	if (fileDialog.ShowModal() == wxID_OK)
	{
		//SingleStockDB::Instance().ImportDailyQuotes(_T("ADSK"), fileDialog.GetPath());
		file_path_input->SetValue(fileDialog.GetPath());
	}
}

ImportDailyFrame::ImportDailyFrame(void)
{
	d = new PrivateImportDailyFrame;
}

ImportDailyFrame::~ImportDailyFrame(void)
{
	if (d)
		delete d;
	d = NULL;
}

void PrivateImportDailyFrame::OnCloseWindow(wxCloseEvent& ev)
{
	EndModal(0);
	ev.Skip();
}
std::string ImportDailyFrame::GetSymbol()
{
	return d->m_symbol;
}

std::string ImportDailyFrame::GetDailyFile()
{
	return d->m_filename;
}

bool ImportDailyFrame::DoModal()
{
	d->ShowModal();//(true);
	return false;
}