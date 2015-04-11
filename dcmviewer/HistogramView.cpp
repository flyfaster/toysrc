// HistogramView.cpp : implementation file
//

#include "stdafx.h"
#include "dcmviewer.h"
#include "HistogramView.h"
#include "dcmviewerDoc.h"
#include "lut_creator.h"
// HistogramView

IMPLEMENT_DYNCREATE(HistogramView, CView)

HistogramView::HistogramView()://m_pixel_data(NULL),
								m_rows(0),
								m_cols(0),
								m_bits_stored(0),
								m_signed_image(false),
								m_min(32767),
								m_max(-32768),
								m_hline_offset(0),
								m_vline_offset(0)
{
}

HistogramView::~HistogramView()
{
}

BEGIN_MESSAGE_MAP(HistogramView, CView)
	ON_WM_KEYDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()


// HistogramView drawing
void get_pixel_padding(dicom::DataSet &data,int& pixel_padding_value,bool& exist_padding)
{
	dicom::Tag tag_pixel_padding = dicom::makeTag(0x28,0x120);
	dicom::DataSet::const_iterator it_padding = data.find(tag_pixel_padding);
	if(it_padding != data.end())
	{
		if(it_padding->second.vr() == dicom::VR_US)
		{
			pixel_padding_value = it_padding->second.Get<UINT16>();
		}
		else
			if(it_padding->second.vr() == dicom::VR_SS)
				pixel_padding_value = it_padding->second.Get<INT16>();
		exist_padding = true;
	}
}

void update_min_max(int& m_min,int& m_max, int p)
{
	if(p<m_min)
		m_min = p;
	else
		if(p>m_max)
			m_max = p;
}

void HistogramView::OnDraw(CDC* pDC)
{
	int x =0;
	int y = 0;
	dicom::DataSet &data = GetDocument()->m_dataset;
	dicom::DataSet::const_iterator it_pixel = data.find(dicom::TAG_PIXEL_DATA);
	//check if the dataset contains valid pixels
	if (!(get_bmp_width()>0 && get_bmp_height()>0 && data.size()>0 && it_pixel !=data.end()))
	{
		pDC->TextOut(x,y,"(null)");
		return;
	}
	bool exist_padding = false;
	int pixel_padding_value = 0;
	get_pixel_padding(data,pixel_padding_value,exist_padding);
	int pixel_range_count = (int)pow(2.0,m_bits_stored);
	int max_pixel_count = 0;
	//std::auto_ptr<int> histogram(new int[pixel_range_count]);
	//int *phistogram = histogram.get();
	//memset(phistogram,0,(size_t)pow(2,m_bits_stored));
	int phistogram[65536];
	memset(phistogram,0,sizeof(phistogram));
	int total_pixels = m_rows*m_cols;
	if(m_bits_stored==8)
	{
		const UINT8* pixel_data = NULL;
		pixel_data = get_pixel_data<UINT8>(data);
		pixel_data += m_frame_index * total_pixels; // for multiframe image
		for(int i=0;i<total_pixels;i++)
		{
			UINT8 p = pixel_data[i];
			phistogram[p] += 1;
			update_min_max(m_min,m_max,p);
		}
	}
	else
	{
		if(m_signed_image)
		{
			const INT16* pixel_data = NULL;
			pixel_data = get_pixel_data<INT16>(data);
			pixel_data += m_frame_index * total_pixels; // for multiframe image
			int offset = pow(2.0,m_bits_stored-1);
			for(int i=0;i<total_pixels;i++)
			{
				INT16 p = pixel_data[i];
				if( p==pixel_padding_value && exist_padding)
					continue;
				int index =  p& 0xffff;
				phistogram[index] += 1;
				update_min_max(m_min,m_max,p);
			}

		}
		else
		{
			const UINT16* pixel_data = NULL;
			pixel_data = get_pixel_data<UINT16>(data);
			pixel_data += m_frame_index * total_pixels; // for multiframe image
			for(int i=0;i<total_pixels;i++)
			{
				UINT16 p = pixel_data[i];
				if( p==pixel_padding_value && exist_padding)
					continue;
				phistogram[p] += 1;
				update_min_max(m_min,m_max,p);
			}

		}
	}
	//to strip padding value
	if(phistogram[m_min&0xffff]>phistogram[m_max&0xffff])
		phistogram[m_min&0xffff] = 0;
	else
		phistogram[m_max&0xffff] = 0;
	if(exist_padding)
	{
		phistogram[pixel_padding_value & 0xffff ] = 0; //sometimes pixel_padding_value = -32768
		max_pixel_count = 0;
		for(int i=0;i<sizeof(phistogram)/sizeof(phistogram[0]);i++)
		{
			if(phistogram[i]>max_pixel_count)
				max_pixel_count = phistogram[i];
		}
	}
	else
	{
		//int next_most_count = 0;
		int max_pixel_val = 0;
		max_pixel_count = 0;
		for(int i=0;i<sizeof(phistogram)/sizeof(phistogram[0]);i++)
		{
			if(phistogram[i]>max_pixel_count)
			{
				//next_most_count = max_pixel_count;
				max_pixel_count = phistogram[i];
				max_pixel_val = i;
			}
		}
	}
	CRect rc_hist;
	GetClientRect(&rc_hist);
	CRect rc_scr;
	GetClientRect(&rc_scr);

	CDC memdc;
	memdc.CreateCompatibleDC(pDC);
	memdc.SetTextColor(theApp.m_font_color);
	memdc.SetBkMode(TRANSPARENT);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC,rc_scr.Width(),rc_scr.Height());
	TEXTMETRIC tm;
	memdc.GetTextMetrics(&tm);
	int line_height = tm.tmHeight + tm.tmExternalLeading;
	CSize left_margin = memdc.GetTextExtent("123");
	rc_hist.DeflateRect(24,line_height+4,24,line_height+4);
	if(rc_hist.Width()>m_max-m_min)
	{
		rc_hist.right = rc_hist.left + m_max - m_min;
	}
	CPen pen_hist;
	pen_hist.CreatePen(PS_SOLID,1,theApp.m_font_color);
	HGDIOBJ oldpen = memdc.SelectObject(&pen_hist);
	HGDIOBJ oldbmp = memdc.SelectObject(&bmp);
	HGDIOBJ oldbrush = memdc.SelectObject(theApp.get_background_brush());
	double bmp_width = rc_hist.Width();
	double bmp_height = rc_hist.Height();
	::FillRect(memdc.m_hDC,&rc_scr,theApp.get_background_brush());
	CRect rg(rc_hist.left, rc_hist.top, rc_hist.right + 1, rc_hist.bottom + 1);
	memdc.Rectangle(&rg);
	CString left_mark;
	left_mark.Format("(%d,%d)",m_min,0);
	CSize mark_size = memdc.GetTextExtent(left_mark);
	memdc.TextOut(rc_hist.left,rc_hist.bottom + 2,left_mark);
	CString mark;
	mark.Format("(%d,%d)",m_max,max_pixel_count);
	mark_size = memdc.GetTextExtent(mark);
	memdc.TextOut(rc_hist.right - mark_size.cx, 2, mark);
	double pixel_value_range = m_max - m_min;
	int index = 0;
	for(int i=m_min;i<=m_max;i++)
	{
		index = i & 0xffff;
		int histval = phistogram[index];
		if(histval>0)
		{
			int line_height = (int)((double)(histval)/(double)max_pixel_count*bmp_height);
			line_height = bmp_height - line_height;
			x = int((double)(i-m_min)/pixel_value_range * bmp_width);
			if(line_height>0)
			{
				memdc.MoveTo(x + rc_hist.left,bmp_height + rc_hist.top);
				memdc.LineTo(x + rc_hist.left,line_height + rc_hist.top);
			}
		}
	}

	int center_pixel_value = m_min + (int)pixel_value_range/2;
	center_pixel_value += m_hline_offset;
	if(center_pixel_value>m_max)
	{
		center_pixel_value = m_max;
		m_hline_offset = pixel_value_range/2;
	}
	if(center_pixel_value<m_min)
	{
		center_pixel_value = m_min;
		m_hline_offset = 0-pixel_value_range/2;
	}
	mark.Format("(x=%d)",center_pixel_value);
	x = int((double)(center_pixel_value-m_min)/pixel_value_range * bmp_width);
	int xmark = x  + rc_hist.left;
	memdc.MoveTo(xmark,rc_hist.top);
	memdc.LineTo(xmark,rc_hist.bottom);
	memdc.TextOut(xmark +1,rc_hist.top+1,mark);
	//
	int histcount = phistogram[center_pixel_value & 0xffff];
	//int line_y = rc_hist.top + rc_hist.Height()/2;
	int line_y = rc_hist.bottom - ( (double)rc_hist.Height()/max_pixel_count * histcount);
	memdc.MoveTo(rc_hist.left,line_y);
	memdc.LineTo(rc_hist.right, line_y);
	mark.Format("(y=%d)",histcount/*max_pixel_count/2*/ );
	memdc.TextOut(rc_hist.left+2, line_y-line_height-2, mark);


	pDC->StretchBlt(rc_scr.left,rc_scr.top,rc_scr.Width(),rc_scr.Height(),&memdc,0,0,rc_scr.Width(),rc_scr.Height(),SRCCOPY);
	memdc.SelectObject(oldbmp);
	memdc.SelectObject(oldpen);
	memdc.SelectObject(oldbrush);
	memdc.DeleteDC();
	bmp.DeleteObject();

}


// HistogramView diagnostics

#ifdef _DEBUG
void HistogramView::AssertValid() const
{
	CView::AssertValid();
}

void HistogramView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG


// HistogramView message handlers

void HistogramView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	CString title;
	title.Format(_T("Histogram:%s"),GetDocument()->GetTitle());
	GetParent()->SetWindowText(title);
}

void HistogramView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	int oldv = m_vline_offset;
	int oldh = m_hline_offset;
	if(VK_UP == nChar)
	{
		m_vline_offset -= nRepCnt;
	}
	if(VK_DOWN == nChar)
	{
		m_vline_offset += nRepCnt;
	}
	if(VK_LEFT == nChar)
		m_hline_offset -= nRepCnt;
	if(VK_RIGHT == nChar)
		m_hline_offset += nRepCnt;
	if(oldv != m_vline_offset || oldh != m_hline_offset)
		Invalidate(FALSE);
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void HistogramView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CView::OnMouseMove(nFlags, point);
	if(MK_RBUTTON &nFlags)
	{
		m_hline_offset += point.x - m_rb_down_pos.x;
		m_rb_down_pos = point;
		Invalidate(FALSE);
	}
}

void HistogramView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CView::OnRButtonDown(nFlags, point);
	if(MK_RBUTTON &nFlags)
		m_rb_down_pos = point;
}
