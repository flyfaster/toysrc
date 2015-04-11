// dcmviewerDoc.cpp : implementation of the CdcmviewerDoc class
//

#include "stdafx.h"
#include "dcmviewer.h"

#include "dcmviewerDoc.h"
#include ".\dcmviewerdoc.h"
#include "File.hpp"
#include "Decoder.hpp"
#include "MainFrm.h"
#include "histogramview.h"
#include "ChildFrm.h"
#include "dcmviewerview.h"
#include "ScrollEditView.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CdcmviewerDoc

IMPLEMENT_DYNCREATE(CdcmviewerDoc, CDocument)

BEGIN_MESSAGE_MAP(CdcmviewerDoc, CDocument)
	ON_COMMAND(ID_VIEW_HISTOGRAM, OnViewHistogram)
	ON_COMMAND(ID_VIEW_DICOM_HEADER, OnViewDicomHeader)
	ON_COMMAND(ID_VIEW_IMAGE, OnViewImage)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
END_MESSAGE_MAP()


// CdcmviewerDoc construction/destruction

CdcmviewerDoc::CdcmviewerDoc():m_bIsBigEndian(false),m_frame_index(0)
{
	// TODO: add one-time construction code here

}

CdcmviewerDoc::~CdcmviewerDoc()
{
}

BOOL CdcmviewerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CdcmviewerDoc serialization

void CdcmviewerDoc::Serialize(CArchive& ar)
{

	if (ar.IsStoring())
	{
		//save contents of CEditView when we are opening an text file
		CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
		if(pMain)
		{
			CFrameWnd* pFrame = pMain->GetActiveFrame();
			if(pFrame)
			{
				CView* pView = pFrame->GetActiveView();
				if(pView && pView->IsKindOf(RUNTIME_CLASS(CEditView)))
				{
					//the OnSaveDocument would check that file name is .txt
					CEditView* peditview = (CEditView*)pView;
					peditview->WriteToArchive(ar);
				}
			}
		}
	}
	else
	{
		// TODO: add loading code here
	}
}


// CdcmviewerDoc diagnostics

#ifdef _DEBUG
void CdcmviewerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CdcmviewerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

size_t GetStreamSize(std::ifstream& In)
{
	std::streampos CurrentPosition=In.tellg();	//get current position.
	In.seekg(0,std::ios::end);					//jump to end
	long StreamSize=In.tellg();					//get stream length.
	In.seekg(CurrentPosition);					//reset
	return StreamSize;
}
// CdcmviewerDoc commands
bool ReadFromStream(std::ifstream& In, dicom::DataSet& data)
{
	//using namespace dicom;
	data.clear();
	dicom::UID TransferSyntaxUID=dicom::IMPL_VR_LE_TRANSFER_SYNTAX;//default
	try
	{
		dicom::FileMetaInformation MetaInfo(In);
		MetaInfo.MetaElements_(dicom::TAG_TRANSFER_SYNTAX_UID) >> TransferSyntaxUID;
		for(dicom::DataSet::const_iterator it = MetaInfo.MetaElements_.begin();
			it!=MetaInfo.MetaElements_.end();
			it++)
		{
			data.insert(dicom::DataSet::value_type(it->first,it->second));
		}
	}
	catch (dicom::FileMetaInfoException& e)
	{
		/*
		If FileMetaInformation constructor fails, it probably
		means we're opening a KODAK file that doesn't have meta info.
		(this is a violation of the DICOM standard.)
		Against my better judgement I've been asked to make
		the code try and recover at this point.  I can't bring
		myself to, but if you really need it, remove the following
		re-throw command
		*/
		//throw;
		In.seekg(0);
	}
	dicom::TS ts(TransferSyntaxUID);
	/*
	if MetaInformation constructs correctly, then
	In should be pointing at the correct point in the
	file to start reading the DataSet.
	*/
	size_t BytesToRead=GetStreamSize(In)-In.tellg();
	int ByteOrder=ts.isBigEndian()?__BIG_ENDIAN:__LITTLE_ENDIAN;
	dicom::Buffer buffer(ByteOrder);
	//Read data from stream onto buffer.
	buffer.assign(BytesToRead,0);
	unsigned char* pData=&buffer.front();;
	In.read((char*)pData,BytesToRead);
	//Transfer data from buffer onto dataset
	dicom::ReadFromBuffer(buffer,data,ts);
	return ts.isBigEndian();
}
BOOL CdcmviewerDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if(theApp.is_text_file(lpszPathName))
	{
		if(this->GetDocTemplate()!=theApp.m_pEditDocTemplate)
		{
			theApp.OpenDocumentFile(lpszPathName);
			CMainFrame* pframe = (CMainFrame*)AfxGetMainWnd();
			CChildFrame* pMDIActive = (CChildFrame*)pframe->MDIGetActive();
			pMDIActive->SetTitle(lpszPathName);
			pMDIActive->SetWindowText(lpszPathName);
			return FALSE;
		}
		//return TRUE;
	}
	if (!CDocument::OnOpenDocument(lpszPathName) || NULL==lpszPathName ||_tcslen(lpszPathName)==0)
		return FALSE;
	try
	{
		if(!theApp.is_text_file(lpszPathName))
		{
			std::ifstream In(lpszPathName,std::ios::binary);	
			m_bIsBigEndian =::ReadFromStream(In,m_dataset); //ambiguous call to overloaded function, so use global namespace
			dicom::Tag tag_PhotometricInterpretation = dicom::makeTag(0x28,0x4);
			dicom::DataSet::const_iterator it = m_dataset.find(tag_PhotometricInterpretation);
			if(it!=m_dataset.end())
			{
				m_photometric_interpretation = it->second.Get<std::string>();
			}
		dicom::DataSet& data = m_dataset;
		m_bits_allocated = data.operator()(dicom::TAG_BITS_ALLOC).Get<unsigned short>();
		m_bits_stored = data.operator()(dicom::TAG_BITS_STORED).Get<unsigned short>();
		m_high_bit = data.operator()(dicom::TAG_HIGH_BIT).Get<unsigned short>();
		//Pixel Representation==1: signed image
		unsigned short pixel_represent = data.operator()(dicom::TAG_PX_REPRESENT).Get<unsigned short>();
		m_signed_image = (pixel_represent==1);
		m_rows = data.operator()(dicom::TAG_ROWS).Get<unsigned short>();
		m_cols = data.operator()(dicom::TAG_COLUMNS).Get<unsigned short>();

		}
		SetTitle(lpszPathName);
	}
	catch(dicom::exception e)
	{
		std::string msg = "Error openning file:";
		msg += lpszPathName;
		msg += " :   ";
		msg += e.what();
		//AfxMessageBox(msg.c_str());
		m_error = msg;
	}
	catch(...)
	{
		CString text;
		text.Format(_T("failed to open file:%s"),lpszPathName);
		//AfxMessageBox(text);
	}

	return TRUE;
}

void CdcmviewerDoc::OnViewHistogram()
{
	if(m_dataset.size()>0 && m_rows>0 && m_cols>0)
	{
		CMainFrame* pframe = (CMainFrame*)AfxGetMainWnd();
		pframe->CreateOrActivateFrame(((CdcmviewerApp*)AfxGetApp())->m_pHistDocTemplate,RUNTIME_CLASS(HistogramView)); 
		CChildFrame* pMDIActive = (CChildFrame*)pframe->MDIGetActive();
		HistogramView *pview = (HistogramView*)pMDIActive->GetActiveView();
		pview->m_rows = m_rows;
		pview->m_cols = m_cols;
		pview->m_bits_stored = m_bits_stored;
		pview->m_signed_image = m_signed_image;
		pview->m_frame_index = m_frame_index;
		CString title;
		title.Format(_T("Histogram:%s"),pMDIActive->GetActiveDocument()->GetTitle());
		pMDIActive->SetTitle(title);
	}
}

void CdcmviewerDoc::OnViewDicomHeader()
{
	if(m_dataset.size()>0)
	{
		CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
		pFrame->OnViewDicomHeader();
	}
}

void CdcmviewerDoc::OnViewImage()
{	
	POSITION pos = this->GetFirstViewPosition();
	while(NULL != pos)
	{
		CView* pView = this->GetNextView(pos);
		if(pView->IsKindOf(RUNTIME_CLASS(CdcmviewerView)))
		{
			pView->GetParentFrame()->ActivateFrame();
			return;
		}
	}
	//create a new image view
	if(m_dataset.size()>0)
	{
		CMainFrame *pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
		pMainFrame->CreateOrActivateFrame(this->GetDocTemplate(),RUNTIME_CLASS(CdcmviewerView));
		CChildFrame* pMDIActive = (CChildFrame*)pMainFrame->MDIGetActive();
		CString title;
		title.Format(_T("ImageView:%s"),GetTitle());
		pMDIActive->SetTitle(title);
	}
}
BOOL CdcmviewerDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// save DICOM header as text
	if(!theApp.is_text_file(lpszPathName))
		return FALSE;
	//save contents of CEditView when we are opening an text file
	//CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	//if(pMain)
	//{
	//	CFrameWnd* pFrame = pMain->GetActiveFrame();
	//	if(pFrame)
	//	{
	//		CView* pView = pFrame->GetActiveView();
	//		if(pView && pView->IsKindOf(RUNTIME_CLASS(CEditView)))
	//		{
	//			if(theApp.is_text_file(lpszPathName))
	//			{
	//				CEditView* peditview = (CEditView*)pView;
	//				CString content;
	//				peditview->GetWindowText(content);
	//				std::ofstream save_to_file(lpszPathName);
	//				save_to_file<<(LPCTSTR)content;
	//				save_to_file.close();
	//				return TRUE;
	//			}
	//		}
	//	}
	//}
	return CDocument::OnSaveDocument(lpszPathName);
}

void CdcmviewerDoc::OnFileSaveAs()
{
	// save content of CEditView to a text file.
	// DICOM viewer will handle FileSaveAs command in its view class.
	   CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	   if(pMain)
	   {
	   	CFrameWnd* pFrame = pMain->GetActiveFrame();
	   	if(pFrame)
	   	{
	   		CView* pView = pFrame->GetActiveView();
	   		if(pView && pView->IsKindOf(RUNTIME_CLASS(CEditView)))
	   		{
				OPENFILENAME ofn;
				TCHAR szFileName[MAX_PATH + 1024] = _T("");
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn); 
				ofn.hwndOwner = NULL;
				ofn.lpstrFilter = _T("Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0");
				ofn.lpstrFile = szFileName;
				ofn.nMaxFile = MAX_PATH;
				ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
				ofn.lpstrDefExt = _T("txt");
				if(GetSaveFileName(&ofn))
				{
	   				CEditView* peditview = (CEditView*)pView;
	   				CString content;
	   				peditview->GetWindowText(content);
	   				std::ofstream save_to_file(szFileName);
	   				save_to_file<<(LPCTSTR)content;
	   				save_to_file.close();
	   			}
	   		}
	   	}
	   }
}
