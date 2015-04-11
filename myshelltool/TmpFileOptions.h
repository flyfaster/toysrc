// TmpFileOptions.h : Declaration of the CTmpFileOptions

#pragma once

#include "resource.h"       // main symbols
#include <atlhost.h>


// CTmpFileOptions

class CTmpFileOptions : 
	public CAxDialogImpl<CTmpFileOptions>
{
public:
	CTmpFileOptions()
	{
	}

	~CTmpFileOptions()
	{
	}

	enum { IDD = IDD_DIALOG1 };

BEGIN_MSG_MAP(CTmpFileOptions)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	COMMAND_HANDLER(IDOK, BN_CLICKED, OnClickedOK)
	COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnClickedCancel)
	CHAIN_MSG_MAP(CAxDialogImpl<CTmpFileOptions>)
END_MSG_MAP()

// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CAxDialogImpl<CTmpFileOptions>::OnInitDialog(uMsg, wParam, lParam, bHandled);
		HWND hWndList=GetDlgItem(IDC_LIST1);
		SendMessage(hWndList,LB_ADDSTRING,0,(LPARAM)".obj");

		return 1;  // Let the system set the focus
	}

	LRESULT OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		EndDialog(wID);
		return 0;
	}

	LRESULT OnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		EndDialog(wID);
		return 0;
	}
};


