#pragma once
#include <wx/wx.h>
#include <sstream>
#include <wx/clipbrd.h>
#include <wx/gbsizer.h>

class MiniWxApp : public wxApp
{
public:
    bool OnInit() override;
    int OnExit() override;
};

// clang-format off
wxDECLARE_APP(MiniWxApp);
// clang-format on
