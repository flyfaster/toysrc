// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "dcmviewer.h"
#include "ChildFrm.h"

#include "MainFrm.h"
//#include ".\mainfrm.h"
#include "dcmviewerdoc.h"
#include "indentdumper.h"
#include "dcmviewer_const.h"
#include ".\mainfrm.h"
#include "configDlg.h"
#include "addressbookdlg.h"
#include "dllsdlg.h"
#include "dcmviewerView.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame
//ON_WM_ERASEBKGND()
//ON_WM_PAINT()
//ON_COMMAND(ID_VIEW_DICOM_HEADER, OnViewDicomHeader)
//ON_UPDATE_COMMAND_UI(ID_VIEW_DICOM_HEADER, OnUpdateViewDicomHeader)

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	ON_WM_CREATE()
	ON_WM_DROPFILES()
	ON_COMMAND(ID_WINDOW_CLOSEALL, OnWindowCloseall)
	ON_WM_PAINT()
	ON_MESSAGE(WM_DICOM_STORE_SCP,OnDicomStoreScp)
	ON_COMMAND(ID_DICOM_CONFIG, OnDicomConfig)
	ON_COMMAND(ID_DICOM_ADDRESS_BOOK, OnDicomAddressBook)
	ON_WM_DESTROY()
	ON_COMMAND(ID_HELP_DEPENDS, OnHelpDepends)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	DragAcceptFiles(TRUE);
	PostMessage(WM_PAINT); // to draw the client window.
	CString msg;
	msg.Format("dcmviewer(listen on port %d)",theApp.get_server_port());
	SetWindowText(msg);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers
///////////////////////////////////////
void CMainFrame::CreateOrActivateFrame(CDocTemplate* pTemplate,CRuntimeClass* pViewClass)
///////////////////////////////////////
// pDocumentTemplate is a CDocTemplate* to the template you wish to use
// which is passed into this function.
// pViewClass is a CRuntimeClass* to the view class desired which is
// passed into the function
{
	// Get the currently active child. 
	CMDIChildWnd* pMDIActive = MDIGetActive();
	ASSERT(pMDIActive != NULL);
	if(pMDIActive == NULL)
		return;
	// now get that child's document... you may not need to do this, but if there may
	// be multiple documents this a way to make sure that the correct set of windows
	// is being acted on.
	CDocument* pDoc = pMDIActive->GetActiveDocument();
	ASSERT(pDoc != NULL); 
	CView* pView;
	POSITION pos = pDoc->GetFirstViewPosition();
	while (pos != NULL)
	{  
		pView=pDoc->GetNextView(pos);
		if(pView->IsKindOf(pViewClass)) 
		{ // We found a view for this			alreadycreated.
			pView->GetParentFrame()->ActivateFrame();
			return;
		}
	}
	// This view doesn't exist for this document, create it
	CMDIChildWnd* pNewFrame = (CMDIChildWnd*)(pTemplate->CreateNewFrame(pDoc, NULL));
	if (pNewFrame ==NULL)
		return; // not created you may want to assert	here 
	ASSERT_KINDOF(CMDIChildWnd,pNewFrame);pTemplate->InitialUpdateFrame(pNewFrame, pDoc);
}



void IndentDumper::operator()(const dicom::DataSet::value_type& v)
{
	char d = 0xD;
	char a = 0xA;//
	const dicom::Tag &tag = v.first;
	std::ostream &oss = Out_;
	if( dicom::GetVR(v.first) == dicom::VR_SQ)
	{
		write_tag(tag,m_indent);
		oss << " sequence begin"<<d<<a;
		const dicom::Sequence& s = v.second.Get<dicom::Sequence>();
		std::for_each(s.begin(),s.end(),IndentDumpDataSet(oss,m_indent+1));
		write_tag(tag,m_indent);
		oss << " sequence end"<<d<<a;
	}
	else
	{
		write_tag(tag,m_indent);
		const dicom::VR vr = v.second.vr();
		if(vr == dicom::VR_OB)
		{
			typedef	std::vector<UINT8>	OB_TYPE;
			const OB_TYPE &binary_data = v.second.Get<OB_TYPE>();
			oss<<"(length:"<< (unsigned int)binary_data.size()<<") ";
			for(size_t i=0;i<16&&i<binary_data.size();i++)
			{
				oss<<std::hex<<std::uppercase <<std::setw(sizeof(UINT8)*2);
				int tmp = binary_data[i];
				tmp = tmp &0xff;
				oss <<tmp;
				oss<<" ";
			}
			oss<<d<<a;
		}
		else if(dicom::VR_OW == vr)
		{
			typedef	std::vector<UINT16>	OW_TYPE;
			const OW_TYPE &binary_data = v.second.Get<OW_TYPE>();
			oss<<"(length:"<< (unsigned int)binary_data.size()<<") ";
			for(size_t i=0;i<16&&i<binary_data.size();i++)
			{
				oss<<std::hex<<std::uppercase << std::setw(sizeof(UINT16)*2)<< binary_data[i]<<" ";
			}
			oss<<d<<a;
		}
		else
			oss<< v.second <<d<<a;
	}
}



void CMainFrame::OnViewDicomHeader()
{
	CreateOrActivateFrame(((CdcmviewerApp*)AfxGetApp())->m_pEditDocTemplate,RUNTIME_CLASS(CEditView)); 
	CChildFrame* pMDIActive = (CChildFrame*)MDIGetActive();
	CEditView *pview = (CEditView*)pMDIActive->GetActiveView();
	CString title;
	title.Format(_T("DicomHeader:%s"),pMDIActive->GetActiveDocument()->GetTitle());
	pMDIActive->SetTitle(title);
	//pMDIActive->m_title = title;

	CdcmviewerDoc *pDoc = static_cast<CdcmviewerDoc*>(pMDIActive->GetActiveDocument());
	std::stringstream oss;
	oss<< (LPCTSTR)pMDIActive->GetActiveDocument()->GetPathName() << char(0xd) << char(0xa);
	IndentDumper::IndentDump(pDoc->m_dataset,oss);
	std::string dcm_header_str = oss.str();
	CEdit& wndEdit = pview->GetEditCtrl();
	pview->SetTabStops(4);
	wndEdit.ModifyStyle(ES_WANTRETURN,ES_AUTOVSCROLL|ES_MULTILINE|ES_AUTOHSCROLL);
	wndEdit.SetWindowText(dcm_header_str.c_str());
	wndEdit.Invalidate();

}


void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
	std::map<std::string,int> open_files;
	POSITION pos = theApp.GetFirstDocTemplatePosition();
	while (pos)
	{
		CDocTemplate* pTemplate = (CDocTemplate*)theApp.GetNextDocTemplate(pos);
		POSITION pos2 = pTemplate->GetFirstDocPosition();
		while (pos2)
		{
			CDocument * pDocument;
			if ((pDocument=pTemplate->GetNextDoc(pos2)) != NULL)
			{
				open_files[(LPCTSTR)pDocument->GetPathName()] = 1;
			}
		}
	}

	TCHAR drop_path[PATH_LEN];
	int file_count = DragQueryFile(hDropInfo,0xFFFFFFFF,drop_path,sizeof(drop_path)/sizeof(drop_path[0]));
	bool new_file = false;
	for(int i=0;i<file_count;i++)
	{
		UINT uret = DragQueryFile(hDropInfo,i,drop_path,sizeof(drop_path)/sizeof(drop_path[0]));
		if(uret>0)
		{
			if(open_files.find(drop_path)==open_files.end())
			{
				theApp.OpenDocumentFile(drop_path);
				open_files[drop_path] = 1;
			}
		}
	}
	DragFinish(hDropInfo);
}



void CMainFrame::OnWindowCloseall()
{
	POSITION pos_doc = theApp.GetFirstDocTemplatePosition();
	while (pos_doc)
	{
		CDocTemplate* pTemplate = (CDocTemplate*)theApp.GetNextDocTemplate(pos_doc);
		POSITION pos2 = pTemplate->GetFirstDocPosition();
		while (pos2)
		{
			CDocument * pDocument;
			if ((pDocument=pTemplate->GetNextDoc(pos2)) != NULL)
			{
				CView* pView = NULL;
				POSITION pos = pDocument->GetFirstViewPosition();
				while (pos != NULL)
				{  
					pView=pDocument->GetNextView(pos);
					pView->GetParentFrame()->SendMessage(WM_CLOSE);
				}

			}
		}
	}
}


void CMainFrame::OnUpdateViewDicomHeader(CCmdUI *pCmdUI)
{

	//CMDIChildWnd* pMDIActive = MDIGetActive();
	//if(pMDIActive)
	//{
	//	CdcmviewerDoc *pDoc = (CdcmviewerDoc *)pMDIActive->GetActiveDocument();
	//	if(pDoc->m_dataset.size()>1)
	//		pCmdUI->Enable(TRUE);
	//}
	pCmdUI->Enable(TRUE);
	//pCmdUI->Enable(FALSE);
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->hwnd == m_hWndMDIClient && pMsg->message ==WM_ERASEBKGND)
	{
		return TRUE;
	}
	if(pMsg->hwnd==m_hWndMDIClient && pMsg->message==WM_PAINT) //in order to pain mainframe
		PostMessage(WM_PAINT);
	return CMDIFrameWnd::PreTranslateMessage(pMsg);
}

HRESULT get_picture(LPCTSTR restype,UINT resid,IPicture** pic)
{
	HRSRC hResLoad = NULL;     // handle to loaded resource 
	HMODULE hExe = NULL;        // handle to existing .EXE file 
	HRSRC hRes = NULL;         // handle/ptr. to res. info. in hExe 
	LPVOID lpResLock = NULL;    // pointer to resource data 

	hRes = FindResource(hExe, MAKEINTRESOURCE(resid), restype); 
	if (hRes == NULL) 
	{ 
		return E_FAIL; 
	} 
	hResLoad = (HRSRC)LoadResource(hExe, hRes); 
	if (hResLoad == NULL) 
	{ 
		return E_FAIL; 
	} 
	lpResLock = LockResource(hResLoad); 
	if (lpResLock == NULL) 
	{ 
		return E_FAIL; 
	} 
	int fileSize = SizeofResource(hExe, hRes);
	//Verbose(_T("SizeofResource return %d"),fileSize);
	CComPtr<IStream> pStream = NULL;
	CreateStreamOnHGlobal(NULL,TRUE,&pStream);
	if(pStream)
	{
		ULONG uwrite = 0;
		pStream->Write(lpResLock, fileSize,&uwrite);
		if(uwrite==fileSize)
		{
			LARGE_INTEGER li;
			li.u.HighPart=0;
			li.u.LowPart =0;
			pStream->Seek( li,STREAM_SEEK_SET,0);
			HRESULT hr=OleLoadPicture(pStream,0,TRUE,IID_IPicture,(LPVOID*)pic);
			return hr;					
		}
	}
	else
		return E_FAIL;
	return 0;
}

void CMainFrame::OnPaint()
{
	CRect rect;
	::GetClientRect(this->m_hWndMDIClient,&rect);
	CDC dc, memdc;
	dc.m_hDC=::GetDC(this->m_hWndMDIClient);
	dc.m_hAttribDC = ::GetDC(this->m_hWndMDIClient);
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(&dc,rect.Width(),rect.Height());
	memdc.CreateCompatibleDC(&dc);
	CBitmap *oldbitmap=memdc.SelectObject(&bitmap);
	if(m_pPicture == NULL)
		get_picture(_T("BIN"),IDR_MAINFRAME_BKGND,&m_pPicture);	
	IPicture* pPicture = m_pPicture;
	if(pPicture && false)
	{
		long hmWidth,hmHeight; // HIMETRIC units 
		pPicture->get_Width(&hmWidth);
		pPicture->get_Height(&hmHeight);
		CSize sz(hmWidth,hmHeight);
		dc.HIMETRICtoDP(&sz); //this may fail if m_hAttribDC was not set
		CDC dcmem ;
		dcmem.CreateCompatibleDC(&dc);
		CBitmap membmp;
		CRect clientrc;
		GetClientRect(&clientrc);
		membmp.CreateCompatibleBitmap(&dc,clientrc.Width(),clientrc.Height());
		CBitmap* pold=dcmem.SelectObject(&membmp);
		pPicture->Render(dcmem.m_hDC,0,0,sz.cx,sz.cy,0,hmHeight,hmWidth,-hmHeight,NULL);
		memdc.StretchBlt(0,0,clientrc.Width(),clientrc.Height(),&dcmem,0,0,sz.cx,sz.cy,SRCCOPY);
		dcmem.SelectObject(pold);
		membmp.DeleteObject();
		dcmem.DeleteDC();
		//return TRUE;
	}
	else
	{
		::FillRect(memdc.m_hDC,&rect,theApp.get_background_brush());
	}
	memdc.SetBkMode(TRANSPARENT);
	memdc.SetTextColor(theApp.m_font_color);
	HGDIOBJ pold_font = NULL;
	CFont newfont;
	if(theApp.get_logfont())
	{
		LOGFONT lf;
		memcpy(&lf,theApp.get_logfont(),sizeof(LOGFONT));
		lf.lfHeight = -CdcmviewerView::ytodc(&memdc,lf.lfHeight); // for printing
		newfont.CreateFontIndirect(&lf);
		pold_font = memdc.SelectObject( &newfont);
	}
	memdc.DrawText( theApp.get_res_string(IDR_BIN_DESCRIPTION,_T("BIN")).c_str(),&rect,0);
	if(theApp.get_logfont())
	{
		memdc.SelectObject(pold_font);
		newfont.DeleteObject();
	}
	dc.BitBlt(0,0,rect.Width(),rect.Height(),&memdc,0,0,SRCCOPY);
	memdc.SelectObject(oldbitmap);
	memdc.DeleteDC();
	dc.Detach();
	//dc.DeleteDC();
	bitmap.DeleteObject();
	CMDIFrameWnd::OnPaint();
}

void CMainFrame::MDIActivate(CWnd *pwnd)
{
	//::SendMessage(m_hWndMDIClient,WM_SETREDRAW,FALSE,0L);
	CMDIFrameWnd::MDIActivate(pwnd);
	//::SendMessage(m_hWndMDIClient,WM_SETREDRAW,TRUE,0L);
	//::InvalidateRect(m_hWndMDIClient,NULL,TRUE);
}

LRESULT CMainFrame::OnDicomStoreScp(WPARAM wp,LPARAM lp)
{
	boost::mutex::scoped_lock scoped_lock(g_store_mutex);
	for(TP_FILE_CONTAINER::iterator it = g_accepted_files.begin();
		it!=g_accepted_files.end();
		it++)
	{
		if(it->second == "newfile")
		{
			it->second = "";
			theApp.OpenDocumentFile(it->first.c_str());
		}
	}
	return 0L;
}
void CMainFrame::OnDicomConfig()
{
	ConfigDlg cdlg;
	cdlg.DoModal();
}

void CMainFrame::OnDicomAddressBook()
{
	AddressBookDlg address;
	address.DoModal();
}

void CMainFrame::OnDestroy()
{
	CMDIFrameWnd::OnDestroy();

	theApp.m_dicom_server->Stop();
}

void CMainFrame::OnHelpDepends()
{
	CdllsDlg dlldlg;
	dlldlg.DoModal();
}

BOOL CMainFrame::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;
	return CMDIFrameWnd::OnEraseBkgnd(pDC);
}
