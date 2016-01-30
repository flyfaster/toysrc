/*
 * wxfront.h
 *
 *  Created on: Jan 3, 2016
 *      Author: onega
 */

#ifndef WXFRONT_H_
#define WXFRONT_H_
#define wxUSE_UNICODE 1
//#define __WXGTK__
#define _FILE_OFFSET_BITS 64
#include <wx/wx.h>
#include <sstream>
#include <wx/clipbrd.h>
#include <wx/gbsizer.h>

class MiniWxApp: public wxApp {
	virtual bool OnInit();
};
IMPLEMENT_APP (MiniWxApp)

class MyFrame: public wxFrame {
public:
	MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
	void OnQuit(wxCommandEvent& event);
	void OnEraseBackground(wxEraseEvent& event);
	void OnPaint(wxPaintEvent& evt);
	void OnCopyClick(wxCommandEvent&);
	void OnReset(wxCommandEvent&);
	void OnEnterKey(wxKeyEvent& ev);
private:
	void reset();
	void InitButtons();
	wxTextCtrl *m_chinese;
	wxTextCtrl *m_pinyin;
	wxButton *m_copy;
	wxButton *m_reset;
};

#endif /* WXFRONT_H_ */
