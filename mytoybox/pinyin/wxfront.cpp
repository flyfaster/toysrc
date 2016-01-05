/*
 * wxfront.cpp
 *
 *  Created on: Jan 3, 2016
 *      Author: onega
 */

#include "wxfront.h"
#include "main.h"
#ifdef _MSC_VER
#include "resource.h"
#endif

bool MiniWxApp::OnInit() {
	wxString title;
	title.Printf(wxT("Onega's mini dictionary pid:%lu "), wxGetProcessId());
	MyFrame *frame = new MyFrame( title, wxDefaultPosition, wxDefaultSize);
	frame->Show(true);
	SetTopWindow (frame);
	return true;
}

void MyFrame::InitButtons() {
	m_copy = new wxButton(this, wxID_ANY, wxT("Process"), wxDefaultPosition,
			wxDefaultSize, 0);
	m_reset = new wxButton(this, wxID_ANY, wxT("Clear"), wxDefaultPosition,
			wxDefaultSize, 0);
	m_copy->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
			wxCommandEventHandler(MyFrame::OnCopyClick), NULL, this);
	m_reset->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
			wxCommandEventHandler(MyFrame::OnReset), NULL, this);
}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size) :
		wxFrame(NULL, -1, title, pos, size) {
	wxMenuBar *menuBar = new wxMenuBar;
	wxMenu *menuFile = new wxMenu;
	menuFile->Append(wxID_EXIT, wxT("E&xit"));
	menuBar->Append(menuFile, wxT("&File"));
	SetMenuBar(menuBar);

	InitButtons();
	m_chinese = new wxTextCtrl(this, wxID_ANY);
	m_pinyin = new wxTextCtrl(this, wxID_ANY,
			wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxHSCROLL);
	m_pinyin->SetWindowStyle(m_pinyin->GetWindowStyle() & ~wxTE_DONTWRAP | wxTE_BESTWRAP);

	wxGridBagSizer *m_fgsizer = new wxGridBagSizer;
	int row = 0;
	m_fgsizer->Add(new wxStaticText(this, wxID_ANY, "Chinese"), wxGBPosition(row,0));
	m_fgsizer->Add(m_chinese, wxGBPosition(row,1), wxDefaultSpan, wxGROW);
	row++;
	m_fgsizer->Add(m_copy, wxGBPosition(row,0));
	m_fgsizer->Add(m_pinyin, wxGBPosition(row,1), wxGBSpan(3,1), wxGROW);
	row++;
	m_fgsizer->Add(m_reset, wxGBPosition(row,0));
	m_fgsizer->AddGrowableCol(1);
	m_fgsizer->AddGrowableRow(row);
	this->SetSizer(m_fgsizer);

	Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED,
			(wxObjectEventFunction) & MyFrame::OnQuit);
	Connect(wxEVT_PAINT, wxPaintEventHandler(MyFrame::OnPaint));
	Connect(wxEVT_ERASE_BACKGROUND,
			wxEraseEventHandler(MyFrame::OnEraseBackground));
	reset();
#ifdef _MSC_VER	
	SetIcon(wxICON(IDR_MYICON));
#endif	
}
void MyFrame::OnQuit(wxCommandEvent& event) {
		Close(true);
	}
	void MyFrame::OnEraseBackground(wxEraseEvent& event) {
		wxClientDC dc(this);
		dc.Clear();
	}
	void MyFrame::OnPaint(wxPaintEvent& evt) {
		wxClientDC dc(this);
		wxDateTime dt = wxDateTime::Now();
		wxString txt;
		txt.Printf(wxT("pid:%lu "), wxGetProcessId());
		txt.Append(dt.FormatTime());
		dc.SetTextForeground(*wxBLUE);
		dc.SetBackgroundMode(wxTRANSPARENT);
		dc.DrawText(txt, 0, 0);
	}
	void MyFrame::OnCopyClick(wxCommandEvent&)
	{
		wxString chinese;
		chinese = m_chinese->GetValue();
		std::wstring pinyinstr = MainApp::Instance()->chinese_to_pinyin(chinese.ToStdWstring());
		m_pinyin->SetValue(wxString(pinyinstr));
	}
	void MyFrame::OnReset(wxCommandEvent&)
	{
		reset();
	}
	void MyFrame::reset()
	{
		m_pinyin->SetValue(wxEmptyString);
		m_chinese->SetValue(wxEmptyString);
	}
