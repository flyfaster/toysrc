/*
 * GuiFindDlg.h
 *
 *  Created on: 8 Apr 2010
 *      Author: onega
 */

#ifndef GUIFINDDLG_H_
#define GUIFINDDLG_H_
#include <boost/thread.hpp>
#include <list>
#include "CustomEvent.h"
#include <fstream>
//typedef wxFrame ParentWindowType;


class GuiFindDlg : public wxFrame {
public:
	GuiFindDlg();
	GuiFindDlg(wxWindow* parent);
	virtual ~GuiFindDlg();
	void OnButton(wxCommandEvent& event);
	void OnFind(wxCommandEvent& event);
	void AppendResult(wxString msg);
	void OnAppendResult (wxCustomEvent& evt);
	void OnUpdateResult(wxCommandEvent &evt);
	void OnChooseRoot(wxCommandEvent& evt);
	void OnCustomEvent(wxEvent &evt);

	void CreateGUI();
	DECLARE_EVENT_TABLE()

    DECLARE_DYNAMIC_CLASS(GuiFindDlg)

	wxButton *m_btnFind;
	wxButton *m_btnCancel;
	wxButton *m_btnClose;
	wxButton *m_btnChooseRoot;
	wxTextCtrl *m_filename_pattern_tc;
	wxTextCtrl *m_content_pattern_tc;
	wxTextCtrl *m_result_tc;
	wxTextCtrl *m_root_path_tc;
	boost::mutex m_result_mx;
	std::list<wxString> m_result_list;
	std::ofstream m_log;
};

// label: file name pattern
// edit: file name patter input control
// label: file content pattern
// edit: file content patter input
// button: choose root folder
// edit: root folder input
// edit: result
// button: find
// button: cancel
// button: close



#endif /* GUIFINDDLG_H_ */
