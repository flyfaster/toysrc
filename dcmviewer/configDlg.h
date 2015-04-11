#pragma once
#include "afxcmn.h"
#include "GridListCtrl.h"


// ConfigDlg dialog

class ConfigDlg : public CDialog
{
	DECLARE_DYNAMIC(ConfigDlg)

public:
	ConfigDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~ConfigDlg();

// Dialog Data
	enum { IDD = IDD_CONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CGridListCtrl m_lv;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
