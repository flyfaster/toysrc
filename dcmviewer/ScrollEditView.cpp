// ScrollEditView.cpp : implementation file
//

#include "stdafx.h"
#include "dcmviewer.h"
#include "ScrollEditView.h"
#include ".\scrolleditview.h"


// CScrollEditView

IMPLEMENT_DYNCREATE(CScrollEditView, CEditView)

CScrollEditView::CScrollEditView()
{
}

CScrollEditView::~CScrollEditView()
{
}

BEGIN_MESSAGE_MAP(CScrollEditView, CEditView)
END_MESSAGE_MAP()


// CScrollEditView diagnostics

#ifdef _DEBUG
void CScrollEditView::AssertValid() const
{
	CEditView::AssertValid();
}

void CScrollEditView::Dump(CDumpContext& dc) const
{
	CEditView::Dump(dc);
}
#endif //_DEBUG


// CScrollEditView message handlers

BOOL CScrollEditView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class
cs.style |= (ES_AUTOHSCROLL|WS_HSCROLL | ES_AUTOVSCROLL | WS_VSCROLL);

	return CEditView::PreCreateWindow(cs);
}
