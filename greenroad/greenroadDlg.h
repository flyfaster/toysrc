// greenroadDlg.h : header file
//

#if !defined(AFX_GREENROADDLG_H__D78A69A0_D587_4887_8601_88B14C2EB683__INCLUDED_)
#define AFX_GREENROADDLG_H__D78A69A0_D587_4887_8601_88B14C2EB683__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <MEMORY>
/////////////////////////////////////////////////////////////////////////////
// CGreenroadDlg dialog
#include "resource.h"
#include "SystemTray.h"
#include "afxcmn.h"
#include <string>
#include <map>
const int PATH_LEN = MAX_PATH+1024;

class CGreenroadDlg : public CDialog
{
// Construction
public:
	CGreenroadDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CGreenroadDlg)
	enum { IDD = IDD_GREENROAD_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGreenroadDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CGreenroadDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDo();
	afx_msg void OnUndo();
	afx_msg void OnChooseSource();
	afx_msg void OnSetDestination();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnGetMd5();
	afx_msg void OnDestroy();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void common_step(char *pkey, size_t key_len,bool bencrypt,LPCTSTR src_file,LPCTSTR dest_file);
	std::auto_ptr< CSystemTray> m_pTrayIcon;
	char m_key_buf[32];
	size_t m_key_len;
	bool read_key();
	std::map<std::basic_string<TCHAR>, std::basic_string<TCHAR> > m_files; //filename, md5
	CFont m_GbFont;
	std::basic_string<TCHAR> zip();
	bool unzip(std::basic_string<TCHAR> src_path,std::basic_string<TCHAR> dest_dir);
	std::basic_string<TCHAR> get_work_dir();
	std::basic_string<TCHAR> get_temp_filename();
public:
	afx_msg void OnBnClickedUsePlainKey();
	CListCtrl m_lv_files;
	afx_msg void OnBnClickedClear();
	afx_msg void OnBnClickedDelete();
	afx_msg void OnBnClickedCopy();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GREENROADDLG_H__D78A69A0_D587_4887_8601_88B14C2EB683__INCLUDED_)
