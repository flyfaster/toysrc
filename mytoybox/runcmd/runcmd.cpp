#include <google/protobuf/util/time_util.h>
#include "runcmd.h"
#include "frame.h"

// clang-format off
wxIMPLEMENT_APP (MiniWxApp);
// clang-format on

bool MiniWxApp::OnInit()
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;
    wxString title;
    title.Printf(wxT("Runcmd pid:%lu powered by %s"), wxGetProcessId(), wxVERSION_STRING);
    MyFrame* frame = new MyFrame(title, wxDefaultPosition, wxSize(800, 600));
    frame->Show(true);
    SetTopWindow(frame);
    return true;
}

int MiniWxApp::OnExit()
{
	google::protobuf::ShutdownProtobufLibrary();
	return 0;
}
