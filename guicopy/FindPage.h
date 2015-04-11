// FindPage.h: interface for the FindPage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FINDPAGE_H__D39F58C8_80D9_4747_A544_4AB53DFD3823__INCLUDED_)
#define AFX_FINDPAGE_H__D39F58C8_80D9_4747_A544_4AB53DFD3823__INCLUDED_

#pragma once
#include <boost/shared_ptr.hpp>

class find_file_thread;
class FindPage : public wxPanel  
{
public:
    FindPage(wxWindow* parent);
    virtual ~FindPage();
    wxXmlNode* GetNode();
    void Init( wxXmlNode * child );
    wxString GetName();
    void PostDoneEvent(wxString msg);
    void FindFile();
    void OnButton(wxCommandEvent& eventObj);
private:
    wxTextCtrl* m_source_dirs;
    wxTextCtrl* m_find_patterns;
    wxTextCtrl* m_find_results;
    wxTextCtrl* m_content_pattern;
    wxCheckBox* m_regex_content;
    wxButton* m_choose_source_dir;
    wxButton* m_copy_results;
    wxButton* m_clear_results;
	wxComboBox* m_source_type;
    wxCheckBox* m_use_regex;
    void OnCancel();
    void OnCopyResultToClipboard(wxCommandEvent& evt);
    void OnClearResults(wxCommandEvent& evt);
    void OnFindDoneEvent( wxCommandEvent &evt );
    DECLARE_EVENT_TABLE()

	void OnOK(void);
    bool IsInterestedFile(const tpath& file_path);
    std::list<tchar_string> m_filename_pattern_list;
    void ParsePatterList();
    std::list<tpath> path_list;
    boost::shared_ptr<find_file_thread> m_find_thread;
    volatile bool m_cancel;
    tchar_string m_result_str;
};

#endif // !defined(AFX_FINDPAGE_H__D39F58C8_80D9_4747_A544_4AB53DFD3823__INCLUDED_)
