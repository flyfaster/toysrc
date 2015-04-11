// dcmviewerView.h : interface of the CdcmviewerView class
//


#pragma once
#include "dib.h"
#include <boost/variant.hpp>
//#pragma comment(linker,"/VERBOSE:LIB")
class CdcmviewerView : public CScrollView
{
protected: // create from serialization only
	CdcmviewerView();
	DECLARE_DYNCREATE(CdcmviewerView)

// Attributes
public:
	CdcmviewerDoc* GetDocument() const;

// Operations
public:

// Overrides
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CdcmviewerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	std::auto_ptr<CDib> m_pdib;
	byte m_lut[65536];
// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	bool create_lut();
private:
	UINT16 m_bits_allocated;
	UINT16 m_bits_stored;
	UINT16 m_high_bit;
	double m_ww;
	double m_wc;
	double m_initial_ww;
	double m_initial_wc;
	bool m_signed_image;
	unsigned short m_rows;
	unsigned short m_cols;
	typedef boost::variant<short,unsigned short> US_SS;
	US_SS m_SmallestImagePixelValue;
	US_SS m_LargestImagePixelValue;
	bool m_create_dcm_img_error;
	double get_min()
	{
		if(m_signed_image)
			return boost::get<short>(m_SmallestImagePixelValue);
		else
			return boost::get<unsigned short>(m_SmallestImagePixelValue);
		return m_wc - get_ww()/2;
	}
	double get_ww()
	{
		return m_ww;
	}
	double get_max()
	{
		if(m_signed_image)
			return boost::get<short>(m_LargestImagePixelValue);
		else
			return boost::get<unsigned short>(m_LargestImagePixelValue);
		
		return m_wc + m_ww/2;
	}
	double get_wc()
	{
		return m_wc;
	}
	CPoint m_old_pos;
	int m_frame_index;
	int m_number_of_frame;
	CSize get_lowinch_size();//MM_LOW
	CSize get_himm_size();//MM
	void dcm_to_bmp(CDC *pViewDC,dicom::DataSet &data);
	void print(CDC *pDC);
	void draw_dcm_header(CDC *pdc,dicom::DataSet &data,int x, int &y,int line_height);
	bool m_invert;
	int m_pixel_to_short_table[65536];
	void init_pixel_to_short_table();
	CSize sizeTotal;
	void get_scroll_rect(CRect& sizeobj);
	bool m_fit_to_window;
	//void apply_lut();
	int set_font(CDC *pViewDC, CFont &newfont);
	void print_rect(CRect &rc,CDC *pdc,int x, int &y,int line_height);
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnViewRestore();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnViewWindowLevel();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	afx_msg void OnViewInvert();
	afx_msg void OnUpdateViewInvert(CCmdUI *pCmdUI);
	afx_msg void OnDrawLine();
	afx_msg void OnDrawRect();
	afx_msg void OnDicomSaveAsGrey();
	afx_msg void OnUpdateDicomSaveAsGrey(CCmdUI *pCmdUI);
	afx_msg void OnDicomSaveAsBmp();
	afx_msg void OnFileSaveAs();
	afx_msg void OnFitToWindow();
	afx_msg void OnUpdateFitToWindow(CCmdUI *pCmdUI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
public:
	void SetScrollRange();
	static int xtodc(CDC *pdc,int points);
	static int ytodc(CDC *pdc,int points);
};
struct DCKeeper
{
	DCKeeper(CDC *pdc):m_pdc(pdc)
	{
		if(m_pdc)
		{
			m_pdc->SaveDC();
		}
	}
	~DCKeeper()
	{
		if(m_pdc)
			m_pdc->RestoreDC(-1);
	}
private:
	CDC *m_pdc;
};
CRect UserPage(CDC * pDC, float margin);
void print_rect(CRect &rc,CDC *pdc,int x, int &y,int line_height);



#ifndef _DEBUG  // debug version in dcmviewerView.cpp
inline CdcmviewerDoc* CdcmviewerView::GetDocument() const
   { return reinterpret_cast<CdcmviewerDoc*>(m_pDocument); }
#endif

