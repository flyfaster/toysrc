#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// AddressBookDlg dialog

class AddressBookDlg : public CDialog
{
	DECLARE_DYNAMIC(AddressBookDlg)

public:
	AddressBookDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~AddressBookDlg();

// Dialog Data
	enum { IDD = IDD_ADDRESS_BOOK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedDelete();
	CComboBox m_cmbname;
	CIPAddressCtrl m_ip;
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelendokAddressName();
	static void FillCmb(CComboBox &cmb);
};
