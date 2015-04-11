#pragma once

class ErrorPage : public wxPanel
{
public:
    ErrorPage(wxWindow* parent);
    ~ErrorPage(void);
    wxTextCtrl* m_error_code;
    wxTextCtrl* m_description;
    wxButton* m_find_btn;
    wxButton* m_copy_btn;
    wxButton* m_clear_btn;
    wxRadioButton* m_hex;
    wxRadioButton* m_dec;
    void OnButton(wxCommandEvent& event);
    void OnRadioButton(wxCommandEvent& event);
    DECLARE_EVENT_TABLE()

};
