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
	virtual ~GuiFindDlg();
	void OnButton(wxCommandEvent& event);
	void OnFind(wxCommandEvent& event);
	void AppendResult(wxString msg);
	void OnAppendResult (wxCustomEvent& evt);
	void OnUpdateResult(wxCommandEvent &evt);
	void OnChooseRoot(wxCommandEvent& evt);
	void OnCustomEvent(wxEvent &evt);
	void OnClose(wxCloseEvent& event);
	void OnTimerTimeout(wxTimerEvent& event);
	void CreateGUI();
	DECLARE_EVENT_TABLE()

    DECLARE_DYNAMIC_CLASS(GuiFindDlg)

	wxButton *m_btnFind{nullptr};
	wxButton *m_btnCancel{nullptr};
	wxButton *m_btnClose{nullptr};
	wxButton *m_btnChooseRoot{nullptr};
	wxTextCtrl *m_filename_pattern_tc{nullptr};
	wxTextCtrl *m_filename_pattern_exclude{nullptr};
	wxTextCtrl *m_content_pattern_tc{nullptr};
	wxTextCtrl *m_result_tc{nullptr};
	wxTextCtrl *m_root_path_tc{nullptr};
	wxTimer* m_pTimer{nullptr};


	std::ofstream m_log;
	bool m_wnd_closed{true};
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
