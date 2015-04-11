// SheelToolConfig.cpp : Implementation of CSheelToolConfig

#include "stdafx.h"
#include "SheelToolConfig.h"
#include ".\sheeltoolconfig.h"
TCHAR gcc_path_buf[MAX_PATH + 1024];

// CSheelToolConfig

LRESULT CSheelToolConfig::OnMouseMove(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// TODO: Add your message handler code here and/or call default
	if(wParam == MK_LBUTTON)
	{
		::ReleaseCapture();		
		::SendMessage(m_hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
	}
	return 0;
}

LRESULT CSheelToolConfig::OnBnClickedGccPath(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// Display a OPENFILE dialog to select path of g++.exe
	OPENFILENAME ofn;       // common dialog box structure
	//TCHAR szFile[260];       // buffer for file name
	HWND hwnd = NULL;              // owner window
	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = gcc_path_buf;
	//
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	//
	ofn.lpstrFile[0] = _T('\0');
	ofn.nMaxFile = sizeof(gcc_path_buf)/sizeof(gcc_path_buf[0]);
	ofn.lpstrFilter = _T("All\0*.*\0Exe\0*.EXE\0");
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	// Display the Open dialog box.
	if (GetOpenFileName(&ofn)==TRUE) 
	{
		SetDlgItemText(IDE_GCC_PATH,gcc_path_buf);
	}
	return 0;
}
