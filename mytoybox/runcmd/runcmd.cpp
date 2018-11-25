#include "runcmd.h"
#include "frame.h"

// clang-format off
wxIMPLEMENT_APP (MiniWxApp);
// clang-format on

bool MiniWxApp::OnInit()
{
    wxString title;
    title.Printf(wxT("Runcmd pid:%lu %s"), wxGetProcessId(), wxVERSION_STRING);
    MyFrame* frame = new MyFrame(title, wxDefaultPosition, wxSize(800, 600));
    frame->Show(true);
    SetTopWindow(frame);
    return true;
}
