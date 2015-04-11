#pragma once

class CopyPage :public wxPanel
{
public:
    CopyPage(wxWindow* parent);
    ~CopyPage(void);
    void OnButton( wxCommandEvent& eventObj );
    void OnOK();
    void OnCancel();
    void OnKeyDown( wxKeyEvent& eventObj );
    wxXmlNode* GetNode();
    void Init( wxXmlNode * child );
    wxString GetName();
private:
    wxListBox* m_copy_ext_list;
    wxTextCtrl* m_copy_source;
    wxTextCtrl* m_copy_destination;
    wxButton* m_copy_choose_source;
    wxButton* m_copy_choose_destination;
    wxButton* m_copy_add_ext;
    wxButton* m_copy_remove_ext;
    wxRadioBox* m_copy_ext_flag;
    static LPCTSTR COPY_PAGE_INCLUDE_EXTENSIONS_NODE;
    wxCheckBox* m_add_timestamp;
    wxComboBox* m_operation_type;
    DECLARE_EVENT_TABLE()
};
