// dcmviewer.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "dcmviewer.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "dcmviewerDoc.h"
#include "dcmviewerView.h"
#include <afxpriv.h>
#include ".\dcmviewer.h"
#include "lut_creator.h"
#include "HistogramView.h"
#include "storescp.h"
#include <atlbase.h>
#include "dcmviewer_const.h"
#include "SearchView.h"
#include "TLFrame.h"
#include "ResizableDialog.h"
#pragma comment(lib,"resizablelib.lib")
#include "ScrollEditView.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

LPCTSTR DCMVIEWER_REG_PATH = _T("SOFTWARE\\www.fruitfruit.com\\dcmviewer");
LPCTSTR ADDRESS_BOOK_PATH = _T("SOFTWARE\\www.fruitfruit.com\\dcmviewer\\addressbook"); //don't end with \
// CdcmviewerApp

BEGIN_MESSAGE_MAP(CdcmviewerApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// Standard file based document commands
	//ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_VIEW_BACKGROUND_COLOR, OnViewBackgroundColor)
	ON_COMMAND(ID_VIEW_FONT, OnViewFont)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
END_MESSAGE_MAP()


// CdcmviewerApp construction

CdcmviewerApp::CdcmviewerApp()
{
	m_pEditDocTemplate = NULL;
	m_pImageDocTemplate = NULL;
	m_pHistDocTemplate = NULL;
	m_pSearchDocTemplate = NULL;
	m_background_color = RGB(0,0,0);
	m_font_color = RGB(0,255,255);
	m_plogfont = NULL;
	m_pfont.reset(NULL);
	m_brush_background.reset(new CBrush);
	m_brush_background->CreateSolidBrush(m_background_color);
	m_dicom_server = boost::shared_ptr<dicom::Server>(new dicom::Server);
}


// The one and only CdcmviewerApp object

CdcmviewerApp theApp;

// CdcmviewerApp initialization
HWND g_main_wnd;
BOOL CdcmviewerApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();
	//output_self_digest();
	//verify_exe();
	CWinApp::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	CTLFrame::RegisterClass();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	m_pImageDocTemplate = new CMultiDocTemplate(IDR_dcmviewerTYPE,
		RUNTIME_CLASS(CdcmviewerDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CdcmviewerView));
	if (!m_pImageDocTemplate)
		return FALSE;
	AddDocTemplate(m_pImageDocTemplate);
	 m_pEditDocTemplate = new CMultiDocTemplate(
		IDR_dcmviewerTYPE,
		RUNTIME_CLASS(CdcmviewerDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CEditView));
	AddDocTemplate(m_pEditDocTemplate);

	 m_pHistDocTemplate = new CMultiDocTemplate(
		IDR_dcmviewerTYPE,
		RUNTIME_CLASS(CdcmviewerDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(HistogramView));
	AddDocTemplate(m_pHistDocTemplate);
	m_pSearchDocTemplate = new CMultiDocTemplate(
		IDR_dcmviewerTYPE,
		RUNTIME_CLASS(CdcmviewerDoc),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(SearchView));
	AddDocTemplate(m_pSearchDocTemplate);
	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;
	g_main_wnd = m_pMainWnd->m_hWnd;


	// call DragAcceptFiles only if there's a suffix
	//  In an MDI app, this should occur immediately after setting m_pMainWnd
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	cmdInfo.m_nShellCommand=CCommandLineInfo::FileNothing ; 
	ParseCommandLine(cmdInfo);
	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// The main window has been initialized, so show and update it
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();
	setup_dicom_server();
	return TRUE;
}

void CdcmviewerApp::setup_dicom_server()
{
	//setup a dicom store scp
	m_dicom_server->AddHandler( dicom::UID(DICOM_SOPRTIMAGESTORAGE)	    		,StoreSCP); 
	m_dicom_server->AddHandler( dicom::UID(DICOM_SOPRTDOSESTORAGE)	    		,StoreSCP); 
	m_dicom_server->AddHandler( dicom::UID(DICOM_SOPRTSTRUCTURESETSTORAGE)	    		,StoreSCP); 
	m_dicom_server->AddHandler( dicom::UID(DICOM_SOPRTBREAMS)	    		,StoreSCP); 
	m_dicom_server->AddHandler( dicom::UID(DICOM_SOPRTPLANSTORAGE)	    		,StoreSCP); 
	m_dicom_server->AddHandler( dicom::UID(DICOM_SOPRTBRACHYTREATMENT)	    		,StoreSCP); 
	m_dicom_server->AddHandler( dicom::UID(DICOM_SOPRTTREATMENTSUMMARY)	    		,StoreSCP); 

	m_dicom_server->AddHandler( dicom::CR_IMAGE_STORAGE_SOP_CLASS	    		,StoreSCP); //("1.2.840.10008.5.1.4.1.1.1");
	m_dicom_server->AddHandler( dicom::CT_IMAGE_STORAGE_SOP_CLASS	    		,StoreSCP); //("1.2.840.10008.5.1.4.1.1.2");
	m_dicom_server->AddHandler( dicom::USOLD_MF_IMAGE_STORAGE_SOP_CLASS  		,StoreSCP); //("1.2.840.10008.5.1.4.1.1.3");
	m_dicom_server->AddHandler( dicom::US_MF_IMAGE_STORAGE_SOP_CLASS  			,StoreSCP); //("1.2.840.10008.5.1.4.1.1.3.1");
	m_dicom_server->AddHandler( dicom::MR_IMAGE_STORAGE_SOP_CLASS	    		,StoreSCP); //("1.2.840.10008.5.1.4.1.1.4");
	m_dicom_server->AddHandler( dicom::USOLD_IMAGE_STORAGE_SOP_CLASS				,StoreSCP); //("1.2.840.10008.5.1.4.1.1.6");
	m_dicom_server->AddHandler( dicom::US_IMAGE_STORAGE_SOP_CLASS			    ,StoreSCP); //("1.2.840.10008.5.1.4.1.1.6.1");
	m_dicom_server->AddHandler( dicom::SC_IMAGE_STORAGE_SOP_CLASS			    ,StoreSCP); //("1.2.840.10008.5.1.4.1.1.7");
	m_dicom_server->AddHandler( dicom::NM_IMAGE_STORAGE_SOP_CLASS		    	,StoreSCP); //("1.2.840.10008.5.1.4.1.1.20");
	m_dicom_server->AddHandler( dicom::XA_IMAGE_STORAGE_SOP_CLASS			    ,StoreSCP); //("1.2.840.10008.5.1.4.1.1.12.1");
	m_dicom_server->AddHandler( dicom::XRF_IMAGE_STORAGE_SOP_CLASS				,StoreSCP); //("1.2.840.10008.5.1.4.1.1.12.2");
	m_dicom_server->AddHandler( dicom::XA2_IMAGE_STORAGE_SOP_CLASS				,StoreSCP); //("1.2.840.10008.5.1.4.1.1.12.3");
	m_dicom_server->AddHandler( dicom::PET_IMAGE_STORAGE_SOP_CLASS				,StoreSCP); //("1.2.840.10008.5.1.4.1.1.128");
	m_dicom_server->AddHandler( dicom::DX_PRES_IMAGE_STORAGE_SOP_CLASS			,StoreSCP); //("1.2.840.10008.5.1.4.1.1.1.1");
	m_dicom_server->AddHandler( dicom::DX_PROC_IMAGE_STORAGE_SOP_CLASS			,StoreSCP); //("1.2.840.10008.5.1.4.1.1.1.1.1");
	m_dicom_server->AddHandler( dicom::MAMMO_PRES_IMAGE_STORAGE_SOP_CLASS		,StoreSCP); //("1.2.840.10008.5.1.4.1.1.1.2");
	m_dicom_server->AddHandler( dicom::MAMMO_PROC_IMAGE_STORAGE_SOP_CLASS		,StoreSCP); //("1.2.840.10008.5.1.4.1.1.1.2.1");
	m_dicom_server->AddHandler( dicom::BASIC_TEXT_SR_STORAGE_SOP_CLASS			,StoreSCP); //("1.2.840.10008.5.1.4.1.1.88.11");
	m_dicom_server->AddHandler( dicom::ENHANCED_SR_STORAGE_SOP_CLASS				,StoreSCP); //("1.2.840.10008.5.1.4.1.1.88.22");
	m_dicom_server->AddHandler( dicom::COMPREHENSIVE_SR_STORAGE_SOP_CLASS		,StoreSCP); //("1.2.840.10008.5.1.4.1.1.88.33");
	m_dicom_server->AddHandler( dicom::INTRAORAL_PRES_IMAGE_STORAGE_SOP_CLASS	,StoreSCP); //("1.2.840.10008.5.1.4.1.1.1.3");
	m_dicom_server->AddHandler( dicom::INTRAORAL_PROC_IMAGE_STORAGE_SOP_CLASS	,StoreSCP); //("1.2.840.10008.5.1.4.1.1.1.3.1");
	m_dicom_server->SetCheckLocalAETCallback(check_ae_callback);
	m_dicom_server->SetCheckRemoteAETCallback(check_ae_callback);
	UINT port = get_server_port();
	try
	{
		m_dicom_server->ServeInNewThread(port);
	}
	catch (std::exception &e)
	{
		AfxMessageBox(e.what());
	}	
}

UINT CdcmviewerApp::get_server_port()
{
	//get dicom port from registry
	LPCTSTR local_port_value_name = _T("local_port");
	CRegKey reg;
	DWORD port = default_local_port;
	TCHAR data[32];
	DWORD buflen = sizeof(data)/sizeof(data[0]);
	itoa(port,data,10);
	if(ERROR_SUCCESS == reg.Open(HKEY_LOCAL_MACHINE,DCMVIEWER_REG_PATH,KEY_READ|KEY_WRITE))
	{
		if(ERROR_SUCCESS != reg.QueryStringValue(local_port_value_name,data,&buflen))
		{
			port = default_local_port;
			reg.SetStringValue(local_port_value_name,data);
		}
		else
		{
			port = atoi(data);
		}
	}
	else
	{
		if(ERROR_SUCCESS==reg.Create(HKEY_LOCAL_MACHINE,DCMVIEWER_REG_PATH,REG_NONE,REG_OPTION_NON_VOLATILE,KEY_WRITE))
		{
			reg.SetStringValue(local_port_value_name,data);
		}
	}
	reg.Close();
	return port;
}

// CAboutDlg dialog used for App About

class CAboutDlg : public CResizableDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};

CAboutDlg::CAboutDlg() : CResizableDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CResizableDialog)
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// App command to run the dialog
void CdcmviewerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CdcmviewerApp message handlers
CView* CdcmviewerApp::SwitchView(CView* pShowView)
{
	CView* pActiveView =
		((CFrameWnd*) m_pMainWnd)->GetActiveView();
	CView* pNewView= pShowView;
	// Exchange view window IDs so RecalcLayout() works.
#ifndef _WIN32
	UINT temp = ::GetWindowWord(pActiveView->m_hWnd, GWW_ID);
	::SetWindowWord(pActiveView->m_hWnd, GWW_ID, ::GetWindowWord(pNewView->m_hWnd, GWW_ID));
	::SetWindowWord(pNewView->m_hWnd, GWW_ID, temp);
#else
	UINT temp = ::GetWindowLong(pActiveView->m_hWnd, GWL_ID);
	::SetWindowLong(pActiveView->m_hWnd, GWL_ID, ::GetWindowLong(pNewView->m_hWnd, GWL_ID));
	::SetWindowLong(pNewView->m_hWnd, GWL_ID, temp);
#endif

	pActiveView->ShowWindow(SW_HIDE);
	pNewView->ShowWindow(SW_SHOW);
	((CFrameWnd*) m_pMainWnd)->SetActiveView(pNewView);
	((CFrameWnd*) m_pMainWnd)->RecalcLayout();
	pNewView->Invalidate();
	return pActiveView;
}
void CdcmviewerApp::CreateView(CDocument *pCurrentDoc,CView *pView)
{
	//CDocument* pCurrentDoc = ((CFrameWnd*)m_pMainWnd)->GetActiveDocument();
	if(m_pEdit)
		return;
	// Initialize a CCreateContext to point to the active document.
	// With this context, the new view is added to the document
	// when the view is created in CView::OnCreate().
	CCreateContext newContext;
	newContext.m_pNewViewClass = NULL;
	newContext.m_pNewDocTemplate = NULL;
	newContext.m_pLastView = NULL;
	newContext.m_pCurrentFrame = NULL;
	newContext.m_pCurrentDoc = pCurrentDoc;
	// The ID of the initial active view is AFX_IDW_PANE_FIRST.
	// Incrementing this value by one for additional views works
	// in the standard document/view case but the technique cannot
	// be extended for the CSplitterWnd case.
	UINT viewID = AFX_IDW_PANE_FIRST + 1000;
	CRect rect(0, 0, 0, 0); // Gets resized later.
	// Create the new view. In this example, the view persists for
	// the life of the application. The application automatically
	// deletes the view when the application is closed.
	m_pEdit = new CEditView;
	m_pEdit->Create(NULL, _T("AnyWindowName"), WS_CHILD, rect, pView->GetParent(), viewID, &newContext);

	// When a document template creates a view, the WM_INITIALUPDATE
	// message is sent automatically. However, this code must
	// explicitly send the message, as follows.

	m_pEdit->SendMessage(WM_INITIALUPDATE, 0, 0);

}
bool end_with(std::basic_string<TCHAR> s,std::basic_string<TCHAR> postfix,bool ignore_case)
{
	if(ignore_case)
	{
		std::transform(s.begin(),s.end(),s.begin(),toupper);
		std::transform(postfix.begin(),postfix.end(),postfix.begin(),toupper);
	}

	size_t postfix_length = postfix.length();
	size_t first_str_pos = s.length()-postfix.length();
	if(s.compare(first_str_pos,postfix_length,postfix)==0)
		return true;
	else
		return false;
}

CDocument* CdcmviewerApp::OpenDocumentFile(LPCTSTR lpszFileName)
{
	CDocument* pDoc = NULL;
	bool bis_text_file = false;
	if(is_text_file(lpszFileName))
	{
		pDoc = m_pEditDocTemplate->OpenDocumentFile(lpszFileName);
		bis_text_file = true;
	}
	else
        pDoc = m_pImageDocTemplate->OpenDocumentFile(lpszFileName);
	CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
	CMDIChildWnd* pMDIActive = pMainFrame->MDIGetActive();
	if(bis_text_file)
	{
		CEditView *pview = (CEditView*)pMDIActive->GetActiveView();
		pMDIActive->SetTitle(lpszFileName);
		CEdit& wndEdit = pview->GetEditCtrl();
		pview->SetTabStops(4);
		wndEdit.ModifyStyle(ES_WANTRETURN,ES_AUTOVSCROLL|ES_MULTILINE|ES_AUTOHSCROLL);
		CFile ftmp;
		if(ftmp.Open(lpszFileName,CFile::modeRead) && ftmp.GetLength()>0)
		{
			std::auto_ptr<char> buf(new char[ftmp.GetLength()+1]);
			if(buf.get())
			{
				ftmp.Read(buf.get(),ftmp.GetLength());
				buf.get()[ftmp.GetLength()] = 0;
				wndEdit.SetWindowText(buf.get());
			}
			else
			{
				wndEdit.SetWindowText(_T("failed to read file"));
			}
		}
		wndEdit.Invalidate();

	}
	if(lpszFileName)
		pMainFrame->MDIMaximize(pMDIActive);
	pMainFrame->MDIActivate(pMDIActive);//activate the view so that WM_KEYDOWN can work
	return pDoc;
	return CWinApp::OpenDocumentFile(lpszFileName);
}

void CdcmviewerApp::OnFileNew()
{
	//m_pImageDocTemplate->OpenDocumentFile(NULL);
	//CMDIFrameWnd *pFrame = (CMDIFrameWnd *)AfxGetMainWnd();
	//pFrame->MDIMaximize(pFrame->MDIGetActive(NULL));
	//m_pEditDocTemplate->OpenDocumentFile(NULL);
	//CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
	//CMDIChildWnd* pMDIActive = pMainFrame->MDIGetActive();
	//CEditView *pview = (CEditView*)pMDIActive->GetActiveView();
	//pMDIActive->GetActiveDocument()->SetTitle(_T("Readme"));
	//pMDIActive->SetTitle(_T("Readme"));
	//CEdit& wndEdit = pview->GetEditCtrl();
	//pview->SetTabStops(4);
	//wndEdit.ModifyStyle(ES_WANTRETURN,ES_AUTOVSCROLL|ES_MULTILINE|ES_AUTOHSCROLL);
	//wndEdit.SetWindowText(get_res_string(IDR_BIN_DESCRIPTION,_T("BIN")).c_str());
	//wndEdit.Invalidate();
	//pMainFrame->MDIMaximize(pMDIActive);
	//pMainFrame->MDIActivate(pMDIActive);//activate the view so that WM_KEYDOWN can work
	m_pSearchDocTemplate->OpenDocumentFile(NULL);
	CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
	CMDIChildWnd* pMDIActive = pMainFrame->MDIGetActive();
	SearchView *pview = (SearchView*)pMDIActive->GetActiveView();
	pMDIActive->GetActiveDocument()->SetTitle(_T("Search..."));
	pMDIActive->SetTitle(_T("Search..."));
	//pMainFrame->MDIMaximize(pMDIActive);
	pMainFrame->MDIActivate(pMDIActive);//activate the view so that WM_KEYDOWN can work

}

BOOL CAboutDlg::OnInitDialog()
{
	CResizableDialog::OnInitDialog();
	AddAnchor(IDOK, TOP_RIGHT);
	AddAnchor(IDC_STATIC_ICON, TOP_LEFT);
	AddAnchor(IDC_STATIC_VER, TOP_LEFT);
	AddAnchor(IDC_STATIC_COPY, TOP_LEFT);
	AddAnchor(IDE_DESCRIPTION, TOP_LEFT, BOTTOM_RIGHT);

	//show description of the program
	std::string read_me = theApp.get_res_string(IDR_BIN_DESCRIPTION,_T("BIN"));
	read_me += std::string(__DATE__);
	SetDlgItemText(IDE_DESCRIPTION,read_me.c_str());
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CAboutDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// move the dialog by drag somewhere in the dialog
	if(nFlags == MK_LBUTTON)
	{
		::ReleaseCapture();		
		::SendMessage(m_hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
	}
	__super::OnMouseMove(nFlags,point);
}
std::string CdcmviewerApp::get_res_string(UINT res_id,LPCTSTR res_type)
{
	HRSRC hResLoad = NULL;     // handle to loaded resource 
	HMODULE hExe = NULL;        // handle to existing .EXE file 
	HRSRC hRes = NULL;         // handle/ptr. to res. info. in hExe 
	LPVOID lpResLock = NULL;    // pointer to resource data 

	hRes = FindResource(hExe, MAKEINTRESOURCE(res_id), res_type); 
	if (hRes == NULL) 
	{ 
		return _T(""); 
	} 
	hResLoad = (HRSRC)LoadResource(hExe, hRes); 
	if (hResLoad == NULL) 
	{ 
		return _T(""); 
	} 
	lpResLock = LockResource(hResLoad); 
	if (lpResLock == NULL) 
	{ 
		return _T(""); 
	} 
	int fileSize = SizeofResource(hExe, hRes);
	std::auto_ptr<char> buf(new char[fileSize+4]);	
	if(buf.get())
	{
		memcpy(buf.get(),lpResLock,fileSize);
		buf.get()[fileSize] = 0;
		return buf.get();
	}
	return _T("");
}
void CdcmviewerApp::OnViewBackgroundColor()
{
	CColorDialog clrdlg;
	if(IDOK==clrdlg.DoModal() /*&& m_background_color!=clrdlg.GetColor()*/)
	{
		m_background_color = clrdlg.GetColor();
		if(m_brush_background.get())
		{
			m_brush_background->DeleteObject();			
		}
		m_brush_background.reset(new CBrush);
		m_brush_background->CreateSolidBrush(m_background_color);
		ui_changed();
	}
}
void CdcmviewerApp::ui_changed()
{
	//redraw all windows
	AfxGetMainWnd()->SendMessage(WM_PAINT);
	POSITION pos = GetFirstDocTemplatePosition();
	while (pos)
	{
		CDocTemplate* pTemplate = (CDocTemplate*)theApp.GetNextDocTemplate(pos);
		POSITION pos2 = pTemplate->GetFirstDocPosition();
		while (pos2)
		{
			CDocument * pDocument;
			if ((pDocument=pTemplate->GetNextDoc(pos2)) != NULL)
			{
				pDocument->UpdateAllViews(NULL);
			}
		}
	}

}
void CdcmviewerApp::OnViewFont()
{
	if(m_plogfont == NULL)
		m_plogfont = new LOGFONT;
	CFontDialog dlg;
	dlg.m_cf.rgbColors = m_font_color;
	if (dlg.DoModal() == IDOK)
	{
		dlg.GetCurrentFont(m_plogfont);
		m_font_color = dlg.GetColor();
		if(m_pfont.get())
			m_pfont->DeleteObject();
		m_pfont.reset(new CFont);
		m_pfont->CreateFontIndirect(m_plogfont);
		ui_changed();
	}

}

#include "multiextfilterfiledialog.h"

void CdcmviewerApp::OnFileOpen()
{
	//if(end_with(lpszFileName,_T(".txt"),true)|| end_with(lpszFileName,_T(".ini"),true)
	//	||end_with(lpszFileName,_T(".h"),true)||end_with(lpszFileName,_T(".cpp"),true)
	//	||end_with(lpszFileName,_T(".xml"),true)||end_with(lpszFileName,_T(".hpp"),true)
	//	||end_with(lpszFileName,_T(".c"),true)||end_with(lpszFileName,_T(".cxx"),true)
	//	||end_with(lpszFileName,_T(".html"),true) )

	TCHAR szFilters[] =
		_T("DICOM image Files(*.dcm)\0*.dcm\0")
		_T("Text Files (*.txt;*.log;*.xml;*.ini;*.html)\0*.txt;*.log;*.xml;*.ini;*.html\0")
		_T("C++ Files (*.cc;*.cpp;*.h;*.hpp)\0*.cc;*.cpp;*.h;*.hpp\0")
		_T("All Files (*.*)\0*.*\0")
		;
	CMultiExtFilterFileDialog ofdlg(TRUE);
	ofdlg.m_ofn.lpstrFilter = szFilters;
	if (ofdlg.DoModal() == IDOK)
	{
		AfxGetApp()->m_pDocManager->OpenDocumentFile(ofdlg.m_ofn.lpstrFile);
		//AfxMessageBox(ofdlg.GetPathName());
	}
}

bool CdcmviewerApp::is_text_file(LPCTSTR lpszFileName)
{
	if(end_with(lpszFileName,_T(".txt"),true)|| end_with(lpszFileName,_T(".ini"),true)
		||end_with(lpszFileName,_T(".h"),true)||end_with(lpszFileName,_T(".cpp"),true)
		||end_with(lpszFileName,_T(".xml"),true)||end_with(lpszFileName,_T(".hpp"),true)
		||end_with(lpszFileName,_T(".c"),true)||end_with(lpszFileName,_T(".cxx"),true)
		||end_with(lpszFileName,_T(".html"),true)||end_with(lpszFileName,_T(".log"),true)
		||end_with(lpszFileName,_T(".cc"),true)
		)
	{
		return true;
	}
	return false;
}

void CdcmviewerApp::DebugOutput(LPCTSTR msg)
{
	std::basic_stringstream<TCHAR> tss;
	tss<<_T("[Thread:")<<GetCurrentThreadId()<<_T("]");
	if(msg)
		tss<<msg;
	OutputDebugString(tss.str().c_str());
}