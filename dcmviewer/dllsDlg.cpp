// dllsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "dcmviewer.h"
#include "dllsDlg.h"
#include ".\dllsdlg.h"
#include "dllversion.h"
#include "comdef.h"

// CdllsDlg dialog

IMPLEMENT_DYNAMIC(CdllsDlg, CResizableDialog)
CdllsDlg::CdllsDlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CdllsDlg::IDD, pParent)
{
}

CdllsDlg::~CdllsDlg()
{
}

void CdllsDlg::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CdllsDlg, CResizableDialog)
	ON_BN_CLICKED(IDB_SAVE, OnBnClickedSave)
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// CdllsDlg message handlers

BOOL CdllsDlg::OnInitDialog()
{
	__super::OnInitDialog();
	//AddAnchor(IDC_APP_ICON,TOP_LEFT);
	//AddAnchor(IDC_STATIC_VER,TOP_LEFT);
	//AddAnchor(IDC_STATIC_COPY,TOP_LEFT);
	//AddAnchor(IDB_SAVE,TOP_RIGHT);
	//AddAnchor(IDOK,TOP_RIGHT);
	AddAnchor(IDB_SAVE,TOP_LEFT);
	AddAnchor(IDCANCEL,TOP_RIGHT);
	AddAnchor(IDC_LVW,TOP_LEFT , BOTTOM_RIGHT);
	// TODO:  Add extra initialization here
	HWND hwndListView =GetDlgItem(IDC_LVW)->m_hWnd;
	DWORD dwStyle = ListView_GetExtendedListViewStyle(hwndListView);
	dwStyle |=  LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES 
		| LVS_EX_HEADERDRAGDROP ;
	ListView_SetExtendedListViewStyle(hwndListView, dwStyle);

	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 300;
	lvColumn.pszText = _T("Module name");
	ListView_InsertColumn (hwndListView, 0, &lvColumn);
	lvColumn.pszText = _T("version");
	RECT rc;
	::GetWindowRect(hwndListView,&rc);
	lvColumn.cx = 200/*(rc.right-rc.left)-300-20*/;	
	ListView_InsertColumn (hwndListView, 1, &lvColumn);

	HANDLE pProcessHandle = NULL;
	pProcessHandle = GetCurrentProcess();
	HMODULE hInstLib = LoadLibrary( _T("PSAPI.DLL") ) ;

	if (pProcessHandle != NULL&&hInstLib!=NULL) 
	{
		HMODULE pLoadedModules[4096];
		DWORD nLoadedBytes = 0;
		// Get array of loaded modules
		BOOL (WINAPI *lpfEnumProcessModules)( HANDLE, HMODULE *,
			DWORD, LPDWORD );
		DWORD (WINAPI *lpfGetModuleFileNameEx)( HANDLE, HMODULE,
			LPTSTR, DWORD );
		lpfEnumProcessModules = (BOOL(WINAPI *)(HANDLE, HMODULE *,
			DWORD, LPDWORD)) GetProcAddress( hInstLib,
			"EnumProcessModules" ) ;
		lpfGetModuleFileNameEx =(DWORD (WINAPI *)(HANDLE, HMODULE,
			LPTSTR, DWORD )) GetProcAddress( hInstLib,
#ifdef _UNICODE
			"GetModuleFileNameExW" 
#else
			"GetModuleFileNameExA" 
#endif
			) ;

		if (lpfEnumProcessModules(pProcessHandle,
			pLoadedModules,
			(DWORD)sizeof(pLoadedModules),
			&nLoadedBytes)) {
				// Compute effective number of modules
				int nNumberOfModules = nLoadedBytes/sizeof(HMODULE);
				// Loop on all modules
				for (int i=0; i<nNumberOfModules; i++) {
					// Fetch module file name
					TCHAR pFileName[_MAX_PATH];
					pFileName[0]=0;
					CString cleanFileName;
					if (lpfGetModuleFileNameEx(pProcessHandle,
						pLoadedModules[i],
						pFileName,
						_MAX_PATH) > 0) 
					{
						if(_tcslen(pFileName)>0&&_tcslen(pFileName)<ARRAY_SIZE(pFileName))
						{
							CDLLVersion dll_ver(pFileName);
							LVITEM lvI;
							lvI.mask = LVIF_TEXT ; 
							lvI.state = 0; 
							lvI.stateMask = 0; 						
							lvI.iItem = i;
							lvI.iSubItem = 0;
							lvI.pszText = (pFileName);
							ListView_InsertItem(hwndListView, &lvI);
							LV_ITEM lv_sub;
							memset(&lv_sub,0,sizeof(LV_ITEM));
							lv_sub.mask = LVIF_TEXT ; 
							lv_sub.state = 0; 
							lv_sub.stateMask = 0; 
							lv_sub.iItem = i;
							lv_sub.iSubItem = 1;	
							lv_sub.pszText = (LPTSTR)(LPCTSTR)dll_ver.GetFullVersion();
							ListView_SetItem(hwndListView, &lv_sub);
						}
					}
				}
			}
			// Close process handle
			CloseHandle(pProcessHandle);

	} 
	FreeLibrary(hInstLib);
	return TRUE;  // return TRUE unless you set the focus to a control	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CdllsDlg::OnBnClickedSave()
{
	TCHAR app_path_buf[PATH_LEN];
	memset(app_path_buf,0,sizeof(app_path_buf));
	GetModuleFileName(NULL, app_path_buf,sizeof(app_path_buf)/sizeof(app_path_buf[0]));
	int pathlen = _tcslen(app_path_buf);
	int i = pathlen-1;
	while(i>0)
	{
		if(app_path_buf[i]==_T('\\') )
		{
			app_path_buf[i+1] = 0;
			break;
		}
		i--;
	}

	TCHAR szFilters[] =_T("Text Files (*.txt)\0*.txt\0")
		_T("All Files (*.*)\0*.*\0");
	CFileDialog fdlg (FALSE, NULL, NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | 
		OFN_PATHMUSTEXIST |OFN_EXPLORER, szFilters, AfxGetMainWnd());
	//CFileDialog ofdlg(FALSE);
	fdlg.m_ofn.lpstrFilter = szFilters;
	//CFileDialog fdlg(FALSE,_T(".txt"),_T("dlllist.txt"));
	fdlg.m_ofn.lpstrInitialDir =app_path_buf;
	if(IDOK!=fdlg.DoModal())
		return;	
	std::basic_string<TCHAR> savepathname = fdlg.m_ofn.lpstrFile;
	if(1==fdlg.m_ofn.nFilterIndex && !end_with(savepathname,_T(".txt"),true) )
	{
		savepathname += _T(".txt");
	}
	std::basic_fstream<TCHAR> ofile;
	ofile.open( (LPCSTR)_bstr_t(savepathname.c_str()),std::ios_base::out|std::ios_base::trunc);
	HWND hwndListView =GetDlgItem(IDC_LVW)->m_hWnd;
	int rows = 0;
	rows = ListView_GetItemCount(hwndListView);
	int cols = 0;
	HWND headerwnd = ListView_GetHeader(hwndListView);
	cols = Header_GetItemCount(headerwnd);
	for(int i=0;i<rows;i++)
	{
		for(int j=0;j<cols;j++)
		{
			app_path_buf[0] = 0;
			ListView_GetItemText(hwndListView,i,j,app_path_buf,ARRAY_SIZE(app_path_buf));
			if(j<cols-1)
				ofile<<app_path_buf<<_T("    ");
			else
			{
				ofile<<app_path_buf<<(char)0xd<<(char)0xa;
			}
		}
	}
	ofile.close();
}

void CdllsDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// move the dialog by drag somewhere in the dialog
	if(nFlags == MK_LBUTTON)
	{
		::ReleaseCapture();		
		::SendMessage(m_hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
	}
	__super::OnMouseMove(nFlags,point);
}
