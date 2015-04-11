// MainFrm.h : interface of the CMainFrame class
//


#pragma once
class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();
	void CreateOrActivateFrame(CDocTemplate* pTemplate,CRuntimeClass* pViewClass);

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CComPtr<IPicture> m_pPicture;
// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnViewDicomHeader();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnWindowCloseall();
	afx_msg void OnUpdateViewDicomHeader(CCmdUI *pCmdUI);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();
	void MDIActivate(CWnd *pwnd);
	LRESULT OnDicomStoreScp(WPARAM wp,LPARAM lp);
	afx_msg void OnDicomConfig();
	afx_msg void OnDicomAddressBook();
	afx_msg void OnDestroy();
	afx_msg void OnHelpDepends();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

