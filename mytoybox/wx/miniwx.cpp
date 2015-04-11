// [onega@localhost wx]$ wx-config --cppflags --libs base core aui
// -I/usr/local/lib/wx/include/gtk3-unicode-3.0 -I/usr/local/include/wx-3.0 -D_FILE_OFFSET_BITS=64 -DWXUSINGDLL -D__WXGTK__
// -L/usr/local/lib -pthread   -lwx_baseu-3.0 -lwx_gtk3u_core-3.0 -lwx_gtk3u_aui-3.0 
// g++ -I/usr/local/lib/wx/include/gtk3-unicode-3.0 -I/usr/local/include/wx-3.0 -D_FILE_OFFSET_BITS=64 -DWXUSINGDLL -D__WXGTK__ -L/usr/local/lib -o miniwx miniwx.cpp -pthread   -lwx_baseu-3.0 -lwx_gtk3u_core-3.0 -lwx_gtk3u_aui-3.0
#include <wx/wx.h>
class MiniWxApp : public wxApp
{
virtual bool OnInit();
};
IMPLEMENT_APP(MiniWxApp)
class MyFrame : public wxFrame
{
public:
MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
void OnQuit(wxCommandEvent& event){Close(true); }
void OnAbout(wxCommandEvent& event);
void OnEraseBackground( wxEraseEvent& event )
{
wxClientDC dc(this);
dc.Clear();
}
void OnPaint(wxPaintEvent& evt)
{    wxClientDC dc(this);
wxDateTime dt=wxDateTime::Now();
wxString txt = dt.FormatTime();
dc.SetTextForeground( *wxBLUE );
dc.SetBackgroundMode(wxTRANSPARENT);
dc.DrawText(txt, 0, 0);
}
};

bool MiniWxApp::OnInit()
{
MyFrame *frame = new MyFrame( ("Onega’s mini wxWidgets application"),
wxDefaultPosition, wxSize(450, 350));
frame->Show(true);
SetTopWindow(frame);
return true;
}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame( NULL, -1, title, pos, size )
{
wxMenuBar *menuBar = new wxMenuBar;
wxMenu *menuFile = new wxMenu;
menuFile->Append( wxID_ABOUT, ("&About…") );
menuFile->Append( wxID_EXIT, ("E&xit") );
menuBar->Append(menuFile, ("&File"));
SetMenuBar(menuBar);
Connect( wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED,
(wxObjectEventFunction) &MyFrame::OnQuit );
Connect( wxID_ABOUT, wxEVT_COMMAND_MENU_SELECTED,
(wxObjectEventFunction) &MyFrame::OnAbout );
Connect(wxEVT_PAINT, wxPaintEventHandler(MyFrame::OnPaint));
Connect(wxEVT_ERASE_BACKGROUND, wxEraseEventHandler(MyFrame::OnEraseBackground));
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
wxString caption = ("Onega’s mini wxWidgets application.");
wxString body = ("cl /EHsc /MD -DUNICODE=1 "
"-ID:\\src\\wxWidgets-2.9.1\\include "
"-ID:\\src\\wxWidgets-2.9.1\\lib\\vc_libvc90\\mswu miniwx.cpp "
"/link -LIBPATH:D:\\src\\wxWidgets-2.9.1\\lib\\vc_libvc90 "
"gdi32.lib rpcrt4.lib user32.lib Comdlg32.lib comctl32.lib "
"ole32.lib oleaut32.lib Advapi32.lib shell32.lib Winspool.lib wxmsw29u.lib");
wxMessageBox( body, caption, wxOK|wxICON_INFORMATION, this );
}
// miniwx.exe is built by VC++ 2010 + wxWidgets 2.9.1. File size: 2.17MB.
