// For compilers that support precompilation, includes "wx/wx.h".
// [onega@localhost wx]$ wx-config --cppflags --libs base core aui
// -I/usr/local/lib/wx/include/gtk3-unicode-3.0 -I/usr/local/include/wx-3.0 -D_FILE_OFFSET_BITS=64 -DWXUSINGDLL -D__WXGTK__
// -L/usr/local/lib -pthread   -lwx_baseu-3.0 -lwx_gtk3u_core-3.0 -lwx_gtk3u_aui-3.0 
// g++ -I/usr/local/lib/wx/include/gtk3-unicode-3.0 -I/usr/local/include/wx-3.0 -D_FILE_OFFSET_BITS=64 -DWXUSINGDLL -D__WXGTK__ -L/usr/local/lib  -o hello hello.cpp -pthread   -lwx_baseu-3.0 -lwx_gtk3u_core-3.0 -lwx_gtk3u_aui-3.0
#include <tchar.h>
#include <wx/wx.h>

//#pragma comment( linker, "/entry:\"mainCRTStartup\"" ) 
class MyApp : public wxApp
{
public:
    bool OnInit() override;
};

class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

private:
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    wxDECLARE_EVENT_TABLE();
    enum
    {
        ID_Hello = 1
    };
};

// clang-format off
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
	EVT_MENU(ID_Hello, MyFrame::OnHello)
    EVT_MENU(wxID_EXIT, MyFrame::OnExit)
	EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
wxEND_EVENT_TABLE()
wxDECLARE_APP(MyApp);
wxIMPLEMENT_APP(MyApp);
// clang-format on

bool MyApp::OnInit()
{
    auto title = wxString::Format("Hello %s (pid %lu)", wxVERSION_STRING,
                                  wxGetProcessId());
    MyFrame* frame = new MyFrame(title, wxPoint(50, 50), wxSize(450, 340));
    frame->Show(true);
    return true;
}

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
}

void MyFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("This is a wxWidgets' Hello world sample", "About Hello wxWidgets",
                 wxOK | wxICON_INFORMATION);
}

void MyFrame::OnHello(wxCommandEvent& event)
{
    wxLogMessage("Hello world from wxWidgets!");
}
