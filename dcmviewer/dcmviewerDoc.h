// dcmviewerDoc.h : interface of the CdcmviewerDoc class
//


#pragma once

class CdcmviewerDoc : public CDocument
{
protected: // create from serialization only
	CdcmviewerDoc();
	DECLARE_DYNCREATE(CdcmviewerDoc)

// Attributes
public:

// Operations
public:

// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CdcmviewerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	dicom::DataSet m_dataset;
	std::string m_error;
	std::string m_photometric_interpretation;
	bool m_bIsBigEndian;
	afx_msg void OnViewHistogram();
	int m_frame_index;
public:
	UINT16 m_bits_allocated;
	UINT16 m_bits_stored;
	UINT16 m_high_bit;
	//double m_ww;
	//double m_wc;
	//double m_initial_ww;
	//double m_initial_wc;
	bool m_signed_image;
	unsigned short m_rows;
	unsigned short m_cols;

public:
	afx_msg void OnViewDicomHeader();
	afx_msg void OnViewImage();
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	afx_msg void OnFileSaveAs();
};



