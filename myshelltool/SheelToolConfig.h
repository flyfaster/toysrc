// SheelToolConfig.h : Declaration of the CSheelToolConfig

#pragma once

#include "resource.h"       // main symbols
#include <atlhost.h>

extern LPCTSTR REG_APPLICATION_ROOT;
extern DWORD use_unicode ;
extern DWORD enabled ;
extern DWORD keep_build_script;
extern DWORD enable_dbg_output;
extern DWORD show_build_console;
extern TCHAR gcc_path_buf[MAX_PATH + 1024];
// CSheelToolConfig

class CSheelToolConfig : 
	public CAxDialogImpl<CSheelToolConfig>
{
public:
	CSheelToolConfig()
	{
	}

	~CSheelToolConfig()
	{
	}

	enum { IDD = IDD_SHEELTOOLCONFIG };

BEGIN_MSG_MAP(CSheelToolConfig)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	COMMAND_HANDLER(IDOK, BN_CLICKED, OnClickedOK)
	COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnClickedCancel)
	MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
	COMMAND_HANDLER(IDB_GCC_PATH, BN_CLICKED, OnBnClickedGccPath)
	CHAIN_MSG_MAP(CAxDialogImpl<CSheelToolConfig>)
END_MSG_MAP()

// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	void init_check_box(int control_id, DWORD value)
	{
		HWND hWndEnable = GetDlgItem( control_id);
		if(value)
			SendMessage(hWndEnable,BM_SETCHECK,BST_CHECKED,0);
		else
			SendMessage(hWndEnable, BM_SETCHECK, BST_UNCHECKED, 0);
	}
	DWORD get_check_box(int control_id)
	{
		HWND hWndEnable = GetDlgItem( control_id);
		return SendMessage(hWndEnable,BM_GETCHECK,0,0);
	}
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CAxDialogImpl<CSheelToolConfig>::OnInitDialog(uMsg, wParam, lParam, bHandled);
		CRegKey reg;
		LONG lret = reg.Open(HKEY_LOCAL_MACHINE,REG_APPLICATION_ROOT,KEY_READ);
		if(ERROR_SUCCESS == lret)
		{
			reg.QueryDWORDValue(_T("Enabled"),enabled);
			reg.QueryDWORDValue(_T("UseUnicode"), use_unicode);
			reg.QueryDWORDValue(_T("KeepBuildScript"),keep_build_script);
			reg.QueryDWORDValue(_T("EnableTraceOutput"),enable_dbg_output);
			reg.QueryDWORDValue(_T("ShowBuildConsole"), show_build_console);
			gcc_path_buf[0] = 0;
			ULONG size = sizeof(gcc_path_buf)/sizeof(gcc_path_buf[0]);
			reg.QueryStringValue(_T("GccPath"), gcc_path_buf, &size);
		}
		reg.Close();
		init_check_box(IDC_ENABLE, enabled);
		init_check_box(IDC_USE_UNICODE, use_unicode);	
		init_check_box(IDC_ENABLE_DBG_OUTPUT,enable_dbg_output);
		init_check_box(IDC_KEEP_BUILD_SCRIPT, keep_build_script);
		init_check_box(IDC_SHOW_BUILD_CONSOLE, show_build_console);
		HWND gcc_input = GetDlgItem(IDE_GCC_PATH);
		SetDlgItemText(IDE_GCC_PATH, gcc_path_buf);
		return 1;  // Let the system set the focus
	}

	LRESULT OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		CRegKey reg;
		LONG    lRet;
		lRet = reg.Open(HKEY_LOCAL_MACHINE,
			REG_APPLICATION_ROOT,
			KEY_SET_VALUE);
		if (ERROR_SUCCESS != lRet)
			return E_ACCESSDENIED;
		enabled = get_check_box(IDC_ENABLE);
		use_unicode = get_check_box(IDC_USE_UNICODE);
		keep_build_script = get_check_box(IDC_KEEP_BUILD_SCRIPT);
		enable_dbg_output = get_check_box(IDC_ENABLE_DBG_OUTPUT);
		show_build_console = get_check_box(IDC_SHOW_BUILD_CONSOLE);
		reg.SetDWORDValue(_T("Enabled"),enabled);
		reg.SetDWORDValue(_T("UseUnicode"), use_unicode);
		reg.SetDWORDValue(_T("KeepBuildScript"),keep_build_script);
		reg.SetDWORDValue(_T("EnableTraceOutput"),enable_dbg_output);
		reg.SetDWORDValue(_T("ShowBuildConsole"), show_build_console);
		GetDlgItemText(IDE_GCC_PATH, gcc_path_buf, sizeof(gcc_path_buf)/sizeof(gcc_path_buf[0]));
		reg.SetStringValue(_T("GccPath"), gcc_path_buf);
		reg.Close();
		EndDialog(wID);
		return 0;
	}

	LRESULT OnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		EndDialog(wID);
		return 0;
	}
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedGccPath(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};


