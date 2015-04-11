#pragma once

class AboutPage : public wxPanel
{
public:
    AboutPage(wxWindow* parent);
    ~AboutPage(void);
    wxTextCtrl* m_ConsoleWnd;
};
