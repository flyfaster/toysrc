#pragma once


// CScrollEditView view

class CScrollEditView : public CEditView
{
	DECLARE_DYNCREATE(CScrollEditView)

public:
	CScrollEditView();           // protected constructor used by dynamic creation
	virtual ~CScrollEditView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};


