/*
 * VerboseDialog.h
 *
 *  Created on: 8 Apr 2010
 *      Author: onega
 */

#ifndef VERBOSEDIALOG_H_
#define VERBOSEDIALOG_H_

class VerboseDialog : public wxDialog {
public:
	VerboseDialog(wxWindow *parent);
	virtual ~VerboseDialog();
    void OnButton(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);

private:
    DECLARE_EVENT_TABLE()
    wxTextCtrl* m_status;
	wxLogTextCtrl* logger;
	wxLog *m_logOld;
	wxButton *m_btnClose;
	wxButton *m_btnClear;
};

#endif /* VERBOSEDIALOG_H_ */
