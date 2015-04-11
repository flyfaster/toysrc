#pragma once
#include "dib.h"

// HistogramView view
class CdcmviewerDoc;
class HistogramView : public CView
{
	DECLARE_DYNCREATE(HistogramView)

protected:
	HistogramView();           // protected constructor used by dynamic creation
	virtual ~HistogramView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
public:
	//void *m_pixel_data;
	int m_rows;
	int m_cols;
	int m_bits_stored;
	bool m_signed_image;
	int m_min;
	int m_max;
	int m_frame_index;
	//std::auto_ptr<CDib> m_pDib;
	int get_bmp_width()
	{
		return (int)pow( 2.0,m_bits_stored);
	}
	int get_bmp_height()
	{
		return m_rows*m_cols;
	}
	
	CdcmviewerDoc* GetDocument() const
	{ return reinterpret_cast<CdcmviewerDoc*>(m_pDocument); }
private:
	int m_hline_offset;//
	int m_vline_offset;
	CPoint m_rb_down_pos;//right mouse button down positon.
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};


