#pragma once

class AboutPage  : public wxPanel, private Loki::PimplOf<AboutPage>::Owner
{
public:
    AboutPage(wxWindow* parent);
    ~AboutPage(void);
};
