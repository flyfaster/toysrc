#if !defined(AFX_GRIDLISTCTRL_H__92CA5999_9434_11D1_88D5_444553540000__INCLUDED_)
#define AFX_GRIDLISTCTRL_H__92CA5999_9434_11D1_88D5_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// GridListCtrl.h : header file
//
class CInPlaceEdit : public CEdit
{
	// Construction
public:
	CInPlaceEdit(int iItem, int iSubItem, CString sInitText);

	// Attributes
public:

	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInPlaceEdit)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CInPlaceEdit();
	void CalculateSize();

	// Generated message map functions
protected:
	//{{AFX_MSG(CInPlaceEdit)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	int m_iItem;
	int m_iSubItem;
	CString m_sInitText;
	BOOL    m_bESC;         // To indicate whether ESC key was pressed
};
/////////////////////////////////////////////////////////////////////////////
// CGridListCtrl window
typedef std::basic_string<TCHAR> tchar_string;

class CGridListCtrl : public CListCtrl
{
// Construction
public:
	CGridListCtrl();

// Attributes
public:
	// The current subitem or column number which is order based.
	int m_CurSubItem;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGridListCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	void MakeColumnVisible(int nCol);
	BOOL PositionControl( CWnd * pWnd, int iItem, int iSubItem );
	BOOL PrepareControl(WORD wParseStyle);
	virtual ~CGridListCtrl();
	// Generated message map functions
protected:
	//{{AFX_MSG(CGridListCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	WORD m_wStyle;
	DECLARE_MESSAGE_MAP()
private:
	int IndexToOrder( int iIndex );
	CInPlaceEdit *m_pListEdit;
	std::deque<int> m_qEditableCols;
	bool m_bEditPerCol;
public:
	afx_msg void OnLvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	void ResetEditableCols(void);
	void AddEditableCol(int columnindex);
	bool IsColEditable(int nCol);
	int GetColIndexByName(tchar_string col_name);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRIDLISTCTRL_H__92CA5999_9434_11D1_88D5_444553540000__INCLUDED_)
