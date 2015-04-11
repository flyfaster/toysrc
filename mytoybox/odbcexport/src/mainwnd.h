/*
Created by Onega Zhang
*/

#ifndef MAINWND_H
#define MAINWND_H

class MainWnd : public wxFrame
{
  public:
    MainWnd();
    void OnChooseFile(wxCommandEvent & event);
    void OnStart(wxCommandEvent& evt);
    void OnCancel(wxCommandEvent& evt);
    void OnExit(wxCommandEvent& evt);
    void OnTable(wxCommandEvent& evt);
    void UpdateDSN()    ;
  private:
    wxComboBox* m_table_input;
    wxComboBox* m_dsn_input;
    wxTextCtrl* m_file_input;
    wxButton* m_file_btn;
    wxRadioButton *m_export_radio;
    wxRadioButton *m_import_radio;
    wxButton* m_start_btn;
    wxButton* m_cancel_btn;
    wxButton* m_exit_btn;
    wxButton* m_table_btn;
    DECLARE_EVENT_TABLE()
};

#endif // MAINWND_H
