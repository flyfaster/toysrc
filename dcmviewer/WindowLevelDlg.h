#pragma once


// WindowLevelDlg dialog

class WindowLevelDlg : public CDialog
{
	DECLARE_DYNAMIC(WindowLevelDlg)

public:
	WindowLevelDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~WindowLevelDlg();

// Dialog Data
	enum { IDD = IDD_WINDOW_LEVEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	double m_wc;
	double m_ww;
	virtual BOOL OnInitDialog();
	bool is_valid();
};
