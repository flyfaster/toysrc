#include <iostream>
#include <regex>
#include <sstream>
#include <iomanip>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include <wx/wx.h>
#include <wx/app.h>
#include <wx/gbsizer.h>

class MiniWxApp : public wxApp
{
public:
    bool OnInit() override;
};

// clang-format off
wxDECLARE_APP(MiniWxApp);
wxIMPLEMENT_APP(MiniWxApp);
// clang-format on

class MyFrame : public wxFrame
{
	wxTimer timer;
	wxTextCtrl *m_working_dir;
	wxTextCtrl *m_source_file;
	wxButton *m_choose_working_dir;
	wxButton *m_choose_source_file;
	wxButton *m_rename;
	wxButton *m_exit;
public:
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
    void OnQuit(wxCommandEvent& event)
    {
    	timer.Stop();
        Close(true);
    }
    void OnAbout(wxCommandEvent& event);
    void OnEraseBackground(wxEraseEvent& event)
    {
        wxClientDC dc(this);
        dc.Clear();
    }
    void OnPaint(wxPaintEvent& evt)
    {
    	wxPaintDC dc(this);
        wxDateTime dt = wxDateTime::Now();
        wxString txt = dt.FormatTime();
        dc.SetTextForeground(*wxBLUE);
        dc.SetBackgroundMode(wxTRANSPARENT);
        dc.DrawText(txt, 0, 0);
    }
    void OnTimer(wxTimerEvent & event)
    {
        Refresh();
    }
    void OnSelectOutputPath(wxCommandEvent& event);
    void OnSelectWorkingDir(wxCommandEvent& event);
    void OnRename(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
};

bool MiniWxApp::OnInit()
{
    auto title = wxString::Format("Hello %s (pid %lu)", wxVERSION_STRING,
                                  wxGetProcessId());
    MyFrame* frame = new MyFrame(title,
                                 wxDefaultPosition, wxSize(800, 350));
    frame->Show(true);
    SetTopWindow(frame);
    return true;
}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size) :
    wxFrame(NULL, -1, title, pos, size), timer(this)
{
    wxMenuBar* menuBar = new wxMenuBar;
    wxMenu* menuFile = new wxMenu;
    menuFile->Append(wxID_ABOUT, wxT("&About"));
    menuFile->Append(wxID_EXIT, wxT("E&xit"));
    menuBar->Append(menuFile, wxT("&File"));
    SetMenuBar(menuBar);
    Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED,
            (wxObjectEventFunction) &MyFrame::OnQuit);
    Connect(wxID_ABOUT, wxEVT_COMMAND_MENU_SELECTED,
            (wxObjectEventFunction) &MyFrame::OnAbout);
    Connect(wxEVT_PAINT, wxPaintEventHandler(MyFrame::OnPaint));
    Connect(wxEVT_ERASE_BACKGROUND, wxEraseEventHandler(MyFrame::OnEraseBackground));

    Bind(wxEVT_TIMER, &MyFrame::OnTimer, this, timer.GetId());
    timer.Start(1);
    m_choose_working_dir = new wxButton(this, wxID_ANY, _T("WORKING_DIRECTORY"));
    m_working_dir = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize);
    m_choose_source_file = new wxButton(this, wxID_ANY, _T("Source File"));
    m_source_file = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize);
	m_choose_working_dir->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
			wxCommandEventHandler(MyFrame::OnSelectWorkingDir), nullptr, this);
	m_choose_source_file->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
			wxCommandEventHandler(MyFrame::OnSelectOutputPath), nullptr, this);
	m_rename = new wxButton(this, wxID_ANY, _T("Rename"));
	m_exit = new wxButton(this, wxID_ANY, _T("Exit"));
	m_rename->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
			wxCommandEventHandler(MyFrame::OnRename), nullptr, this);
	m_exit->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
			wxCommandEventHandler(MyFrame::OnExit), nullptr, this);
	wxBoxSizer *hbox1 = new wxBoxSizer(wxHORIZONTAL);
	hbox1->Add(m_rename);
	hbox1->Add(m_exit);

	wxGridBagSizer *m_fgsizer = new wxGridBagSizer;
	int row = 0;
	m_fgsizer->Add(m_choose_source_file, wxGBPosition(row,0));
	m_fgsizer->Add(m_source_file, wxGBPosition(row,1), wxGBSpan(1,1), wxGROW);
	++row;
	m_fgsizer->Add(m_choose_working_dir, wxGBPosition(row,0));
	m_fgsizer->Add(m_working_dir, wxGBPosition(row,1), wxGBSpan(1,1), wxGROW);
	++row;
	m_fgsizer->Add(hbox1, wxGBPosition(row,0), wxGBSpan(1,2), wxGROW);
	m_fgsizer->AddGrowableCol(1);
	this->SetSizer(m_fgsizer);
}

void MyFrame::OnSelectOutputPath(wxCommandEvent& event)
{
    wxFileDialog* openFileDialog = new wxFileDialog(
        this, _("Select output file path"), "", "", "*", wxFD_OPEN, wxDefaultPosition);

    if (openFileDialog->ShowModal() == wxID_OK)
    {
    	m_source_file->SetValue(openFileDialog->GetPath());
    }
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

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString caption = ("wxWidgets application.");
    wxString body =
        ("");
    wxMessageBox(body, caption, wxOK | wxICON_INFORMATION, this);
}

void rename_file_with_tm(const std::string& source_file_name) {
	if (!boost::filesystem::exists( source_file_name ) )
	{
		std::cerr << "File not exist "
				  << source_file_name
				  << std::endl;
		return;
	}
	if (boost::filesystem::is_directory( source_file_name ) )
	{
		std::cerr << "File is_directory "
				  << source_file_name
				  << std::endl;
		return;
	}
	std::regex re("[0-9]{8}");
	std::cmatch m;
	if (std::regex_search(source_file_name.c_str(), m, re)) {
		std::cerr << "File already has timestamp "
				  << source_file_name
				  << std::endl;
		return;
	}
	std::time_t last_write_time = boost::filesystem::last_write_time( source_file_name );
	auto last_slash_pos = source_file_name.rfind(boost::filesystem::path::preferred_separator);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&last_write_time), "%Y%m%d%H%M%S");
    std::string tmstr = ss.str();
    ss.str("");
    if (last_slash_pos != std::string::npos) {
    	ss << source_file_name.substr(0, last_slash_pos+1);
    	ss << tmstr << "-" << source_file_name.substr(last_slash_pos+1);
    } else {
    	ss << tmstr << "-" << source_file_name;
    }
    std::string to_file_name = ss.str();

    if (std::rename(source_file_name.c_str(), to_file_name.c_str())) {
        std::perror("Error renaming");
        return;
    } else {
    	std::cout << "New name: " << to_file_name << "\nOld name: " << source_file_name << std::endl;
    }
}

void MyFrame::OnRename(wxCommandEvent& event) {
	if(m_source_file->GetValue().empty()==false) {
		std::string source_file_name = m_source_file->GetValue().ToStdString();
		rename_file_with_tm(source_file_name);
	}

	if(m_working_dir->GetValue().empty()==false) {
		std::string source_dir_name = m_working_dir->GetValue().ToStdString();
		if (!boost::filesystem::exists( source_dir_name ) )
		{
			std::cerr << "Dir not exist "
					  << source_dir_name
					  << std::endl;
			return;
		}
		if (!boost::filesystem::is_directory( source_dir_name ) )
		{
			std::cerr << "Path not is_directory "
					  << source_dir_name
					  << std::endl;
			return;
		}

        for(boost::filesystem::directory_entry& entry : boost::make_iterator_range(boost::filesystem::directory_iterator(source_dir_name), {}))
        {
        	std::cout << entry << " " << typeid(entry).name()<< " \n";
        	if (boost::filesystem::is_regular_file(entry.path()))
        		rename_file_with_tm(entry.path().string());
        }
	}
}

void MyFrame::OnExit(wxCommandEvent& event) {
	Close(true);
}
