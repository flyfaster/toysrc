#pragma once
#ifndef _CMultiExtFilterFileDialog_H
#define _CMultiExtFilterFileDialog_H
const int WM_SETUP_FLAGS = WM_APP+0x500;
// CMultiExtFilterFileDialog
//HOWTO: Enhance File Open Dialog with Multiple Extension Filters PSS ID Number: 200421
//HOWTO: OfnKing Demonstrates CFileDialog Customization PSS ID Number: 195034
class CMultiExtFilterFileDialog : public CFileDialog
{
	DECLARE_DYNAMIC(CMultiExtFilterFileDialog)

public:
	CMultiExtFilterFileDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);
	virtual ~CMultiExtFilterFileDialog();
private:
	BOOL m_bFileMustExist;
protected:
	afx_msg LRESULT OnSetupFlags(WPARAM, LPARAM);  // <-- Add this

	DECLARE_MESSAGE_MAP()
public:
	virtual INT_PTR DoModal();
protected:
	virtual BOOL OnFileNameOK();
};

#endif

