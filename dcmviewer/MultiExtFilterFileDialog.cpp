// MultiExtFilterFileDialog.cpp : implementation file
//

#include "stdafx.h"
#include "MultiExtFilterFileDialog.h"


// CMultiExtFilterFileDialog

IMPLEMENT_DYNAMIC(CMultiExtFilterFileDialog, CFileDialog)
CMultiExtFilterFileDialog::CMultiExtFilterFileDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
	if(dwFlags&OFN_FILEMUSTEXIST)
		m_bFileMustExist = TRUE;
	else
		m_bFileMustExist = FALSE;
}

CMultiExtFilterFileDialog::~CMultiExtFilterFileDialog()
{
}


BEGIN_MESSAGE_MAP(CMultiExtFilterFileDialog, CFileDialog)
	ON_MESSAGE(WM_SETUP_FLAGS, OnSetupFlags)

END_MESSAGE_MAP()



// CMultiExtFilterFileDialog message handlers


INT_PTR CMultiExtFilterFileDialog::DoModal()
{
    // If opening a file, and multi select is not enabled,
    // and no default extension is specified:
    if (m_bOpenFileDialog && !(m_ofn.Flags & OFN_ALLOWMULTISELECT) &&
		!m_ofn.lpstrDefExt)
	{
		m_bFileMustExist = m_ofn.Flags & OFN_FILEMUSTEXIST;
		m_ofn.Flags &= ~OFN_FILEMUSTEXIST;
	}

	return CFileDialog::DoModal();
}

BOOL CMultiExtFilterFileDialog::OnFileNameOK()
{
    // If saving, or if multi select is enabled, or a default extension
    // is available, preserve original functionality:
    if (!m_bOpenFileDialog ||
		(m_ofn.Flags & OFN_ALLOWMULTISELECT) ||
		(m_ofn.lpstrDefExt))
		return CFileDialog::OnFileNameOK();

	CString strFName = GetPathName();
	WIN32_FIND_DATA fd;

	// Use if the file exists as specified in the file name edit box:
	if (::FindFirstFile(strFName, &fd) != INVALID_HANDLE_VALUE)
	{
		// Accept file name:
		return 0;
	}
	// Use if a file extension was specified:
	if (strFName.ReverseFind('.') > strFName.ReverseFind('\\'))
	{
		// Reject file name:
		return 1;
	}

	CString strMultiExt, strExt;

	// Get current filter extensions:
	AfxExtractSubString(strMultiExt, m_ofn.lpstrFilter,
		2*m_ofn.nFilterIndex - 1, (TCHAR)'\0');

	// For every extension in the filter:
	for (int i = 0;
		AfxExtractSubString(strExt, strMultiExt, i, (TCHAR)';');
		i++)
	{
		// No '*' is allowed in filter extensions:
		if (_tcsrchr((LPCTSTR)strExt + 1, (TCHAR)'*'))
			continue;
		if (INVALID_HANDLE_VALUE !=
			::FindFirstFile(strFName+((LPCTSTR)strExt+1), &fd))
		{
			unsigned int flen = _tcslen(m_ofn.lpstrFile);
			// If the buffer is not big enough, ignore this extension:
			if (flen + strExt.GetLength() > m_ofn.nMaxFile)
				continue;

			_tcscpy(m_ofn.lpstrFile + flen, (LPCTSTR)strExt+1);
			// Accept the file name:
			return 0;
		}
	}

	// To get FileMustExist validation, reset the flag:
	if (m_bFileMustExist)
	{
		// Set the file exist check back to the original:
		m_ofn.Flags |= OFN_FILEMUSTEXIST;
		// Get default processing (for instance, the file must exist 
		// message box):
		PostMessage(WM_KEYDOWN, (WPARAM)(VK_RETURN), (LPARAM)(1));
		// Reset the file exist check:
		PostMessage(WM_SETUP_FLAGS);
		return 1;
	}
	else
		return 0;
}

LRESULT CMultiExtFilterFileDialog::OnSetupFlags(WPARAM, LPARAM)
{
    if (m_bFileMustExist)
        m_ofn.Flags &= ~OFN_FILEMUSTEXIST;
    return 0;
}
//Once these steps are completed, you can use the dialog as follows: 

//TCHAR szFilters[] =
//    _T("Text Files (*.txt;*.log;*.bak)\0*.txt;*.log;*.bak\0")
//    _T("C++ Files (*.cc;*.cpp;*.h;*.hpp)\0*.cc;*.cpp;*.h;*.hpp\0");
//CMultiExtFilterFileDialog ofdlg(TRUE);
//ofdlg.m_ofn.lpstrFilter = szFilters;
//if (ofdlg.DoModal() == IDOK)
//{
//    AfxGetApp()->m_pDocManager->OpenDocumentFile(ofdlg.m_ofn.lpstrFile);
//}
