#pragma once

class OptionsPage : public wxPanel
{
public:
    OptionsPage(wxWindow* parent);
    ~OptionsPage(void);

    void OnSaveConfig();
    void OnButton(wxCommandEvent& event);

    wxCheckBox* m_disable_process_path;
    wxTextCtrl* m_process_ignore_list;
    wxButton* m_OK;
    DECLARE_EVENT_TABLE()

};
