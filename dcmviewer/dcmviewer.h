// dcmviewer.h : main header file for the dcmviewer application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include <boost/shared_ptr.hpp>
// CdcmviewerApp:
// See dcmviewer.cpp for the implementation of this class
//

class CdcmviewerApp : public CWinApp
{
public:
	CdcmviewerApp();
	CMultiDocTemplate* m_pEditDocTemplate;
	CMultiDocTemplate* m_pImageDocTemplate;
	CMultiDocTemplate* m_pHistDocTemplate;
	CMultiDocTemplate* m_pSearchDocTemplate;
	CView* SwitchView(CView* pShowView);
	CEditView *m_pEdit;
	void CreateView(CDocument *pDoc,CView *pView);
	COLORREF m_font_color;
	int get_font_height()
	{
		return m_plogfont->lfHeight;
	}
	HBRUSH get_background_brush()
	{
		return (HBRUSH)(*m_brush_background.get() );
	}
	LOGFONT* get_logfont()
	{
		return m_plogfont;
	}
// Overrides
public:
	virtual BOOL InitInstance();
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
	afx_msg void OnFileNew();
	std::string get_res_string(UINT res_id,LPCTSTR res_type);
	void ui_changed();
	UINT get_server_port();
	boost::shared_ptr<dicom::Server> m_dicom_server;
// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnViewBackgroundColor();
	afx_msg void OnViewFont();
	
private:
	COLORREF m_background_color;
	LOGFONT *m_plogfont;
	std::auto_ptr<CFont> m_pfont;
	std::auto_ptr<CBrush> m_brush_background;
	void setup_dicom_server();
public:
	afx_msg void OnFileOpen();
	bool is_text_file(LPCTSTR filename);
	static void DebugOutput(LPCTSTR msg);
};
bool end_with(std::basic_string<TCHAR> s,std::basic_string<TCHAR> postfix,bool ignore_case=true);

extern CdcmviewerApp theApp;