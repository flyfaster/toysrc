#include "frame.h"

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size) :
    wxFrame(NULL, wxID_ANY, title, pos, size)
{
    wxMenu* menuFile = new wxMenu;
    menuFile->Append(ID_Hello, "&Hello...\tCtrl-H",
                     "Help string shown in status bar for this menu item");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);
    wxMenu* menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");
    SetMenuBar(menuBar);
    CreateStatusBar();
    SetStatusText("Welcome to wxWidgets!");

    m_working_dir = new wxTextCtrl(this, wxID_ANY,
    		wxEmptyString, wxDefaultPosition, wxDefaultSize);
    m_envs = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
                              wxDefaultSize, wxTE_MULTILINE | wxHSCROLL);
    m_cmd = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
                              wxDefaultSize, wxTE_MULTILINE | wxHSCROLL);
    m_output_path = new wxTextCtrl(this, wxID_ANY,
    		wxEmptyString, wxDefaultPosition, wxDefaultSize);
    m_cmd_output = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
                              wxDefaultSize, wxTE_MULTILINE | wxHSCROLL);
    m_choose_working_dir = new wxButton(this, wxID_ANY, _T("WORKING_DIRECTORY"));
    m_update_env = new wxButton(this, wxID_ANY, _T("Environment Variables"));
    m_choose_cmds = new wxButton(this, wxID_ANY, _T("Command"));
    m_choose_output_path = new wxButton(this, wxID_ANY, _T("Output Path"));

    m_run = new wxButton(this, wxID_ANY, _T("Run"));
    m_copy = new wxButton(this, wxID_ANY, _T("Copy"));
    m_reset = new wxButton(this, wxID_ANY, _T("Reset"));
	wxBoxSizer *hbox1 = new wxBoxSizer(wxHORIZONTAL);
	hbox1->Add(m_run);
	hbox1->Add(m_copy);
	hbox1->Add(m_reset);

	m_choose_working_dir->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
			wxCommandEventHandler(MyFrame::OnSelectWorkingDir), nullptr, this);
	m_choose_output_path->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
			wxCommandEventHandler(MyFrame::OnSelectOutputPath), nullptr, this);

	wxGridBagSizer *m_fgsizer = new wxGridBagSizer;
	int row = 0;
	m_fgsizer->Add(m_choose_working_dir, wxGBPosition(row,0));
	m_fgsizer->Add(m_working_dir, wxGBPosition(row,1), wxGBSpan(1,1), wxGROW);
	++row;
	m_fgsizer->Add(m_update_env, wxGBPosition(row,0), wxGBSpan(1,1));
	m_fgsizer->Add(m_envs, wxGBPosition(row,1), wxGBSpan(2,1), wxGROW);
	++row;
	m_fgsizer->AddGrowableRow(row);
	++row;
	m_fgsizer->Add(m_choose_cmds, wxGBPosition(row,0), wxGBSpan(1,1));
	m_fgsizer->Add(m_cmd, wxGBPosition(row,1), wxGBSpan(2,1), wxGROW);
	++row;
	m_fgsizer->AddGrowableRow(row);
	++row;
	m_fgsizer->Add(m_choose_output_path, wxGBPosition(row,0));
	m_fgsizer->Add(m_output_path, wxGBPosition(row,1), wxGBSpan(1,1), wxGROW);
	++row;
	m_fgsizer->Add(m_cmd_output, wxGBPosition(row,1), wxGBSpan(2,1), wxGROW);
	++row;
	m_fgsizer->AddGrowableRow(row);
	++row;
	m_fgsizer->Add(hbox1, wxGBPosition(row,0), wxGBSpan(1,2), wxGROW);
	m_fgsizer->AddGrowableCol(1);
	this->SetSizer(m_fgsizer);
}

void MyFrame::OnSelectWorkingDir(wxCommandEvent& event)
{
    wxString dirHome;
    wxGetHomeDir(&dirHome);
    wxDirDialog dialog(this, _T("Choose command working directory"), dirHome,
                       wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if (dialog.ShowModal() == wxID_OK)
    {
        m_working_dir->SetValue(dialog.GetPath());
    }
}

void MyFrame::OnSelectOutputPath(wxCommandEvent& event)
{
    wxFileDialog* openFileDialog = new wxFileDialog(
        this, _("Select output file path"), "", "", "*", wxFD_OPEN, wxDefaultPosition);

    if (openFileDialog->ShowModal() == wxID_OK)
    {
        m_output_path->SetValue(openFileDialog->GetPath());
    }
}

void MyFrame::OnQuit(wxCommandEvent& event)
{
	Close(true);
}

void MyFrame::OnRun(wxCommandEvent&)
{
}

void MyFrame::OnCopy(wxCommandEvent&)
{
}

void MyFrame::OnClear(wxCommandEvent&)
{
}
