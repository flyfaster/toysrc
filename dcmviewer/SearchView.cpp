// SearchView.cpp : implementation file
//

#include "stdafx.h"
#include "dcmviewer.h"
#include "SearchView.h"
#include "addressbookdlg.h"
#include "DataSet.hpp"
#include "dcmviewer_const.h"
#include "AddressBookDlg.h"
#include "NewTreeListCtrl.h"
#include "onegadebug.h"
#include "boost/date_time/posix_time/posix_time_types.hpp"
// SearchView

IMPLEMENT_DYNCREATE(SearchView, CResizableFormView)

SearchView::SearchView()
	: CResizableFormView(SearchView::IDD)
{
}

SearchView::~SearchView()
{
}

void SearchView::DoDataExchange(CDataExchange* pDX)
{
	CResizableFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EXAM_START_DATE, m_exam_start);
	DDX_Control(pDX, IDC_EXAM_END_DATE, m_exam_end);
	DDX_Control(pDX, IDC_SERVER, m_cmbServer);
	DDX_Control(pDX, IDC_MODALITY, m_cmbModality);
}

BEGIN_MESSAGE_MAP(SearchView, CResizableFormView)
	ON_BN_CLICKED(IDB_RETRIEVE, OnBnClickedRetrieve)
	ON_BN_CLICKED(IDB_SEARCH, OnBnClickedSearch)
	ON_BN_CLICKED(IDB_ADDRESS_BOOK, OnBnClickedAddressBook)
	ON_MESSAGE(WM_NEWTREE_ITEM_EXPAND,OnItemExpanded)
END_MESSAGE_MAP()


// SearchView diagnostics

#ifdef _DEBUG
void SearchView::AssertValid() const
{
	CResizableFormView::AssertValid();
}

void SearchView::Dump(CDumpContext& dc) const
{
	CResizableFormView::Dump(dc);
}
#endif //_DEBUG


// SearchView message handlers
LPCTSTR QR_LEVEL[] = {"PATIENT", "STUDY","SERIES","IMAGE"
};

void SearchView::OnBnClickedRetrieve()
{
	HTREEITEM hitem_selected = m_wndTreeList.m_tree.GetSelectedItem();
	if(NULL==hitem_selected)
		return;
	int node_level  = get_node_level(m_wndTreeList.m_tree,hitem_selected);
	// C-MOVE
	dicom::PresentationContexts presentation_contexts;
	presentation_contexts.Add(dicom::STUDY_ROOT_QR_MOVE_SOP_CLASS);
	std::string host = "localhost";
	std::string local_AE = "localhost";
	std::string remote_AE = "CONQUESTSRV1";
	short remote_port = 0;		//standard dicom port
	TCHAR buf[1024];
	buf[0] = 0;
	m_cmbServer.GetLBText(m_cmbServer.GetCurSel(),buf);
	std::basic_string<TCHAR> addressname = buf;
	std::basic_ostringstream<TCHAR> oss;
	oss<<ADDRESS_BOOK_PATH<<"\\"<<addressname;
	CRegKey reg;
	if(ERROR_SUCCESS==reg.Open(HKEY_LOCAL_MACHINE,oss.str().c_str()))
	{
		TCHAR valbuf[MAX_PATH];
		LONG ret = ERROR_SUCCESS;
		ULONG len = 0;
		valbuf[0] = 0;
		len = sizeof(valbuf);
		ret = reg.QueryStringValue(_T("remote_ae"),valbuf,&len);
		remote_AE = valbuf;
		valbuf[0] = 0;
		len = sizeof(valbuf);
		ret = reg.QueryStringValue(_T("local_ae"),valbuf,&len);
		local_AE = valbuf;
		valbuf[0] = 0;
		len = sizeof(valbuf);
		ret = reg.QueryStringValue(_T("port"),valbuf,&len);
		remote_port = _ttoi(valbuf);
		valbuf[0] = 0;
		len = sizeof(valbuf);
		ret = reg.QueryStringValue(_T("ip"),valbuf,&len);
		host = valbuf;
	}
	else
	{
		AfxMessageBox(_T("must choose a server"));
		return;
	}
	if(remote_port<=0 || remote_AE.length()<=0 || host.length()<=0)
	{
		AfxMessageBox(_T("invalid server configuration"));
		return;
	}
	//build a query to search for all studies
	dicom::DataSet query;
	query.Put<dicom::VR_CS>(dicom::TAG_QR_LEVEL,std::string( QR_LEVEL[node_level-1]));
	if(1==node_level)
	{
		HTREEITEM hpatitem = hitem_selected;
		CString patientname = m_wndTreeList.m_tree.GetItemText(hpatitem,0);
		dataset_insert(query,dicom::TAG_PAT_NAME,dicom::VR_PN,std::string( LPCTSTR(patientname)) );
		CString patientid = m_wndTreeList.m_tree.GetItemText(hpatitem,1);
		query.Put<dicom::VR_LO>(dicom::TAG_PAT_ID, std::string( LPCTSTR(patientid)) );
	}
	else
		if(2==node_level)
	{
		HTREEITEM hstudyitem = hitem_selected;
		CString studyuid = m_wndTreeList.m_tree.GetItemText(hstudyitem,1);
		query.Put<dicom::VR_UI>(dicom::TAG_STUDY_INST_UID,dicom::UID( LPCTSTR(studyuid)) );
		HTREEITEM hpatitem = m_wndTreeList.m_tree.GetParentItem(hstudyitem);
		CString patientid = m_wndTreeList.m_tree.GetItemText(hpatitem,1);
		query.Put<dicom::VR_LO>(dicom::TAG_PAT_ID, std::string( LPCTSTR(patientid)) );
	}
	else
		if(3==node_level)
		{
			HTREEITEM hseriesitem = hitem_selected;
			CString series_uid = m_wndTreeList.m_tree.GetItemText(hseriesitem,1);
			query.Put<dicom::VR_UI>(dicom::TAG_SERIES_INST_UID,dicom::UID( LPCTSTR(series_uid)) );
			HTREEITEM hstudyitem = m_wndTreeList.m_tree.GetParentItem(hseriesitem);
			CString studyuid = m_wndTreeList.m_tree.GetItemText(hstudyitem,1);
			query.Put<dicom::VR_UI>(dicom::TAG_STUDY_INST_UID,dicom::UID( LPCTSTR(studyuid)) );
			HTREEITEM hpatitem = m_wndTreeList.m_tree.GetParentItem(hstudyitem);
			CString patientid = m_wndTreeList.m_tree.GetItemText(hpatitem,1);
			query.Put<dicom::VR_LO>(dicom::TAG_PAT_ID, std::string( LPCTSTR(patientid)) );

		}
		else
		{
			//image level
			HTREEITEM himgitem = hitem_selected;
			CString sop_uid = m_wndTreeList.m_tree.GetItemText(himgitem,1);
			query.Put<dicom::VR_UI>(dicom::TAG_SOP_INST_UID,dicom::UID( LPCTSTR(sop_uid)) );
			HTREEITEM hseriesitem = m_wndTreeList.m_tree.GetParentItem(himgitem);
			CString series_uid = m_wndTreeList.m_tree.GetItemText(hseriesitem,1);
			query.Put<dicom::VR_UI>(dicom::TAG_SERIES_INST_UID,dicom::UID( LPCTSTR(series_uid)) );
			HTREEITEM hstudyitem = m_wndTreeList.m_tree.GetParentItem(hseriesitem);
			CString studyuid = m_wndTreeList.m_tree.GetItemText(hstudyitem,1);
			query.Put<dicom::VR_UI>(dicom::TAG_STUDY_INST_UID,dicom::UID( LPCTSTR(studyuid)) );
			HTREEITEM hpatitem = m_wndTreeList.m_tree.GetParentItem(hstudyitem);
			CString patientid = m_wndTreeList.m_tree.GetItemText(hpatitem,1);
			query.Put<dicom::VR_LO>(dicom::TAG_PAT_ID, std::string( LPCTSTR(patientid)) );
		}
	try
	{
		dicom::ClientConnection client(host,remote_port,local_AE,remote_AE,presentation_contexts);
		dicom::DataSet result = client.Move(local_AE,query,dicom::QueryRetrieve::STUDY_ROOT);
		PrintResult(result);
	}
	catch(std::exception &e)
	{
		AfxMessageBox(e.what());
	}
	OnegaDebug::Dump("result of C-MOVE %s",QR_LEVEL[node_level-1]);
}
void PrintResult(const dicom::DataSet& data)
{
	std::ostringstream oss;
	oss << "Query result:" << std::endl << data /*<< std::endl*/;
	OnegaDebug::Dump(oss.str().c_str());
}
void dataset_insert(dicom::DataSet &query,dicom::Tag t,dicom::VR v, std::string s)
{
	dicom::Value valtmp( v,s);
	query.insert(dicom::DataSet::value_type(t,valtmp ) );
}
void SearchView::OnBnClickedSearch()
{
	// C-FIND
	dicom::PresentationContexts presentation_contexts;
	presentation_contexts.Add(dicom::STUDY_ROOT_QR_FIND_SOP_CLASS);
	std::string host = "localhost";
	std::string local_AE = "localhost";
	std::string remote_AE = "CONQUESTSRV1";
	short remote_port = 0;		//standard dicom port
	TCHAR buf[1024];
	buf[0] = 0;
	m_cmbServer.GetLBText(m_cmbServer.GetCurSel(),buf);
	std::basic_string<TCHAR> addressname = buf;
	std::basic_ostringstream<TCHAR> oss;
	oss<<ADDRESS_BOOK_PATH<<"\\"<<addressname;
	CRegKey reg;
	if(ERROR_SUCCESS==reg.Open(HKEY_LOCAL_MACHINE,oss.str().c_str()))
	{
		TCHAR valbuf[MAX_PATH];
		LONG ret = ERROR_SUCCESS;
		ULONG len = 0;
		valbuf[0] = 0;
		len = sizeof(valbuf);
		ret = reg.QueryStringValue(_T("remote_ae"),valbuf,&len);
		remote_AE = valbuf;
		valbuf[0] = 0;
		len = sizeof(valbuf);
		ret = reg.QueryStringValue(_T("local_ae"),valbuf,&len);
		local_AE = valbuf;
		valbuf[0] = 0;
		len = sizeof(valbuf);
		ret = reg.QueryStringValue(_T("port"),valbuf,&len);
		remote_port = _ttoi(valbuf);
		valbuf[0] = 0;
		len = sizeof(valbuf);
		ret = reg.QueryStringValue(_T("ip"),valbuf,&len);
		host = valbuf;
	}
	else
	{
		AfxMessageBox(_T("must choose a server"));
		return;
	}
	if(remote_port<=0 || remote_AE.length()<=0 || host.length()<=0)
	{
		AfxMessageBox(_T("invalid server configuration"));
		return;
	}
	//build a query to search for all studies
	dicom::DataSet query;
	query.Put<dicom::VR_CS>(dicom::TAG_QR_LEVEL,std::string("STUDY"));
	query.Put<dicom::VR_UI>(dicom::TAG_STUDY_INST_UID,dicom::UID("*"));
	GetDlgItemText(IDE_PATIENT_NAME,buf,sizeof(buf)/sizeof(buf[0]));
	query.Put<dicom::VR_PN>(dicom::TAG_PAT_NAME, std::string(buf) );
	GetDlgItemText(IDE_PATIENT_ID,buf,sizeof(buf)/sizeof(buf[0]));
	query.Put<dicom::VR_LO>(dicom::TAG_PAT_ID,std::string(buf) );
	dataset_insert(query,dicom::TAG_PAT_BIRTH_DATE,dicom::VR_SH,"");
	//exam date range
	SYSTEMTIME ststart,stend;
	m_exam_start.GetTime(&ststart);
	m_exam_end.GetTime(&stend);
	if(ststart.wYear == stend.wYear && ststart.wMonth == stend.wMonth && ststart.wDay == stend.wDay)
	{
		boost::gregorian::date examdate(ststart.wYear,ststart.wMonth,ststart.wDay);
		query.Put<dicom::VR_DA>(dicom::TAG_STUDY_DATE,examdate);
	}
	else
	{
		SYSTEMTIME st;
		m_exam_start.GetTime(&st);
		std::ostringstream oss;
		oss.str("");
		oss<<st.wYear<<std::setw(2)<<std::setfill('0')<<st.wMonth<<std::setw(2)<<st.wDay;
		oss<<"-";
		m_exam_end.GetTime(&st);
		oss<<st.wYear<<std::setw(2)<<std::setfill('0')<<st.wMonth<<std::setw(2)<<st.wDay;
		dataset_insert(query,dicom::TAG_STUDY_DATE,dicom::VR_ST,oss.str());
	}
	query.Put<dicom::VR_SH>(dicom::TAG_STUDY_ID,std::string() );
	//execute the query
	m_wndTreeList.m_tree.DeleteAllItems();
	m_wndTreeList.m_tree.SetRedraw(FALSE);
	HTREEITEM hTmp = NULL;
	TCHAR buftmp[128];
	try
	{
		dicom::ClientConnection client(host,remote_port,local_AE,remote_AE,presentation_contexts);
		std::vector<dicom::DataSet> result = client.Find(query,dicom::QueryRetrieve::STUDY_ROOT);
		std::vector<dicom::DataSet>::const_iterator it = result.begin();
		while(it!=result.end())
		{
			std::string patname;
			const dicom::DataSet &ds = (*it);
			ds.operator()(dicom::TAG_PAT_NAME).Get<std::string>(patname);
			hTmp = m_wndTreeList.m_tree.InsertItem( patname.c_str(), 0, 0,hTmp);

			std::string stmp;
			if(ds.find(dicom::TAG_PAT_ID)!=ds.end())
			{
				ds.operator()(dicom::TAG_PAT_ID).Get<std::string>(stmp);
				m_wndTreeList.m_tree.SetItemText(hTmp, 1, stmp.c_str());
				if(ds.find(dicom::TAG_PAT_BIRTH_DATE)!=ds.end())
				{
					boost::gregorian::date tmpdate = boost::posix_time::second_clock::local_time().date() ;
					ds.operator()(dicom::TAG_PAT_BIRTH_DATE).Get<boost::gregorian::date>(tmpdate);
					StringCchPrintf(buftmp,sizeof(buftmp)/sizeof(buftmp[0]),_T("%04d/%02d/%02d"),tmpdate.year(),tmpdate.month(),tmpdate.day());
					m_wndTreeList.m_tree.SetItemText(hTmp, 2, buftmp);
				}
			}
			else
			{
				it++;
				continue;
			}
			while(it!=result.end())
			{
				const dicom::DataSet &ds = (*it);
				ds.operator()(dicom::TAG_PAT_NAME).Get<std::string>(stmp);
				if(stmp==patname)
				{
					std::string studyid;
					if(ds.find(dicom::TAG_STUDY_ID)!=ds.end())
					{
						ds.operator()(dicom::TAG_STUDY_ID).Get<std::string>(studyid);
						hTmp = m_wndTreeList.m_tree.InsertItem( studyid.c_str(), 0, 0,hTmp);
						dicom::UID tmp_uid;
						ds.operator()(dicom::TAG_STUDY_INST_UID).Get<dicom::UID>(tmp_uid);
						m_wndTreeList.m_tree.SetItemText(hTmp, 1, tmp_uid.str().c_str());
						HTREEITEM tmp_series_item = m_wndTreeList.m_tree.InsertItem("",0,0,hTmp);
						m_wndTreeList.m_tree.InsertItem(_T(""),0,0,tmp_series_item);
					}
					m_wndTreeList.m_tree.SetItemText(hTmp, 2, "1/1/98");
					it++;
				}
				else
					break;
			}
		}	
	}
	catch(std::exception &e)
	{
		AfxMessageBox(e.what());
	}

	m_wndTreeList.m_tree.SetRedraw(TRUE);
	m_wndTreeList.m_tree.Invalidate(TRUE);
}

void SearchView::OnInitialUpdate()
{
	AddAnchor(IDC_TREELIST,TOP_LEFT,BOTTOM_RIGHT);
	//ResizeParentToFit();

	__super::OnInitialUpdate();

	SYSTEMTIME st;
	st.wYear = 1980;
	st.wMonth = 1;
	st.wDay = 1;
	st.wHour = 0;
	st.wMinute = 0;
	st.wSecond = 0;
	st.wMilliseconds = 0;
	m_exam_start.SetTime(&st);
	m_wndTreeList.SubclassDlgItem(IDC_TREELIST, this);
	m_wndTreeList.m_tree.SetHandlerWnd(this->m_hWnd);
	m_cImageList.Create(IDB_DOCS_IMAGES, 16, 10, RGB(255, 255, 255));
	m_wndTreeList.m_tree.SetImageList(&m_cImageList, TVSIL_NORMAL);
	AddressBookDlg::FillCmb(m_cmbServer);
	{
		int char_width = 6;
		m_wndTreeList.m_tree.InsertColumn(0, "Name", LVCFMT_LEFT, 24*char_width);
		m_wndTreeList.m_tree.InsertColumn(1, "ID/UID", LVCFMT_LEFT, 16*char_width);
		m_wndTreeList.m_tree.InsertColumn(2, "Date", LVCFMT_RIGHT, 12*char_width);
	}
	if(m_cmbModality.GetCount()<1)
	{	//PS03 C.7.3.1.1.1 Defined Terms for the Modality
		m_cmbModality.AddString(_T("CR"));
		m_cmbModality.AddString(_T("MR"));
		m_cmbModality.AddString(_T("CT"));
		m_cmbModality.AddString(_T("NM"));
		m_cmbModality.AddString(_T("US"));
		m_cmbModality.AddString(_T("OT"));
		m_cmbModality.AddString(_T("BI"));
		m_cmbModality.AddString(_T("CD"));
		m_cmbModality.AddString(_T("DD"));
		m_cmbModality.AddString(_T("DG"));
		m_cmbModality.AddString(_T("ES"));
		m_cmbModality.AddString(_T("LS"));
		m_cmbModality.AddString(_T("PT"));
		m_cmbModality.AddString(_T("RG"));
		m_cmbModality.AddString(_T("ST"));
		m_cmbModality.AddString(_T("TG"));
		m_cmbModality.AddString(_T("XA"));
		m_cmbModality.AddString(_T("RF"));
		m_cmbModality.AddString(_T("RTIMAGE"));
		m_cmbModality.AddString(_T("RTDOSE"));
		m_cmbModality.AddString(_T("RTSTRUCT"));
		m_cmbModality.AddString(_T("RTDOSE"));
		m_cmbModality.AddString(_T("RTPLAN"));
		m_cmbModality.AddString(_T("RTRECORD"));
		m_cmbModality.AddString(_T("HC"));
		m_cmbModality.AddString(_T("DX"));
		m_cmbModality.AddString(_T("MG"));
		m_cmbModality.AddString(_T("IO"));
		m_cmbModality.AddString(_T("PX"));
		m_cmbModality.AddString(_T("GM"));
		m_cmbModality.AddString(_T("SM"));
		m_cmbModality.AddString(_T("XC"));
		m_cmbModality.AddString(_T("PR"));
		m_cmbModality.AddString(_T("AU"));
		m_cmbModality.AddString(_T("ECG"));
		m_cmbModality.AddString(_T("EPS"));
		m_cmbModality.AddString(_T("HD"));
		m_cmbModality.AddString(_T("SR"));
		m_cmbModality.AddString(_T("IVUS"));
		//Retired Defined Terms for the Modality
		m_cmbModality.AddString(_T("DS"));
		m_cmbModality.AddString(_T("CF"));
		m_cmbModality.AddString(_T("DF"));
		m_cmbModality.AddString(_T("VF"));
		m_cmbModality.AddString(_T("AS"));
		m_cmbModality.AddString(_T("CS"));
		m_cmbModality.AddString(_T("EC"));
		m_cmbModality.AddString(_T("LP"));
		m_cmbModality.AddString(_T("FA"));
		m_cmbModality.AddString(_T("CP"));
		m_cmbModality.AddString(_T("DM"));
		m_cmbModality.AddString(_T("FS"));
		m_cmbModality.AddString(_T("MA"));
		m_cmbModality.AddString(_T("MS"));
	}
}

void SearchView::OnBnClickedAddressBook()
{
	AddressBookDlg addressdlg;
	addressdlg.DoModal();
	AddressBookDlg::FillCmb(m_cmbServer);
}
void delete_children(CTreeCtrl &tree, HTREEITEM item)
{
	if(item)
	{
		HTREEITEM hChild = tree.GetChildItem( item );
		while ( hChild ) 
		{
			tree.DeleteItem( hChild );
			hChild = tree.GetChildItem( item );
		}
	}
}
int get_node_level(CTreeCtrl &tree,HTREEITEM hitem)
{
	int node_level = 0;
	while(hitem)
	{
		node_level++;
		hitem = tree.GetParentItem(hitem);
	}
	return node_level;
}
LRESULT SearchView::OnItemExpanded(WPARAM hitem_expand,LPARAM l)
{
 	HTREEITEM hitem = reinterpret_cast<HTREEITEM>(hitem_expand);
	HTREEITEM htmpitem = hitem;
	CString msg = m_wndTreeList.m_tree.GetItemText(hitem,0);
	std::basic_stringstream<TCHAR> oss;
	oss<<__TFUNCTION__<<" "<<(LPCTSTR)msg;
	int node_level = get_node_level(m_wndTreeList.m_tree,hitem);
	if(node_level!=2 && node_level!=3)
		return 0;//only handle expanding study and series
	oss<<" node level:"<<node_level;
	hitem = htmpitem;
	//node_level = 1: expand patient
		// C-FIND
		dicom::PresentationContexts presentation_contexts;
		presentation_contexts.Add(dicom::STUDY_ROOT_QR_FIND_SOP_CLASS);
		//presentation_contexts.Add(dicom::APPLICATION_CONTEXT);
		std::string host = "localhost";
		std::string local_AE = "localhost";
		std::string remote_AE = "CONQUESTSRV1";
		short remote_port = 0;		//standard dicom port
		TCHAR buf[1024];
		buf[0] = 0;
		m_cmbServer.GetLBText(m_cmbServer.GetCurSel(),buf);
		std::basic_string<TCHAR> addressname = buf;
		oss.str(_T(""));
		oss<<ADDRESS_BOOK_PATH<<"\\"<<addressname;
		CRegKey reg;
		if(ERROR_SUCCESS==reg.Open(HKEY_LOCAL_MACHINE,oss.str().c_str()))
		{
			TCHAR valbuf[MAX_PATH];
			LONG ret = ERROR_SUCCESS;
			ULONG len = 0;
			valbuf[0] = 0;
			len = sizeof(valbuf);
			ret = reg.QueryStringValue(_T("remote_ae"),valbuf,&len);
			remote_AE = valbuf;
			valbuf[0] = 0;
			len = sizeof(valbuf);
			ret = reg.QueryStringValue(_T("local_ae"),valbuf,&len);
			local_AE = valbuf;
			valbuf[0] = 0;
			len = sizeof(valbuf);
			ret = reg.QueryStringValue(_T("port"),valbuf,&len);
			remote_port = _ttoi(valbuf);
			valbuf[0] = 0;
			len = sizeof(valbuf);
			ret = reg.QueryStringValue(_T("ip"),valbuf,&len);
			host = valbuf;
		}
		else
		{
			AfxMessageBox(_T("must choose a server"));
			return 0;
		}
		if(remote_port<=0 || remote_AE.length()<=0 || host.length()<=0)
		{
			AfxMessageBox(_T("invalid server configuration"));
			return 0;
		}
		dicom::ClientConnection client(host,remote_port,local_AE,remote_AE,presentation_contexts);
		//build a query to search for all studies
		dicom::DataSet query;
		delete_children(m_wndTreeList.m_tree,reinterpret_cast<HTREEITEM>(hitem_expand) );
		m_wndTreeList.m_tree.SetRedraw(FALSE);
		HTREEITEM hTmp = NULL;
	try
	{
		//=2	expand study
		if(2 == node_level)
		{
			//search series
			query.Put<dicom::VR_CS>(dicom::TAG_QR_LEVEL,std::string("SERIES"));
			CString studyuid = m_wndTreeList.m_tree.GetItemText(hitem,1);
			query.Put<dicom::VR_UI>(dicom::TAG_STUDY_INST_UID,dicom::UID(LPCTSTR(studyuid)));

			HTREEITEM hpatitem = m_wndTreeList.m_tree.GetParentItem(hitem);
			CString patientname = m_wndTreeList.m_tree.GetItemText(hpatitem,0);
			query.Put<dicom::VR_PN>(dicom::TAG_PAT_NAME, std::string( (LPCTSTR)patientname) );
			query.Put<dicom::VR_IS>(dicom::TAG_SERIES_NO, std::string("") );
			CString modality;
			m_cmbModality.GetWindowText(modality);
			query.Put<dicom::VR_CS>(dicom::TAG_MODALITY, std::string(LPCTSTR(modality)) );
			dataset_insert(query,dicom::TAG_SERIES_INST_UID,dicom::VR_SH,"");
			//execute the query
			std::vector<dicom::DataSet> result = client.Find(query,dicom::QueryRetrieve::STUDY_ROOT);
			std::vector<dicom::DataSet>::const_iterator it = result.begin();
			while(it!=result.end())
			{
				std::string strtmp;
				const dicom::DataSet &ds = (*it);
				if(ds.find(dicom::TAG_SERIES_NO)!=ds.end())
				{
					ds.operator()(dicom::TAG_SERIES_NO).Get<std::string>(strtmp);
					hTmp = m_wndTreeList.m_tree.InsertItem( strtmp.c_str(), 0, 0,reinterpret_cast<HTREEITEM>(hitem_expand));
					m_wndTreeList.m_tree.InsertItem(_T(""),0,0,hTmp);
					dicom::UID tmp_uid;
					ds.operator()(dicom::TAG_SERIES_INST_UID).Get<dicom::UID>(tmp_uid);
					m_wndTreeList.m_tree.SetItemText(hTmp, 1, tmp_uid.str().c_str());
				}
				it++;
			}
			//m_wndTreeList.m_tree.SetRedraw(TRUE);
			//m_wndTreeList.m_tree.Invalidate(TRUE);
		}
		//=3	expand series
		if(3 == node_level)
		{
			//search series
			query.Put<dicom::VR_CS>(dicom::TAG_QR_LEVEL,std::string("IMAGE"));
			query.Put<dicom::VR_IS>(dicom::TAG_IMAGE_NO,std::string("") );
			dataset_insert(query,dicom::TAG_SOP_INST_UID,dicom::VR_SH,std::string(""));

			CString series_uid = m_wndTreeList.m_tree.GetItemText(hitem,1);
			query.Put<dicom::VR_UI>(dicom::TAG_SERIES_INST_UID, dicom::UID( (LPCTSTR)series_uid) );

			HTREEITEM studyiditem = m_wndTreeList.m_tree.GetParentItem(hitem);
			CString studyuid = m_wndTreeList.m_tree.GetItemText(studyiditem,1);
			query.Put<dicom::VR_UI>(dicom::TAG_STUDY_INST_UID,dicom::UID(LPCTSTR(studyuid)));

			HTREEITEM hpatitem = m_wndTreeList.m_tree.GetParentItem(studyiditem);
			CString patid = m_wndTreeList.m_tree.GetItemText(hpatitem,1);
			query.Put<dicom::VR_LO>(dicom::TAG_PAT_ID,std::string(LPCTSTR(patid)) );
			//execute the query
			OnegaDebug::Dump("image level query");
			PrintResult(query);
			std::vector<dicom::DataSet> result = client.Find(query,dicom::QueryRetrieve::STUDY_ROOT);
			//m_wndTreeList.m_tree.SetRedraw(FALSE);
			//delete_children(m_wndTreeList.m_tree,reinterpret_cast<HTREEITEM>(hitem_expand) );
			//HTREEITEM hTmp = NULL;
			std::vector<dicom::DataSet>::const_iterator it = result.begin();
			std::string strtmp;
			while(it!=result.end())
			{
				const dicom::DataSet &ds = (*it);
				if(ds.find(dicom::TAG_IMAGE_NO)!=ds.end())
				{
					ds.operator()(dicom::TAG_IMAGE_NO).Get<std::string>(strtmp);
					hTmp = m_wndTreeList.m_tree.InsertItem( strtmp.c_str(), 0, 0,reinterpret_cast<HTREEITEM>(hitem_expand));
					dicom::UID tmp_uid;
					ds.operator()(dicom::TAG_SOP_INST_UID).Get<dicom::UID>(tmp_uid);
					m_wndTreeList.m_tree.SetItemText(hTmp, 1, tmp_uid.str().c_str());
				}
				it++;
			}
		}
	}
	catch(std::exception &e)
	{
		AfxMessageBox(e.what());
	}

	m_wndTreeList.m_tree.SetRedraw(TRUE);
	m_wndTreeList.m_tree.Invalidate(TRUE);
	OnegaDebug::Dump(oss.str().c_str());
	return 0;
}