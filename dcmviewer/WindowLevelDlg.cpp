// WindowLevelDlg.cpp : implementation file
//

#include "stdafx.h"
#include "dcmviewer.h"
#include "WindowLevelDlg.h"
#include ".\windowleveldlg.h"


// WindowLevelDlg dialog

IMPLEMENT_DYNAMIC(WindowLevelDlg, CDialog)
WindowLevelDlg::WindowLevelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(WindowLevelDlg::IDD, pParent)
{
	m_ww = -1;
	m_wc = (std::numeric_limits<UINT16>::max)()+1;

}

WindowLevelDlg::~WindowLevelDlg()
{
}

void WindowLevelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(WindowLevelDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// WindowLevelDlg message handlers

void WindowLevelDlg::OnBnClickedOk()
{
	CString tmp;
	GetDlgItemText(IDE_WC,tmp);
	if(tmp.GetLength()>0)
		m_wc = atof(tmp);
	GetDlgItemText(IDE_WW,tmp);
	if(tmp.GetLength()>0)
		m_ww = atof(tmp);
	OnOK();
}

BOOL WindowLevelDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString tmp;
	tmp.Format("%f",m_wc);
	SetDlgItemText(IDE_WC,tmp);
	tmp.Format("%f",m_ww);
	SetDlgItemText(IDE_WW,tmp);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

bool WindowLevelDlg::is_valid()
{
	if(m_wc<=(std::numeric_limits<UINT16>::max)()&&m_ww>0)
		return true;
	return false;
}
