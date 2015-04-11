#pragma once
#include "afxdtctl.h"
#include "afxcmn.h"

#include "TLFrame.h"
#include "afxwin.h"

// SearchView form view
#include <ResizableFormView.h>
class SearchView : public CResizableFormView
{
	DECLARE_DYNCREATE(SearchView)

protected:
	SearchView();           // protected constructor used by dynamic creation
	virtual ~SearchView();

public:
	enum { IDD = IDD_SEARCH };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRetrieve();
	afx_msg void OnBnClickedSearch();
	CDateTimeCtrl m_exam_start;
	CDateTimeCtrl m_exam_end;
	//CTreeCtrl m_tree;
	CTLFrame m_wndTreeList;
	virtual void OnInitialUpdate();
	CImageList m_cImageList;
	CComboBox m_cmbServer;
	afx_msg void OnBnClickedAddressBook();
	LRESULT OnItemExpanded(WPARAM w,LPARAM l);
private:
	CComboBox m_cmbModality;
};
void delete_children(CTreeCtrl &tree, HTREEITEM item);
int get_node_level(CTreeCtrl &tree,HTREEITEM hitem);
void PrintResult(const dicom::DataSet& data);
void dataset_insert(dicom::DataSet &query,dicom::Tag t,dicom::VR v, std::string s);





