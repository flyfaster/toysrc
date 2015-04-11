// greenroadDlg.cpp : implementation file
//

#include "stdafx.h"
#include "greenroad.h"
#include "greenroadDlg.h"
#include "rijndael.h"
#include <MEMORY>
#include "MD5Checksum.h"
#include "SystemTray.h"
#include <comdef.h>
#include "sha.h"
#include ".\greenroaddlg.h"
#pragma comment(lib,"cryptlib.lib")
#include <deque>
#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
#include <io.h>
#include <direct.h>
#include <sstream>
#include <strsafe.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
char default_key[]="Enc+20040723hide";

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGreenroadDlg dialog

CGreenroadDlg::CGreenroadDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGreenroadDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGreenroadDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGreenroadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGreenroadDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_FILE_LIST, m_lv_files);
}

BEGIN_MESSAGE_MAP(CGreenroadDlg, CDialog)
	//{{AFX_MSG_MAP(CGreenroadDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDB_DO, OnDo)
	ON_BN_CLICKED(IDB_UNDO, OnUndo)
	ON_BN_CLICKED(IDB_CHOOSE_SOURCE, OnChooseSource)
	ON_BN_CLICKED(IDB_SET_DESTINATION, OnSetDestination)
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDB_GET_MD5, OnGetMd5)
	ON_WM_DESTROY()
	ON_WM_DROPFILES()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_USE_PLAIN_KEY, OnBnClickedUsePlainKey)
	ON_BN_CLICKED(IDB_CLEAR, OnBnClickedClear)
	ON_BN_CLICKED(IDB_DELETE, OnBnClickedDelete)
	ON_BN_CLICKED(IDB_COPY, OnBnClickedCopy)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGreenroadDlg message handlers
      LRESULT FAR PASCAL MyDragDropProc(HWND, unsigned, WPARAM, LONG);

      FARPROC lpfnDragDropProc, lpfnOldEditProc;

BOOL CGreenroadDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	TCHAR buf[PATH_LEN];
	buf[0] = _T('\0');
	int buf_length = sizeof(buf)/sizeof(buf[0]);
	DWORD dwret = GetModuleFileName(NULL,buf, buf_length);
	std::basic_string<TCHAR> file_name;
	std::basic_string<TCHAR> application_path =buf;
	std::basic_string<TCHAR>::size_type find_position = application_path.rfind(_T('\\'));
	if(find_position != std::basic_string<TCHAR>::npos)
	{
		file_name = application_path.substr(find_position+1);
	}
	std::auto_ptr<CSystemTray> ptmp( new CSystemTray);
	m_pTrayIcon= ptmp;
	int nTrayNotificationMsg_=(RegisterWindowMessage(__TFUNCTION__));		

	m_pTrayIcon->Create(0, nTrayNotificationMsg_,file_name.c_str(),m_hIcon, 0);
	m_pTrayIcon->SetMainWnd(this);
	DragAcceptFiles(TRUE);
	CButton *pb_use_plain_key = (CButton*)GetDlgItem(IDC_USE_PLAIN_KEY);
	pb_use_plain_key->SetCheck(BST_UNCHECKED);

	OnBnClickedUsePlainKey();

    // subclass the drag-drop edit control
	lpfnDragDropProc = (FARPROC)MakeProcInstance(MyDragDropProc, hInst);
	HWND    hWndTemp = GetDlgItem(IDE_SOURCE_FILE)->m_hWnd;
    if (lpfnDragDropProc)
            lpfnOldEditProc = (FARPROC)SetWindowLong(hWndTemp, GWL_WNDPROC,(LONG)lpfnDragDropProc);
	::DragAcceptFiles(hWndTemp, TRUE);
	hWndTemp = GetDlgItem(IDE_RESULT_FILE)->m_hWnd;
	if (lpfnDragDropProc)
		lpfnOldEditProc = (FARPROC)SetWindowLong(hWndTemp, GWL_WNDPROC,(LONG)lpfnDragDropProc);
	::DragAcceptFiles(hWndTemp, TRUE);

	const int MIN_COLUMN_WIDTH = 40;
	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	TCHAR field_name_buf[256];
	StringCchCopy(field_name_buf,sizeof(field_name_buf)/sizeof(field_name_buf[0]), _T("PATH") );
	lvColumn.pszText = field_name_buf;
	lvColumn.cx = 500;
	//lvColumn.cx = 8*_tcslen(lvColumn.pszText);
	//if(lvColumn.cx < MIN_COLUMN_WIDTH )
	//	lvColumn.cx = MIN_COLUMN_WIDTH;
	m_lv_files.InsertColumn(0,&lvColumn);
	StringCchCopy(field_name_buf,sizeof(field_name_buf)/sizeof(field_name_buf[0]), _T("MD5") );
	lvColumn.pszText = field_name_buf;
	lvColumn.cx = 8*32/*8*_tcslen(lvColumn.pszText)*/;
	m_lv_files.InsertColumn(1,&lvColumn);
	m_lv_files.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));       // zero out structure
	lf.lfHeight = 14;                      // request a 12-pixel-height font
	lf.lfPitchAndFamily = FIXED_PITCH;
	StringCchCopy(lf.lfFaceName, LF_FACESIZE, _T("·ÂËÎÌå-18030"));//Simsun-18030 ¿¬Ìå-18030
	if(m_GbFont.CreateFontIndirect(&lf))
	{
		m_lv_files.SetFont(&m_GbFont);
	}
	return TRUE;  // return TRUE  unless you set the focus to a control
}
LRESULT FAR PASCAL MyDragDropProc(HWND hWnd, unsigned message,WPARAM wParam, LONG lParam)
{
    switch (message)
    {
	case WM_CREATE:
		DragAcceptFiles(hWnd, TRUE);
		break;
    case WM_DROPFILES:
		{
			int wFilesDropped =0;
			TCHAR    szTemp64[PATH_LEN];
			wFilesDropped=DragQueryFile((HDROP )wParam,-1,NULL,0);
			memset(szTemp64,0,sizeof(szTemp64));
			wFilesDropped = DragQueryFile((HDROP)wParam, 0,(LPTSTR)szTemp64, PATH_LEN);
			SendMessage(hWnd, WM_SETTEXT, 0, (LONG)szTemp64);
		}
		DragFinish((HDROP)wParam);
		break;
    default:
		return CallWindowProc( (WNDPROC)lpfnOldEditProc, hWnd, message,wParam, lParam);
		break;
    }
    return 0L;
}

void CGreenroadDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	if(SC_MINIMIZE==nID) 
	{
		m_pTrayIcon->ToTray();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CGreenroadDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CGreenroadDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
int Encrypt(char *key, int key_len, char *rawbuf, int len, char *destbuf, int destbuflen, int& resultlen, bool bencrypt)
{
	//return 0 if OK, otherwise return the line number of error
	Rijndael rjd_dc;
	Rijndael::KeyLength KeyLen=Rijndael::Key16Bytes;
	switch(key_len) {
	case 16:
		KeyLen=Rijndael::Key16Bytes;
		break;
	case 24:
		KeyLen=Rijndael::Key24Bytes;
		break;
	case 32:
		KeyLen=Rijndael::Key32Bytes;
		break;
	default:
		{
//			std::ostringstream oss;
//			oss<<"Invalid key length:"<<key_len	;
			//throw CExceptionSimple(__FILE__,__LINE__,oss.str().c_str());
		}
		return __LINE__;
	}
	if(bencrypt)
		rjd_dc.init(Rijndael::ECB,Rijndael::Encrypt,(unsigned char*)key,KeyLen);
	else
		rjd_dc.init(Rijndael::ECB,Rijndael::Decrypt,(unsigned char*)key,KeyLen);
	if(bencrypt)
		resultlen=rjd_dc.padEncrypt((const unsigned char*)(rawbuf),len,(unsigned char*)(destbuf),1);
	else
		resultlen=rjd_dc.padDecrypt((const unsigned char*)(rawbuf),len,(unsigned char*)(destbuf),1);
	if(resultlen>0&&resultlen<=destbuflen)
	{
		return 0;
	}
	else
	{
//		char err_msg_buf[256];
//		StringCchPrintf(err_msg_buf,sizeof(err_msg_buf),"Encrypt return invaild result length:%d, destination buffer length:%d",
//			resultlen,destbuflen);
	}
	return __LINE__;
}

void CGreenroadDlg::common_step(char *pkey, size_t key_len,bool bencrypt,LPCTSTR src_path,LPCTSTR dest_path)
{//b = true, encrypt
	//false:decrypt
	DWORD dw_start = GetTickCount();
	if(_tcslen(src_path)<1||_tcslen(dest_path)<1)
	{
		AfxMessageBox(_T("must set source and result file path"));
		return;
	}
	CFile fsrc;
	if(fsrc.Open(src_path,CFile::modeRead))
	{
		ULONGLONG length = fsrc.GetLength();
		if(length>0)
		{
			std::auto_ptr<char> pbuf_src(new char[length]);
			if(pbuf_src.get())
			{
				UINT uread = fsrc.Read(pbuf_src.get(),length);
				if(uread==length)
				{
					std::auto_ptr<char> pbuf_dest(new char[int(length)+32]);
					int result_length =0;
					int ret = Encrypt(pkey,key_len,pbuf_src.get(),length,pbuf_dest.get(),length+32,result_length,bencrypt);
					if(0==ret && result_length>0)
					{

						CFile fdest;
						if(fdest.Open(dest_path,CFile::modeCreate|CFile::modeWrite ) )
						{
							fdest.Write(pbuf_dest.get(),result_length);
							fdest.Close();
							dw_start = GetTickCount()-dw_start;
							//CString msg;
							//msg.Format(_T("succeed, it take %d seconds"),dw_start/1000);
							//AfxMessageBox(msg);
						}
						else
						{
							AfxMessageBox(_T("create result file failed"));
						}
					}
					else
					{
						AfxMessageBox(_T("operation failed"));
					}
				}
				else
				{
					AfxMessageBox(_T("read file error"));
				}
			}
			else
			{
				AfxMessageBox(_T("allocate memory error"));
			}
		}
		else
		{
			AfxMessageBox(_T("file length error"));
		}
	}
	else
	{
		AfxMessageBox(_T("read file error"));
	}
	fsrc.Close();
}




void CGreenroadDlg::OnChooseSource() 
{
	// TODO: Add your control notification handler code here
	CFileDialog open_dlg(TRUE);
	if(IDOK==open_dlg.DoModal())
	{
		CString selected_path = open_dlg.GetPathName();
		SetDlgItemText(IDE_SOURCE_FILE, selected_path);
	}
}
INT CALLBACK BrowseCallbackProc(HWND hwnd, 
                                UINT uMsg,
                                LPARAM lp, 
                                LPARAM pData) 
{
   TCHAR szDir[PATH_LEN];

   switch(uMsg) 
   {
   case BFFM_INITIALIZED: 
      if (GetCurrentDirectory(sizeof(szDir)/sizeof(TCHAR), szDir))
      {
         // WParam is TRUE since you are passing a path.
         // It would be FALSE if you were passing a pidl.
         SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)szDir);
      }
      break;

   case BFFM_SELCHANGED: 
      // Set the status window to the currently selected path.
      if (SHGetPathFromIDList((LPITEMIDLIST) lp ,szDir))
      {
         SendMessage(hwnd,BFFM_SETSTATUSTEXT,0,(LPARAM)szDir);
      }
      break;
   }
   return 0;
}

void CGreenroadDlg::OnSetDestination() 
{
	// TODO: Add your control notification handler code here
   BROWSEINFO bi;
   LPITEMIDLIST pidl = NULL;
   LPMALLOC pMalloc = NULL;

   if (SUCCEEDED(SHGetMalloc(&pMalloc)))
   {
      ZeroMemory(&bi,sizeof(bi));
      bi.hwndOwner = NULL;
      bi.pszDisplayName = 0;
      bi.pidlRoot = 0;
      bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;
      bi.lpfn = BrowseCallbackProc;

	  pidl = SHBrowseForFolder(&bi); 
	  if (pidl) 
	  { 
		  // 
		  // Other code omited 
		  // 
		  TCHAR dest_path[PATH_LEN];
		  dest_path[0] = 0;
		  if(TRUE==SHGetPathFromIDList(pidl,dest_path))
		  {
			  SetDlgItemText(IDE_RESULT_FILE,dest_path);
		  }
		  pMalloc->Free(pidl); 
	  } 
	  pMalloc->Release();
   }
}
	

void CGreenroadDlg::OnMouseMove(UINT nFlags, CPoint point)
{//move the window by draggin
	if(nFlags == MK_LBUTTON)
	{
		::ReleaseCapture();		
		::SendMessage(m_hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
	}
	CDialog::OnMouseMove(nFlags, point);
}


void CGreenroadDlg::OnGetMd5() 
{
	// TODO: Add your control notification handler code here
	TCHAR src_path[PATH_LEN];
	DWORD dw_start = GetTickCount();
	src_path[0] = 0;
	//GetDlgItemText(IDE_SOURCE_FILE,src_path,sizeof(src_path)/sizeof(TCHAR));
	//CString md5 = CMD5Checksum::GetMD5(src_path);
	//SetDlgItemText(IDE_MD5,md5);
	TCHAR md5_buf[64];
	const int MD5_LEN = 32;
	std::deque<std::basic_string<TCHAR> > files;
	int item_count = m_lv_files.GetItemCount();
	CMD5Checksum all_md5;
	for(int i=0;i<item_count;i++)
	{
		memset(md5_buf,0,sizeof(md5_buf));
		m_lv_files.GetItemText(i,1,md5_buf,sizeof(md5_buf)/sizeof(md5_buf[0]));
		memset(src_path,0,sizeof(src_path));
		m_lv_files.GetItemText(i,0,src_path,sizeof(src_path)/sizeof(src_path[0]));
		if(_tcslen(md5_buf)<MD5_LEN)
		{
			files.clear();
			std::deque<std::basic_string<TCHAR> > dirs;			
			if(PathIsDirectory(src_path))
			{
				dirs.push_back(src_path);
				while(dirs.size()>0)
				{
					std::basic_string<TCHAR> dir_name = dirs.front();
					dirs.pop_front();					
					struct _tfinddata_t fdata;
					int iResult = _tchdir (dir_name.c_str());  
					intptr_t iHandle = _tfindfirst (_T("*.*"), &fdata);
					if(-1!=iHandle)
					{
						int iResult =0;
						while (!(iResult == -1))
						{
							iResult = _tfindnext (iHandle, &fdata);
							if(_tcslen(fdata.name)<1 || _tcscmp(fdata.name,_T("."))==0||_tcscmp(fdata.name,_T(".."))==0)
							{
								continue;
							}
							OutputDebugString(fdata.name);
							std::basic_string<TCHAR> full_path = dir_name;
							size_t last_char_pos = full_path.length()-1;
							TCHAR last_char = full_path.at(last_char_pos);
							if(last_char!=_T('\\'))
							{
								full_path += _T("\\");
							}
							full_path += fdata.name;
							if(PathIsDirectory(full_path.c_str()))
							{
								
								dirs.push_back(full_path);
							}
							else
								files.push_back(full_path);
						}
						_findclose (iHandle);
					}

				}
			}
			else
				files.push_back(src_path);
			//here I get all files
			std::sort(files.begin(),files.end());
			std::deque<std::basic_string<TCHAR> >::iterator it = files.begin();
			CMD5Checksum MD5Checksum;
			int nLength = 0;				//number of bytes read from the file
			const int nBufferSize = 4096;	//checksum the file in blocks of 1024 bytes
			BYTE Buffer[nBufferSize];		//buffer for data read from the file
			bool error = false;
			for(it = files.begin();it!=files.end();it++)
			{
				std::basic_string<TCHAR> file_name = *it;
				CFile f;
				if(f.Open(file_name.c_str(),CFile::modeRead))
				{
					while ((nLength = f.Read( Buffer, nBufferSize )) > 0 )
					{
						MD5Checksum.Update( Buffer, nLength );
						all_md5.Update(Buffer,nLength);
					}
					f.Close();
				}
				else
				{
					m_lv_files.SetItemText(i,1,file_name.c_str());
					error = true;
					break;
				}
			}
			if(!error)
			{
				m_lv_files.SetItemText(i,1,MD5Checksum.Final());
			}
		}
	}
	SetDlgItemText(IDE_MD5,all_md5.Final());
}

void CGreenroadDlg::OnDestroy() 
{
	CDialog::OnDestroy();	
	m_pTrayIcon = std::auto_ptr<CSystemTray>(NULL);
//    if (lpfnOldEditProc)
//            SetWindowLong(hWndTemp, GWL_WNDPROC,(LONG)lpfnOldEditProc);

	 if (lpfnDragDropProc)
         FreeProcInstance(lpfnDragDropProc);
}

void CGreenroadDlg::OnDropFiles(HDROP hDropInfo) 
{
	// TODO: Add your message handler code here and/or call default
	POINT pt;
	DragQueryPoint(hDropInfo,&pt);
	CRect rc;
	GetDlgItem(IDE_SOURCE_FILE)->GetClientRect(&rc);
	if(rc.PtInRect(pt))
		return;
	TCHAR drop_path[PATH_LEN];
	int file_count = DragQueryFile(hDropInfo,0xFFFFFFFF,drop_path,sizeof(drop_path)/sizeof(drop_path[0]));
	for(int i=0;i<file_count;i++)
	{
		UINT uret = DragQueryFile(hDropInfo,i,drop_path,sizeof(drop_path)/sizeof(drop_path[0]));
		if(uret>0)
		{
			if(m_files.find(drop_path)==m_files.end())
			{
				m_files[drop_path] = _T("");
				m_lv_files.InsertItem(m_lv_files.GetItemCount(),drop_path);
			}
		}

	}
	//UINT uret = DragQueryFile(hDropInfo,0,drop_path,sizeof(drop_path)/sizeof(drop_path[0]));
	//if(uret>0)
	//	SetDlgItemText(IDE_SOURCE_FILE,drop_path);

	CDialog::OnDropFiles(hDropInfo);
}

bool CGreenroadDlg::read_key()
{
	memset(m_key_buf,0,sizeof(m_key_buf));
	TCHAR key_buf[32];
	GetDlgItemText(IDE_KEY,key_buf,sizeof(key_buf)/sizeof(key_buf[0]));
	SHORT right_control_state = GetAsyncKeyState(VK_RCONTROL);
	int key_str_len = _tcslen(key_buf);
	if(key_str_len<1)
	{
		if(0x8000&right_control_state)
		{
			m_key_len = strlen(default_key);
			memcpy(m_key_buf,default_key,m_key_len);
			return true;
		}
		else
		{
			AfxMessageBox(_T("must set key"));
		}
	}
	else
	{
		_bstr_t bs_key = key_buf;
		LPCSTR pmbcs_key = (LPCSTR)bs_key;
		key_str_len = strlen(pmbcs_key);
		CButton *pb_use_plain_key = (CButton*)GetDlgItem(IDC_USE_PLAIN_KEY);
		if( BST_CHECKED==pb_use_plain_key->GetCheck())
		{
			if(key_str_len>32)
			{
				CButton *pb_ignore_extra = (CButton*)GetDlgItem(IDB_IGNORE_EXTRA);
				if(pb_ignore_extra->GetCheck()==BST_CHECKED)
				{
					key_str_len = 32;
				}
				else
				{
					AfxMessageBox(_T("key is too long"));
					return false;
				}
			}
			
			{
				memcpy(m_key_buf,pmbcs_key,key_str_len);
				if(key_str_len>24)
					m_key_len = 32;
				else
					if(key_str_len>16&&key_str_len<=24)
						m_key_len = 24;
					else
						m_key_len = 16;
				return true;
			}
		}
		else
		{
			CryptoPP::SHA256 md;
			CryptoPP::SecByteBlock digest(md.DigestSize());
			md.Update( (const byte*)pmbcs_key, key_str_len);
			md.Final(digest);
			memcpy(m_key_buf,digest,md.DigestSize());
			m_key_len = 32;
			return true;
		}
	}
	return false;
}
void CGreenroadDlg::OnBnClickedUsePlainKey()
{
		CButton *pb_use_plain_key = (CButton*)GetDlgItem(IDC_USE_PLAIN_KEY);
		CButton *pb_ignore_extra = (CButton*)GetDlgItem(IDB_IGNORE_EXTRA);
		pb_ignore_extra->EnableWindow(BST_CHECKED==pb_use_plain_key->GetCheck());
}

void CGreenroadDlg::OnBnClickedClear()
{
	m_files.clear();
	m_lv_files.DeleteAllItems();
}

void CGreenroadDlg::OnBnClickedDelete()
{
	// delete selected files from listcontrol
	int item_count = m_lv_files.GetItemCount();
	TCHAR path_buf[PATH_LEN];
	for(int i=item_count-1;i>=0;i--)
	{
		if(LVIS_SELECTED==m_lv_files.GetItemState(i,LVIS_SELECTED))
		{
			memset(path_buf,0,sizeof(path_buf));
			m_lv_files.GetItemText(i,0,path_buf, sizeof(path_buf)/sizeof(path_buf[0]));
			m_lv_files.DeleteItem(i);
			std::map<std::basic_string<TCHAR>, std::basic_string<TCHAR> >::iterator it = m_files.find(path_buf);
			if(it!=m_files.end())
			{
				m_files.erase(it);
			}
		}
	}
}

void CGreenroadDlg::OnBnClickedCopy()
{
	// copy file name and md5 to clipboard, if select count is none, copy all to clipboard
	int item_count = m_lv_files.GetItemCount();
	TCHAR path_buf[PATH_LEN];
	TCHAR md5_buf[64];
	bool selection_exist = false;
	for(int i=0;i<item_count;i++)
	{
		if(LVIS_SELECTED==m_lv_files.GetItemState(i,LVIS_SELECTED))
		{
			selection_exist = true;
			break;
		}
	}
	std::basic_stringstream<TCHAR> oss;
	for(int i=item_count-1;i>=0;i--)
	{
		memset(path_buf,0,sizeof(path_buf));
		m_lv_files.GetItemText(i,0,path_buf, sizeof(path_buf)/sizeof(path_buf[0]));
		memset(md5_buf,0,sizeof(md5_buf));
		m_lv_files.GetItemText(i,1,md5_buf, sizeof(md5_buf)/sizeof(md5_buf[0]));
		if(selection_exist)
		{
			if(LVIS_SELECTED==m_lv_files.GetItemState(i,LVIS_SELECTED) )
			{
				oss<<path_buf<<_T("    ")<<md5_buf<</*std::endl*/ TCHAR(0xD)<<TCHAR(0xA);
			}
		}
		else
		{
			oss<<path_buf<<_T("    ")<<md5_buf<</*std::endl*/  TCHAR(0xD)<<TCHAR(0xA);
		}
	}
	if(OpenClipboard())
	{
		HGLOBAL clipbuffer;
		TCHAR * buffer;
		EmptyClipboard();
		int buf_len = (oss.str().length()+1)*sizeof(TCHAR);
		clipbuffer = GlobalAlloc(GMEM_DDESHARE,buf_len );
		buffer = (TCHAR*)GlobalLock(clipbuffer);
		StringCchCopy(buffer,buf_len/sizeof(TCHAR), LPCTSTR(oss.str().c_str()));
		GlobalUnlock(clipbuffer);
#ifdef _UNICODE
		SetClipboardData(CF_UNICODETEXT,clipbuffer);
#else
		SetClipboardData(CF_TEXT,clipbuffer);
#endif
		CloseClipboard();
	}

}
