// ChildFrm.cpp : implementation of the CChildFrame class
//
#include "stdafx.h"
#include "dcmviewer.h"

#include "ChildFrm.h"
#include ".\childfrm.h"
//#include ".\childfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_MDIACTIVATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
	// TODO: add member initialization code here
}

CChildFrame::~CChildFrame()
{
}


BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	//在MDI窗口中防止子窗口闪烁
	cs.style = cs.style | WS_MAXIMIZE ;
	cs.style &= ~WS_VISIBLE;
	cs.style &= ~FWS_ADDTOTITLE;
	CRect rect;
	AfxGetMainWnd()->GetClientRect(&rect);
	cs.cx = rect.Width();
	cs.cy = rect.Height();
	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;
	cs.style |= WS_VISIBLE; 
	return TRUE;
}


// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG


// CChildFrame message handlers

BOOL CChildFrame::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;

	return CMDIChildWnd::OnEraseBkgnd(pDC);
}

int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	//MDIMaximize();
	//ShowWindow(SW_SHOWMAXIMIZED); 
	//GetParent()->SendMessage( WM_SETREDRAW, TRUE, 0);
	//GetParent()->RedrawWindow( NULL, NULL,
	//	RDW_INVALIDATE | RDW_ALLCHILDREN);
	if (!m_toolbar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_toolbar.LoadToolBar(IDR_VIEW_TOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	return 0;
}

void CChildFrame::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd)
{
	CMDIChildWnd::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);
	if(m_title.GetLength()>0)
		SetWindowText(m_title);
	//else
	//	SetWindowText(GetActiveDocument()->GetPathName());
}

void CChildFrame::OnSize(UINT nType, int cx, int cy)
{
	CMDIChildWnd::OnSize(nType, cx, cy);
	if(m_title.GetLength()>0)
		SetWindowText(m_title);
}
