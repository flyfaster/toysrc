#pragma once

#include "ResizableDialog.h"
#pragma comment(lib,"resizablelib.lib")

// CdllsDlg dialog

class CdllsDlg : public CResizableDialog
{
	DECLARE_DYNAMIC(CdllsDlg)

public:
	CdllsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CdllsDlg();

// Dialog Data
	enum { IDD = IDD_DCMVIEWER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedSave();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
