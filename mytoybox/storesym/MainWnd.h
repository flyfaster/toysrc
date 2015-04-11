/*
 * MainWnd.h
 *
 *  Created on: Feb 2, 2015
 *      Author: onega
 */

#ifndef MAINWND_H_
#define MAINWND_H_

class MainWnd: public wxFrame {
public:
	MainWnd();
	MainWnd(wxWindow* parent);
	virtual ~MainWnd();
    DECLARE_EVENT_TABLE()
	void OnSelectBinary(wxCommandEvent& event);
    void OnSelectSymbol(wxCommandEvent& event);
    void OnSelectDumpSyms(wxCommandEvent& event);

DECLARE_DYNAMIC_CLASS(wxFrame)

    wxButton *m_select_bin;
    wxButton *m_select_symbol;
    wxButton *m_select_dumpsym;
    wxTextCtrl *m_bin_path;
    wxTextCtrl *m_sym_path;
    wxTextCtrl *m_dumpsym_path;
};

#endif /* MAINWND_H_ */
