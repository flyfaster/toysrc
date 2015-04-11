// dcmviewerView.cpp : implementation of the CdcmviewerView class
//

#include "stdafx.h"
#include "dcmviewer.h"

#include "dcmviewerDoc.h"
#include "dcmviewerView.h"
#include ".\dcmviewerview.h"
#include "indentdumper.h"
#include "lut_creator.h"
#include "windowleveldlg.h"
#include "HistogramView.h"
#include "MainFrm.h"
#include "ChildFrm.h"

#include <Objbase.h>
#include <Gdiplus.h>
#include <GdiPlusEnums.h>
using namespace Gdiplus;
#pragma comment(lib,"gdiplus")
#include "2d_array_operation.h"
#include "timewatcher.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CdcmviewerView

IMPLEMENT_DYNCREATE(CdcmviewerView, CScrollView)

BEGIN_MESSAGE_MAP(CdcmviewerView, CScrollView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_VIEW_RESTORE, OnViewRestore)
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_VIEW_WINDOW_LEVEL, OnViewWindowLevel)
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_VIEW_INVERT, OnViewInvert)
	ON_UPDATE_COMMAND_UI(ID_VIEW_INVERT, OnUpdateViewInvert)
	ON_COMMAND(ID_DRAW_LINE, OnDrawLine)
	ON_COMMAND(ID_DRAW_RECT, OnDrawRect)
	ON_COMMAND(ID_DICOM_SAVE_AS_GREY, OnDicomSaveAsGrey)
	ON_UPDATE_COMMAND_UI(ID_DICOM_SAVE_AS_GREY, OnUpdateDicomSaveAsGrey)
	ON_COMMAND(ID_DICOM_SAVE_AS_BMP, OnFileSaveAs)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_VIEW_FIT_TO_WINDOW, OnFitToWindow)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FIT_TO_WINDOW,OnUpdateFitToWindow)
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CdcmviewerView construction/destruction

CdcmviewerView::CdcmviewerView():m_bits_allocated(0),m_old_pos(-1,-1),m_ww(0),m_frame_index(0)
	,m_number_of_frame(1),m_signed_image(false),m_rows(0),m_cols(0)
	,m_initial_wc(0),m_initial_ww(0)
	,m_invert(false)
	,m_fit_to_window(false)
{
	// TODO: add construction code here
	
}

CdcmviewerView::~CdcmviewerView()
{
}


void CdcmviewerView::init_pixel_to_short_table()
{
	int pos_mask = 0xffff;
	int neg_mask = 0xffff;
	int sign_mask = 0;
	switch(m_bits_stored)
	{
	case 14:
		pos_mask = 0x3fff;
		sign_mask = 0x2000;
		break;
	case 12:
		pos_mask = 0xfff;
		sign_mask = 0x800;
		break;
	case 10:
		pos_mask = 0x3ff;
		sign_mask = 0x200;
		break;
	case 8:
		pos_mask = 0xff;
		sign_mask = 0x8f;
		break;
	case 16:
	default:
		pos_mask = 0xffff;
		neg_mask = 0;
		sign_mask = 0x8000;
		break;
	}
	neg_mask = 0xffffffff - pos_mask; // data | neg_mask;  data & pos_mask , data & sign_mask
	if(m_bits_stored == 8)
		neg_mask = 0;
	for(int i=0;i<65536;i++)
	{
		if(m_signed_image)
		{
			if( i & sign_mask) //negative
			{
				m_pixel_to_short_table[i] = i | neg_mask;
			}
			else //positive
			{				
				m_pixel_to_short_table[i] = i & pos_mask;
			}
		}
		else
		{
			m_pixel_to_short_table[i] = i & pos_mask;
		}
	}
}

//void CdcmviewerView::apply_lut()
//{
//	//try to use Modality LUT Sequence.
//	dicom::DataSet &data = GetDocument()->m_dataset;
//	dicom::DataSet &ds = GetDocument()->m_dataset;
//
//	dicom::Tag tag_modality_lut_sequence = dicom::makeTag(0x28,0x3000);
//	dicom::DataSet::const_iterator modality_lut_it = data.find(tag_modality_lut_sequence);
//	if(modality_lut_it!=data.end())
//	{
//		dicom::Sequence modality_lut = data.operator()(tag_modality_lut_sequence).Get<dicom::Sequence>();
//		if(modality_lut.size()>0)
//		{
//			const dicom::Tag tag_lut_descriptor = dicom::makeTag(0x28,0x3002);
//			std::pair<dicom::DataSet::const_iterator,dicom::DataSet::const_iterator> LUTDescriptors = ds.equal_range(tag_lut_descriptor);
//			dicom::DataSet::const_iterator first = LUTDescriptors.first;
//			UINT16 entry_count = first->second.Get<UINT16>();
//			first++;
//			UINT16 pixel_first_mapped_tmp = 0;
//			pixel_first_mapped_tmp = first->second.Get<UINT16>();
//			first++;
//			UINT16 lut_bits_stored = first->second.Get<UINT16>();
//			UINT16 pixel_first_mapped = (UINT16)pixel_first_mapped_tmp;
//			//(0028,3006)  SS/US  LUTData
//			const dicom::Tag tag_LUTData = dicom::makeTag(0x28,0x3006);
//			std::pair<dicom::DataSet::const_iterator,dicom::DataSet::const_iterator> LUTData = ds.equal_range(tag_LUTData);
//			dicom::DataSet::const_iterator lutdata_first = LUTData.first;
//			UINT16 first_lut_value = lutdata_first->second.Get<UINT16>();
//			UINT16 output = first_lut_value;
//
//			//for(int i=0;i<pixel_first_mapped_tmp;i++)
//			//{
//			//	int d = m_pixel_to_short_table[i];
//			//		m_pixel_to_short_table[i] = first_lut_value;
//			//}
//			////for pixel with entry in modality lut
//			//for(;lutdata_first!=LUTData.second;lutdata_first++)
//			//{
//			//	output = lutdata_first->second.Get<TYPE_LUT>();
//			//	bgray = get_gray(output,min,window_width);
//			//	index = (int(i) )& 0xffff;
//			//	m_lut[i] = bgray;
//			//	i++;
//			//}
//		}
//	}
//	else
//	{
//		double rescale_intercept = dataset_get_ds(data,dicom::TAG_RESCALE_INTERCEPT,0);
//		double rescale_slope = dataset_get_ds(data,dicom::TAG_RESCALE_SLOPE,1.0);
//		for(int i=0;i<65536;i++)
//		{
//			double d = m_pixel_to_short_table[i];
//			d = d * rescale_slope + rescale_intercept ;
//			m_pixel_to_short_table[i] = (int)d;
//		}
//	}
//}

namespace PhotometricInterpretation
{	//C.7.6.3.1.2 Photometric Interpretation,Part 3: Information Object Definitions

	LPCTSTR MONOCHROME2 = _T("MONOCHROME2");
	LPCTSTR MONOCHROME1 = _T("MONOCHROME2");
	LPCTSTR PALETTE_COLOR = _T("PALETTE_COLOR");
	LPCTSTR RGB = _T("RGB");
	LPCTSTR ARGB = _T("ARGB");
	LPCTSTR CMYK = _T("CMYK");
	LPCTSTR YBR_FULL_422 = _T("YBR Full 422");
	LPCTSTR YBR_FULL = _T("YBR Full");
	LPCTSTR YBR_RCT = _T("YBR_RCT");
	LPCTSTR YBR_ICT = _T("YBR_ICT");
};
namespace PlanarConfiguration
{
	const unsigned short ByPixel = 0;
	const unsigned short ByPlane = 1;
};
bool CdcmviewerView::create_lut()
{
	CdcmviewerDoc* pDoc = GetDocument();
	if(!pDoc)
		return false;
	dicom::DataSet& data = pDoc->m_dataset;
	if(data.size()<1)
		return false;
	std::string photometric_interception = data.operator()(dicom::TAG_PHOTOMETRIC).Get<std::string>();//CS
	TimeWatcher tw(__TFUNCTION__);
	if(0==m_bits_allocated)
	{
		m_bits_allocated = data.operator()(dicom::TAG_BITS_ALLOC).Get<unsigned short>();
		m_bits_stored = data.operator()(dicom::TAG_BITS_STORED).Get<unsigned short>();
		m_high_bit = data.operator()(dicom::TAG_HIGH_BIT).Get<unsigned short>();
		//Pixel Representation==1: signed image
		unsigned short pixel_represent = data.operator()(dicom::TAG_PX_REPRESENT).Get<unsigned short>();
		m_signed_image = (pixel_represent==1);
		m_rows = data.operator()(dicom::TAG_ROWS).Get<unsigned short>();
		m_cols = data.operator()(dicom::TAG_COLUMNS).Get<unsigned short>();
		init_pixel_to_short_table();
		dicom::Tag TAG_SMALLEST_PIXEL_VALUE = dicom::makeTag(0x0028,0x0106); //(0028,0106)  US/SS  SmallestImagePixelValue    0
		dicom::Tag TAG_LARGEST_PIXEL_VALUE = dicom::makeTag(0x0028,0x0107); //(0028,0107)  US/SS  LargestImagePixelValue    1966
		bool min_max_exists = false;
		if(data.find(TAG_LARGEST_PIXEL_VALUE) != data.end())
		{
			min_max_exists = true;
			if(m_signed_image)
			{
				m_SmallestImagePixelValue = data.find(TAG_SMALLEST_PIXEL_VALUE)->second.Get<short>();
				m_LargestImagePixelValue = data.find(TAG_LARGEST_PIXEL_VALUE)->second.Get<short>();
			}
			else
			{
				m_SmallestImagePixelValue = data.find(TAG_SMALLEST_PIXEL_VALUE)->second.Get<unsigned short>();
				m_LargestImagePixelValue = data.find(TAG_LARGEST_PIXEL_VALUE)->second.Get<unsigned short>();
			}
		}

		if(data.find(dicom::TAG_WINDOW_CENTER) == data.end())
		{
			//guess wc/ww
			bool exist_padding = false;
			dicom::Tag tag_pixel_padding = dicom::makeTag(0x28,0x120);
			dicom::DataSet::const_iterator it_padding = data.find(tag_pixel_padding);
			int pixel_padding_value = 0;
			if(it_padding != data.end())
			{
				if(it_padding->second.vr() == dicom::VR_US)
				{
					pixel_padding_value = it_padding->second.Get<UINT16>();
				}
				else
					if(it_padding->second.vr() == dicom::VR_SS)
						pixel_padding_value = it_padding->second.Get<INT16>();
			}
			int m_min = 65535;
			int m_max = -32768;
			if(!min_max_exists)
			{
				int total_pixels = m_rows*m_cols;
				if(m_bits_stored == 8)
				{
					const UINT8* pixel_data = get_pixel_data<UINT8>(data);
					if(exist_padding)
						find_min_max_ignore_padding<UINT8>(pixel_data,total_pixels,m_min,m_max,pixel_padding_value,m_pixel_to_short_table);
					else
						find_min_max<UINT8>(pixel_data,total_pixels,m_min,m_max);
				}
				else
				{
					if(m_signed_image)
					{
						const INT16* pixel_data = get_pixel_data<INT16>(data);
						if(exist_padding)
							find_min_max_ignore_padding<INT16>(pixel_data,total_pixels,m_min,m_max,pixel_padding_value,m_pixel_to_short_table);
						else
							find_min_max<INT16>(pixel_data,total_pixels,m_min,m_max);
					}
					else
					{
						const UINT16* pixel_data = get_pixel_data<UINT16>(data);
						if(exist_padding)
							find_min_max_ignore_padding<UINT16>(pixel_data,total_pixels,m_min,m_max,pixel_padding_value,m_pixel_to_short_table);
						else
							find_min_max<UINT16>(pixel_data,total_pixels,m_min,m_max);
					}
					OnegaDebug::Dump(_T("min:%d, max:%d"),m_min, m_max);
				}
			}
			else
			{
				if(m_signed_image)
				{
					m_max = get_max();
					m_min = get_min();
				}
			}
			double rescale_intercept = dataset_get_ds(data,dicom::TAG_RESCALE_INTERCEPT,0);
			double rescale_slope = dataset_get_ds(data,dicom::TAG_RESCALE_SLOPE,1.0);
			m_max = m_max * rescale_slope + rescale_intercept;
			m_min = m_min * rescale_slope + rescale_intercept;
			m_wc = (m_max +m_min)/2;
			m_ww = m_max - m_min;
			for(int i=0;i<sizeof(m_pixel_to_short_table)/sizeof(m_pixel_to_short_table[0]);i++)
			{
				double d = m_pixel_to_short_table[i];
				d = d * rescale_slope + rescale_intercept;
				m_pixel_to_short_table[i] = d;
			}
		}
		else
		{
			m_wc = dataset_get_ds(data,dicom::TAG_WINDOW_CENTER,pow(2.0,m_bits_stored-1)) /*s2i(swindow_center)*/;
			m_ww = dataset_get_ds(data,dicom::TAG_WINDOW_WIDTH,pow(2.0,m_bits_stored)) /*s2i(swindow_width)*/;
			OnegaDebug::Dump(_T("ww:%f, WC:%f"), m_ww,m_wc);
		}
		m_initial_ww = m_ww;
		m_initial_wc = m_wc;
		//
		dicom::Tag tag_number_of_frame = dicom::makeTag(0x0028,0x0008);
		dicom::DataSet::const_iterator it_number_of_frame = data.find(tag_number_of_frame);
		if(it_number_of_frame!=data.end())
		{
			std::string stmp = it_number_of_frame->second.Get<std::string>();
			m_number_of_frame = atoi(stmp.c_str());
		}
	}
	typedef std::string DS_TYPE ;
	typedef std::string LO_TYPE;
	//unsigned short bits_stored = data.operator()(dicom::TAG_BITS_STORED).Get<unsigned short>();
	// 8, 10, 12, 16 grayscale and 24 bit images 
	// JPEG (8, 10, 12, 16-bit Lossless grayscale, 8, 12 bit lossy grayscale, 24-bit lossless and lossy color), 
	//JPEG 2000 (including lossy and lossless 8-16 bit grayscale images and 24-bit color) Run Length, Uncompressed, 
	//Planar Configuration Both possible values (color-by-plane, color-by-pixel).
	double window_center = get_wc();
	double window_width = get_ww();
	double min = get_min();
	double max = get_max();
	if(photometric_interception!= PhotometricInterpretation::MONOCHROME1 
		&& photometric_interception!= PhotometricInterpretation::MONOCHROME2)
		return false;

	//try to use Modality LUT Sequence.
	dicom::Tag tag_modality_lut_sequence = dicom::makeTag(0x28,0x3000);
	dicom::DataSet::const_iterator modality_lut_it = data.find(tag_modality_lut_sequence);
	if(modality_lut_it!=data.end())
	{
		dicom::Sequence modality_lut = data.operator()(tag_modality_lut_sequence).Get<dicom::Sequence>();
		if(modality_lut.size()>0)
		{
			dicom::DataSet &ds = modality_lut[0];
			if( 8 == m_bits_allocated )
			{
				create_lut_from_modality_lut<UINT16,UINT8>(ds,min,window_width,m_lut);
			}
			else
			if(m_signed_image)
			{
				create_lut_from_modality_lut<INT16,INT16>(ds,min,window_width,m_lut);
			}
			else
			{
				create_lut_from_modality_lut<UINT16,UINT16>(ds,min,window_width,m_lut);
			}			
		}
	}
	else
	{
		double rescale_intercept = dataset_get_ds(data,dicom::TAG_RESCALE_INTERCEPT,0);
		double rescale_slope = dataset_get_ds(data,dicom::TAG_RESCALE_SLOPE,1.0);
		if(!m_signed_image)//unsigned image
		{
			create_linear_lut<UINT16>(rescale_slope,rescale_intercept,m_lut,min,window_width);
		}
		else
		{
			create_linear_lut<INT16>(rescale_slope,rescale_intercept,m_lut,min,window_width);
		}
	}
	//and single and multi-frame images.
	if( (photometric_interception== PhotometricInterpretation::MONOCHROME1) ^ (m_invert) ) //P3 C.7.6.3.1.2 The minimum sample value is intended to be displayed as white
	{
		for(int i=0;i<65536;i++)
		{
			m_lut[i] = 255-m_lut[i];
		}
	}	
	return true;
}

int CdcmviewerView::xtodc(CDC *pdc,int points)
{
	CSize size;
	int perinch = pdc->GetDeviceCaps(LOGPIXELSX);
	size.cx = size.cy = (perinch*points)/72;
	pdc->DPtoLP(&size);
	return size.cx;
}

int CdcmviewerView::ytodc(CDC *pdc,int points)
{
	CSize size;
	int perinch = pdc->GetDeviceCaps(LOGPIXELSY);
	size.cx = size.cy = (perinch*points)/72;
	pdc->DPtoLP(&size);
	return size.cy;
}

void CdcmviewerView::dcm_to_bmp(CDC *pViewDC,dicom::DataSet &data)
{//convert DICOM pixel data to m_pdib in original size
	if(!m_pdib.get())
	{
		CDC dcCompat;
		dcCompat.CreateCompatibleDC(pViewDC);
		m_pdib.reset(new CDib(CSize(m_cols,m_rows),24)) ; // size of the bitmap
		VERIFY(m_pdib->CreateSection(&dcCompat));	 // allocate memory for bitmap
		dcCompat.DeleteDC();
	}
	typedef std::string DS_TYPE ;
	typedef std::string LO_TYPE;
	typedef	std::vector<UINT16>	OW_TYPE;
	UINT16 sample_per_pixel = 1;
	dicom::Tag tag_sample_per_pixel = dicom::makeTag(0x28,0002);
	dicom::DataSet::const_iterator it_number_of_frame = data.find(tag_sample_per_pixel);
	if(it_number_of_frame!=data.end())
	{
		sample_per_pixel = it_number_of_frame->second.Get<UINT16>();
	}
	int pixel_number = m_rows * m_cols;

	{
		//TimeWatcher tw("create bitmap from pixels");
		//create bitmap from dcm pixels
		BYTE* dibits = m_pdib->m_lpImage;
		int index = 0;
		byte gray_value;
		const UINT8 * pixel_segment = get_pixel_data<UINT8>(data);
		if( 8 == m_bits_allocated ) 
		{
			const UINT8 *pixel_data_pointer = reinterpret_cast<const UINT8*>(pixel_segment);
			if(m_number_of_frame>1)
				pixel_data_pointer += m_frame_index * sample_per_pixel * m_rows *m_cols; // for multiframe image
			//process RGB image
			dicom::Tag tag_PhotometricInterpretation = dicom::makeTag(0x28,0x4);
			dicom::DataSet::const_iterator it = data.find(tag_PhotometricInterpretation);
			if(it!=data.end())
			{
				std::string photometric_interpretation = it->second.Get<std::string>();
				if(photometric_interpretation == PhotometricInterpretation::RGB)
				{ 
					dicom::Tag tagPlanarConfiguration = dicom::makeTag(0x28,0x06);
					//0 = The sample values for the first pixel are followed by the sample values for the second pixel
					//1 = Each color plane shall be sent contiguously.
					UINT16 usPlanarConfiguration = PlanarConfiguration::ByPixel;
					dicom::DataSet::const_iterator itPlanarConfiguration = data.find(tagPlanarConfiguration);
					if(itPlanarConfiguration != data.end() )
					{
						usPlanarConfiguration = itPlanarConfiguration->second.Get<UINT16>();
					}
					if( PlanarConfiguration::ByPixel == usPlanarConfiguration )
					{
						memcpy(dibits,pixel_data_pointer,pixel_number*sample_per_pixel);
						//the bmp seems to be headstand/ stand upside down/ handstand
						if(!this->GetDocument()->m_bIsBigEndian)
						{
							//exchane r and b
							byte tmp = 0;
							for(int i=0;i<pixel_number*3;i=i+3)
							{
								tmp = dibits[i];
								dibits[i] = dibits[i+2];
								dibits[i+2] = tmp;
							}
						}
					}
					else
					{
						int roffset = 0;
						int goffset = pixel_number;
						int boffset = pixel_number+pixel_number;
						//if(this->GetDocument()->m_bIsBigEndian)
						//{
						//	//exchane r and b
						//	roffset = pixel_number+pixel_number;
						//	boffset = 0;
						//}

						for(int i=0;i<pixel_number;i++)
						{
							*(dibits++) = pixel_data_pointer[boffset+i]; // blue
							*(dibits++) = pixel_data_pointer[goffset+i]; //green
							*(dibits++) = pixel_data_pointer[roffset+i]; //red
							//*dibits++ = 0; //blue
							//*dibits++ = 0; //green
							//*dibits++ = 255; //red
						}
					}
					flip_array(m_pdib->m_lpImage,m_rows,m_cols,3);
					return;
				}
			}
			for(index = 0;index<pixel_number;index++)
			{
				gray_value = m_lut[pixel_data_pointer[index]];
				*(dibits++) = gray_value;
				*(dibits++) = gray_value;
				*(dibits++) = gray_value;
			}
		}
		else
			if(16 == m_bits_allocated)
		{
			const UINT16 *pixel_data_pointer = reinterpret_cast<const UINT16*>(pixel_segment);
			if(m_number_of_frame>1)
				pixel_data_pointer += m_frame_index * sample_per_pixel * m_rows *m_cols;// for multiframe image
			int pixel_mask = 0xffff;
			switch(m_bits_allocated) {
				case 8:
					pixel_mask = 0xff;
					break;
			case 10:
				pixel_mask = 0x3ff;
				break;
			case 12:
				pixel_mask = 0xfff;
				break;
			case 14:
				pixel_mask = 0x3fff;
				break;
			default:
				pixel_mask = 0xffff;
				break;
			}
			if(pixel_mask!=0xffff)
			{
				for(index = 0;index<pixel_number;index++)
				{
					const UINT16 &pixel_raw = pixel_data_pointer[index];	
					gray_value = m_lut[pixel_raw & pixel_mask];
					*(dibits++) = gray_value;
					*(dibits++) = gray_value;
					*(dibits++) = gray_value;
				}
			}
			else
			{//save the time of masking high bits
				for(index = 0;index<pixel_number;index++)
				{
					const UINT16 &pixel_raw = pixel_data_pointer[index];	
					gray_value = m_lut[pixel_raw];
					*(dibits++) = gray_value;
					*(dibits++) = gray_value;
					*(dibits++) = gray_value;
				}
			}
		}
		//the bmp seems to be headstand/ stand upside down/ handstand
		flip_array(m_pdib->m_lpImage,m_rows,m_cols,3);
	}
}
void CdcmviewerView::print_rect(CRect &rc,CDC *pdc,int x, int &y,int line_height)
{
	CString msg;
	msg.Format("rect :(%d,%d,%d,%d)",rc.left,rc.top,rc.right,rc.bottom);
	pdc->TextOut(x,y,msg);
	y += line_height;
}
void CdcmviewerView::draw_dcm_header(CDC *pDC,dicom::DataSet &data,int x, int &y,int line_height)
{
	if(m_create_dcm_img_error)
	{
		CDocument *pDoc = GetDocument();
		if(pDoc)
		{
			CString msg;
			msg.Format( _T("unrecognized file :%s"),pDoc->GetPathName());
			pDC->TextOut(x,y,msg);
		}
		else
		{
			pDC->TextOut(x,y,_T("no file"));
		}
		y+= line_height;
	}
	CString text;
	CdcmviewerDoc* pDoc = (CdcmviewerDoc*)GetDocument();
	if(pDoc && pDoc->m_photometric_interpretation!="RGB")
	{
		text.Format( _T("ww:%f,wc:%f,"),get_ww(),get_wc());
		theApp.DebugOutput(text);
		pDC->TextOut(x,y,text);
		y += line_height;
	}

	if(m_number_of_frame>1)
	{
		text.Format("frame index:%04d,navigate to next frame by key PAGE DOWN/RIGHT ARROW/DOWN ARROW",m_frame_index);
		pDC->TextOut(x,y,text);
		y += line_height;
	}
	std::stringstream oss;
	for(dicom::DataSet::const_iterator it=data.begin();
		it!=data.end();
		it++)
	{
		dicom::Tag tag = it->first;
		if(GroupTag(tag)==0x28 || /*GroupTag(tag) == 0x08 ||*/ GroupTag(tag)==0x10)
		{
			oss.str("");
			IndentDumper::write_tag(oss,tag,0);
			oss<<it->second;
			pDC->TextOut(x,y,oss.str().c_str());
			y += line_height;
		}
	}
}

int CdcmviewerView::set_font(CDC *pViewDC, CFont &newfont)
{
	//input newfont should be uninitialized , I'll create it inside the function
	//return line_height
	if(theApp.get_logfont())
	{
		LOGFONT lf;
		memcpy(&lf,theApp.get_logfont(),sizeof(LOGFONT));
		lf.lfHeight = -ytodc(pViewDC,lf.lfHeight); // for printing
		newfont.CreateFontIndirect(&lf);
		pViewDC->SelectObject( &newfont);
	}
	TEXTMETRIC textMetric;
	pViewDC->GetTextMetrics(&textMetric);
	int line_height = textMetric.tmHeight+ textMetric.tmExternalLeading;
	if(  line_height>0 && pViewDC->GetMapMode()!=MM_TEXT)
		line_height = 0 - line_height;
	return line_height;
}
void CdcmviewerView::print(CDC *pViewDC)
{
	if(pViewDC->IsPrinting()==false)
		return;
	dicom::DataSet& data = GetDocument()->m_dataset;
	pViewDC->SetMapMode(MM_TWIPS);
	int map_mode = pViewDC->GetMapMode();
	CFont newfont;
	int line_height = 12;
	int y = 0;
	pViewDC->SetTextColor( theApp.m_font_color);
	pViewDC->SetBkMode(TRANSPARENT);
	line_height = set_font(pViewDC,newfont);
	//make image display fit to page.
	CRect rect_page = UserPage(pViewDC,0.5);
	CSize dcm_true_size_on_dc(xtodc(pViewDC,m_cols),ytodc(pViewDC,m_rows));
	double fx = ((double)rect_page.Width())/dcm_true_size_on_dc.cx;
	double fy = ((double)rect_page.Height())/dcm_true_size_on_dc.cy;
	double factor = fx<fy?fx:fy;
	int mx = dcm_true_size_on_dc.cx*factor;
	int my = dcm_true_size_on_dc.cy*factor;	
	CRect rect_img(0,0,mx,my);
	if(factor>1) //zoom out image to fit into the view
	{
		rect_img.right = dcm_true_size_on_dc.cx;
		rect_img.bottom = dcm_true_size_on_dc.cy;
		mx = rect_img.right;
		my = rect_img.bottom;
	}
	rect_img.MoveToXY(rect_page.left,rect_page.top);
	TimeWatcher tw(__TFUNCTION__);
	dcm_to_bmp(pViewDC,data);
	int x = rect_img.right;
	y = -rect_page.top;	
	draw_dcm_header(pViewDC,data,x,y,line_height);// y was changed by draw_dcm_header
	print_rect(rect_page,pViewDC,x,y,line_height);

	y = -rect_page.top;	
	x = rect_page.left;
	m_pdib->Draw(pViewDC,CPoint(x,y),CSize( mx, -my));	
}

void CdcmviewerView::OnDraw(CDC* pViewDC)
{
		CdcmviewerDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		DCKeeper dc_restore_point(pViewDC);
		int y = 0;
		int line_height = 12;
		CRect rect_view;
		//GetClientRect(&rect_view);
		//::FillRect(pViewDC->m_hDC,&rect_view,theApp.get_background_brush() );

		get_scroll_rect(rect_view);//GetClientRect(&rect_view);
		if (!pDoc || pDoc->GetPathName().GetLength()<1)//No DICOM image file
		{
			pViewDC->SetTextColor( theApp.m_font_color);
			pViewDC->SetBkMode(TRANSPARENT);
			::FillRect(pViewDC->m_hDC,&rect_view,theApp.get_background_brush() );
			pViewDC->TextOut(0,y,"no file opened");
			y+= line_height;
			return;
		}
		dicom::DataSet& data = pDoc->m_dataset;
		if(data.size()<1) //No DICOM dataset
		{
			pViewDC->SetTextColor( theApp.m_font_color);
			pViewDC->SetBkMode(TRANSPARENT);
			::FillRect(pViewDC->m_hDC,&rect_view,theApp.get_background_brush()  );
			if(pDoc->m_error.length()>0)
			{
				pViewDC->TextOut(0,y,pDoc->m_error.c_str());
				y+=line_height;
			}
			else
			{
				CString msg;
				msg.Format("failed to open file :%s",pDoc->GetPathName());
				pViewDC->TextOut(0,y,msg);
				y+= line_height;
			}
			return;
		}
		m_create_dcm_img_error = false;
		try
		{
			create_lut();
		}
		catch(...)
		{
			pViewDC->SetTextColor( theApp.m_font_color);
			pViewDC->SetBkMode(TRANSPARENT);
			::FillRect(pViewDC->m_hDC,&rect_view,theApp.get_background_brush() );
			m_create_dcm_img_error = true;
			//return;
		}
		bool is_printing = pViewDC->IsPrinting();
		if(is_printing)
		{
			print(pViewDC);
			return;
		}
		CRect whole_rect;
		get_scroll_rect(whole_rect);
		::FillRect(pViewDC->m_hDC,&whole_rect,theApp.get_background_brush() );
		{
			//sometimes scroll rect is smaller than client rect
			CRect client_rect;
			GetClientRect(&client_rect);
			::FillRect(pViewDC->m_hDC,&client_rect,theApp.get_background_brush() );
		}

		//set font
		CFont newfont;
		line_height = set_font(pViewDC,newfont);
		//make image display fit to window.
		CRect rect_img;
		int mx = 0;
		int my = 0;
		double factor = 1;
		if(!m_create_dcm_img_error)
		{
			double fx = ((double)rect_view.Width())/m_cols;
			double fy = ((double)rect_view.Height())/m_rows;
			factor = fx<fy?fx:fy;
			if(!m_fit_to_window)
				factor = 1;//now user can scroll the view
			mx = m_cols*factor;
			my = m_rows*factor;	
			rect_img.left = 0;
			rect_img.top = 0;
			rect_img.right = mx;
			rect_img.bottom = my;
			if(factor>1) //zoom out image to fit into the view
			{
				rect_img.right = m_cols;
				rect_img.bottom = m_rows;
				mx = rect_img.right;
				my = rect_img.bottom;
			}
			TimeWatcher tw(__TFUNCTION__);
			dcm_to_bmp(pViewDC,data);

		}
		//draw in memory dc to avoid flicking
		CDC dcMem; // dont use DCKeeper with it , for that I want to control delete order
		dcMem.CreateCompatibleDC(pViewDC);
		CDC *pBufferDC= &dcMem;
		HGDIOBJ poldfont = NULL;
		if(theApp.get_logfont())
			poldfont = pBufferDC->SelectObject( &newfont);
		CBitmap bm_draw;
		bm_draw.CreateCompatibleBitmap(pViewDC,rect_view.Width(),rect_view.Height());
		HGDIOBJ poldbmp = pBufferDC->SelectObject(bm_draw);
		::FillRect(pBufferDC->m_hDC,&rect_view,theApp.get_background_brush() );
		pBufferDC->SetTextColor( theApp.m_font_color);
		pBufferDC->SetBkMode(TRANSPARENT);
		int x = 0;
		if(!m_create_dcm_img_error)
			x = rect_img.Width();
		draw_dcm_header(pBufferDC,data,x,y,line_height);
		//print_rect(rect_view,pBufferDC,x,y,line_height);
		TCHAR buf[256];
		_stprintf(buf,_T("zoom factor:%0.2f"),factor>1?1:factor);
		pBufferDC->TextOut(x,y,buf);
		y += line_height;
		x = 0;
		y = 0;
		if(!m_create_dcm_img_error)
		{
			m_pdib->Draw(pBufferDC,CPoint(x,y),CSize(mx,my));		
		}
		pViewDC->BitBlt(0,0,rect_view.Width(),rect_view.Height(),pBufferDC,0,0,SRCCOPY);
		pBufferDC->SelectObject(poldbmp);
		pBufferDC->SelectObject(poldfont);
		pBufferDC->DeleteDC();
		bm_draw.DeleteObject();
}

// CdcmviewerView printing

BOOL CdcmviewerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	pInfo->SetMaxPage(1);
	return DoPreparePrinting(pInfo);
}

void CdcmviewerView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	//int m_nPageHeight = pDC->GetDeviceCaps (VERTRES);
	//int nDocLength = m_rows;
	//int nMaxPage = max (1, (nDocLength + 
	//	(m_nPageHeight - 1)) /
	//	m_nPageHeight);
	//pi2->pDevMode->dmFields = DM_ORIENTATION;
	//pi2->pDevMode->dmOrientation = DMORIENT_LANDSCAPE;
	pInfo->SetMaxPage (1);

}
CSize CdcmviewerView::get_lowinch_size()
{
	//0028,0030 DS pixel spacing, Physical distance in the patient between
	//the center of each pixel, specified by a
	//	numeric pair - adjacent row spacing
	//	(delimiter) adjacent column spacing in 	mm.
	dicom::Tag tag_pixel_spacing = dicom::makeTag(0x28,0x30);
	CDC dc;
	dc.m_hDC=::GetDC(GetDesktopWindow()->m_hWnd);
	int old_map_mode = dc.SetMapMode(MM_TEXT);
	int xpixel_per_inch = dc.GetDeviceCaps(LOGPIXELSX);
	int ypixel_per_inch = dc.GetDeviceCaps(LOGPIXELSY);
	CSize ret;
	ret.cx = m_cols/xpixel_per_inch;
	ret.cy = m_rows/ypixel_per_inch;
	dc.SetMapMode(old_map_mode);
	return ret;	
}
CSize CdcmviewerView::get_himm_size()
{
	//get image size in MM_TEXT mode
	CDC *pdc=GetDesktopWindow()->GetDC();
	int old_map_mode = pdc->SetMapMode(MM_TEXT);
	int screen_width = pdc->GetDeviceCaps(HORZSIZE);
	int screen_height = pdc->GetDeviceCaps(VERTSIZE);
	int xpixels = pdc->GetDeviceCaps(HORZRES);
	int ypixels = pdc->GetDeviceCaps(VERTRES);
	CSize ret;
	ret.cx = ((double)m_cols/xpixels *screen_width)*1000;
	ret.cy = ((double)m_rows/ypixels *screen_height)*1000;
	pdc->SetMapMode(old_map_mode);
	return ret;
}
void CdcmviewerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CdcmviewerView diagnostics

#ifdef _DEBUG
void CdcmviewerView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CdcmviewerView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CdcmviewerDoc* CdcmviewerView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CdcmviewerDoc)));
	return (CdcmviewerDoc*)m_pDocument;
}
#endif //_DEBUG

void CdcmviewerView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();
	memset(m_lut,0,sizeof(m_lut));
	CString title;
	title.Format(_T("DicomView:%s"),GetDocument()->GetTitle());
	CChildFrame* pMDI = (CChildFrame*)GetParent();
	pMDI->SetTitle(title);
	SetScrollRange();
}
void CdcmviewerView::SetScrollRange()
{
	//set scroll size
	CRect rect_view_size;
	GetClientRect(&rect_view_size);
	if(m_fit_to_window)
	{
		sizeTotal.cx = rect_view_size.Width();
		sizeTotal.cy = rect_view_size.Height();
	}
	else
	{
		sizeTotal.cx=GetDocument()->m_cols;
		sizeTotal.cy = GetDocument()->m_rows;
	}
	SetScrollSizes(MM_TEXT,sizeTotal); 

}
BOOL CdcmviewerView::OnEraseBkgnd(CDC* pDC)
{
	//return __super::OnEraseBkgnd(pDC);
	//CRect rect_view;
	//get_scroll_rect(rect_view);//GetClientRect(&rect_view);
	//::FillRect(pDC->m_hDC,&rect_view,theApp.get_background_brush() );
	return TRUE;
}

void CdcmviewerView::OnMouseMove(UINT nFlags, CPoint point)
{
	//adjust window width/window center
	CScrollView::OnMouseMove(nFlags, point);
	if(MK_RBUTTON &nFlags)
	{
		if(m_old_pos.x == -1 && -1==m_old_pos.y)
		{
			m_old_pos = point;
		}
		else
		{
			m_wc += (point.x-m_old_pos.x)/2;
			m_ww += (point.y - m_old_pos.y)/2;
			if(m_ww<1)
				m_ww = 1;
			if(m_ww>65535)
				m_ww = 65535;
			if(m_wc>65535)
				m_wc = 65535;
			else
				if(m_wc<-32768)
					m_wc = -32768;
			m_old_pos = point;
			Invalidate();
		}
	}
}

void CdcmviewerView::OnViewRestore()
{
	m_ww = m_initial_ww;
	m_wc = m_initial_wc;
	m_invert = false;
	Invalidate();
}

void CdcmviewerView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// remember current position for ww/wc adjustment
	m_old_pos = point;
	CScrollView::OnRButtonDown(nFlags, point);
}

void CdcmviewerView::OnViewWindowLevel()
{
	WindowLevelDlg wld;
	wld.m_wc = get_wc();
	wld.m_ww = get_ww();
	wld.DoModal();
	if(wld.is_valid())
	{
		m_wc = wld.m_wc;
		m_ww = wld.m_ww;
		Invalidate();
	}
}

void CdcmviewerView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	//move frame index
	const UINT KEY_PAGE_UP = 33;
	const UINT KEY_PAGE_DOWN = 34;
	if(m_number_of_frame>1)
	{
		if( KEY_PAGE_DOWN==nChar || VK_DOWN==nChar||VK_RIGHT==nChar )//move to next frame
		{
			m_frame_index++;
			if(m_frame_index>=m_number_of_frame)
				m_frame_index = 0;
			Invalidate();
		}
		else
			if(KEY_PAGE_UP==nChar || VK_UP==nChar|| VK_LEFT==nChar)//previous frame
			{
				m_frame_index--;
				if(m_frame_index<0)
					m_frame_index = m_number_of_frame-1;//wrap to last frame
				Invalidate();
			}
	}
	GetDocument()->m_frame_index = m_frame_index;
	CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}

//Input: pointer to device context for printer
//Input: desired margin
//Output: CRect to use for printing area
CRect UserPage(CDC * pDC, float margin)
{
	// This function returns the area in device units to be used to
	// prints a page with a true boarder of "margin".
	//
	// You could use individual margins for each edge
	// and apply below as needed.
	//
	// Set Map Mode - We do not want device units
	// due to lack of consistency.
	// If you do not use TWIPS you will have to change
	// the scaling factor below.
	int OriginalMapMode = pDC->SetMapMode(MM_TWIPS);

	// Variable needed to store printer info.
	CSize PrintOffset,Physical,Printable;

	// This gets the Physical size of the page in Device Units
	Physical.cx = pDC->GetDeviceCaps(PHYSICALWIDTH);
	Physical.cy = pDC->GetDeviceCaps(PHYSICALHEIGHT);
	// convert to logical
	pDC->DPtoLP(&Physical);

	// This gets the offset of the printable area from the
	// top corner of the page in Device Units
	PrintOffset.cx = pDC->GetDeviceCaps(PHYSICALOFFSETX);
	PrintOffset.cy = pDC->GetDeviceCaps(PHYSICALOFFSETY);
	// convert to logical
	pDC->DPtoLP(&PrintOffset);

	// Set Page scale to TWIPS, Which is 1440 per inch,
	// Zero/Zero is the upper left corner
	// Get Printable Page Size (This is in MM!) so convert to twips.
	Printable.cx =  (int)((float)pDC->GetDeviceCaps(HORZSIZE)*56.69);
	Printable.cy = (int)((float)pDC->GetDeviceCaps(VERTSIZE)*56.69);

	// Positive X -> RIGHT
	// Positive Y -> UP
	// Ref Zero is upper left corner
	int inch = 1440; // Scaling Factor Inches to TWIPS
	int Dx1, Dx2, Dy1, Dy2; // Distance printable area is from edge of paper
	Dx1 = PrintOffset.cx;
	Dy1 = PrintOffset.cy;
	// calculate remaining borders
	Dy2 = Physical.cy-Printable.cy-Dy1;
	Dx2 = Physical.cx-Printable.cx-Dx1;
	//
	// Define the User Area's location
	CRect PageArea;
	PageArea.left = (long)(margin*inch-Dx1);
	PageArea.right = (long)(Printable.cx-margin*inch+Dx2);
	PageArea.top = (int)-(margin*inch-Dy1); // My scale is inverted for y
	PageArea.bottom = (int)-(Printable.cy-margin*inch+Dy2);
	// now put back to device units to return to the program.
	pDC->LPtoDP(&PageArea);
	//
	// return
	return PageArea;
}

void CdcmviewerView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	if(pDC->IsPrinting())
	{
		LPDEVMODE  pDevMode;
		pDevMode = pInfo->m_pPD->GetDevMode();
		pDevMode->dmOrientation = DMORIENT_LANDSCAPE;
		pDC->ResetDC(pDevMode);
	}	
	CScrollView::OnPrepareDC(pDC, pInfo);
}


void CdcmviewerView::OnViewInvert()
{
	m_invert = !m_invert;
	Invalidate(FALSE);
}

void CdcmviewerView::OnUpdateViewInvert(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_invert?1:0);
}

void CdcmviewerView::OnDrawLine()
{
	// TODO: Add your command handler code here
}

void CdcmviewerView::OnDrawRect()
{
	// TODO: Add your command handler code here
}

void CdcmviewerView::OnDicomSaveAsGrey()
{
	//save RGB dicom file to grey, 8 bit
	CDocument* pDoc = GetDocument();
	if(pDoc)
	{
		CdcmviewerDoc* pdcmdoc = (CdcmviewerDoc*)pDoc;
		//process RGB image
		dicom::Tag tag_PhotometricInterpretation = dicom::makeTag(0x28,0x4);
		dicom::DataSet::const_iterator it = pdcmdoc->m_dataset.find(tag_PhotometricInterpretation);
		if(it!=pdcmdoc->m_dataset.end())
		{
			std::string photometric_interpretation = it->second.Get<std::string>();
			if(photometric_interpretation == "RGB")
			{ 
				//
				CFileDialog fd(FALSE);
				if(IDOK==fd.DoModal())
				{
					dicom::DataSet dscopy = pdcmdoc->m_dataset;
					dicom::Tag tagSamplePerPixel = dicom::makeTag(0x28,0x0002);
					dscopy.erase(dscopy.find(tagSamplePerPixel));
					dscopy.Put<dicom::VR_US>(tagSamplePerPixel,UINT16(1));//(0028,0002)  US  SamplesperPixel
					dicom::Tag tagPhotometricInterpretation = dicom::makeTag(0x28,0x0004);//(0028,0004)  CS  PhotometricInterpretation
					dscopy.erase(dscopy.find(tagPhotometricInterpretation));
					dscopy.Put<dicom::VR_CS>(tagPhotometricInterpretation,std::string("MONOCHROME2"));
									
					dicom::Tag tagPlanarConfiguration = dicom::makeTag(0x28,0x0006);//(0028,0006)  US  PlanarConfiguration
					const UINT8 * pixel_segment = get_pixel_data<UINT8>(pdcmdoc->m_dataset);
					typedef	std::vector<BYTE>	OB_TYPE;
					//dicom::DataSet::const_iterator it_pixel = data.find(dicom::TAG_PIXEL_DATA);
					OB_TYPE grey_pixels(m_rows*m_cols);
					BYTE *grey_pixel_pointer = grey_pixels.begin().operator->();
					int total_pixel_count = m_rows*m_cols;

					//dicom::Tag tagPlanarConfiguration = dicom::makeTag(0x28,0x06);
					//0 = The sample values for the first pixel are followed by the sample values for the second pixel
					//1 = Each color plane shall be sent contiguously.
					UINT16 usPlanarConfiguration = 0;
					dicom::DataSet::const_iterator itPlanarConfiguration = dscopy.find(tagPlanarConfiguration);
					if(itPlanarConfiguration != dscopy.end() )
					{
						usPlanarConfiguration = itPlanarConfiguration->second.Get<UINT16>();
					}
					dscopy.erase(dscopy.find(tagPlanarConfiguration));

					if(0== usPlanarConfiguration )
					{
						for(int i=0;i<total_pixel_count;i++)
						{
							grey_pixel_pointer[i] = (pixel_segment[3*i]+pixel_segment[3*i+1]+pixel_segment[3*i+2])/3;
						}
					}
					else
					{
						int goffset = total_pixel_count;
						int boffset = total_pixel_count+total_pixel_count;
						for(int i=0;i<total_pixel_count;i++)
						{
							grey_pixel_pointer[i] = (pixel_segment[i]+pixel_segment[i+goffset]+pixel_segment[i+boffset])/3;
						}

					}
					dscopy.erase(dscopy.find(dicom::TAG_PIXEL_DATA) );
					dscopy.Put<dicom::VR_OB>(dicom::makeTag(0x7FE0,0x10),grey_pixels);//(7FE0,0010)  OW  PixelData
					std::string filename = (LPCTSTR)fd.GetPathName();
					dicom::Write(dscopy,filename);
				}
			}
		}

	}

}

void CdcmviewerView::OnUpdateDicomSaveAsGrey(CCmdUI *pCmdUI)
{
	CDocument* pDoc = GetDocument();
	if(pDoc)
	{
		CdcmviewerDoc* pdcmdoc = (CdcmviewerDoc*)pDoc;
		//process RGB image
		dicom::Tag tag_PhotometricInterpretation = dicom::makeTag(0x28,0x4);
		dicom::DataSet::const_iterator it = pdcmdoc->m_dataset.find(tag_PhotometricInterpretation);
		if(it!=pdcmdoc->m_dataset.end())
		{
			std::string photometric_interpretation = it->second.Get<std::string>();
			if(photometric_interpretation == "RGB")
			{ 
				pCmdUI->Enable(TRUE);
				return;
			}
		}

	}
	pCmdUI->Enable(FALSE);
}

void CdcmviewerView::OnDicomSaveAsBmp()
{
	if(m_pdib.get())
	{
		CFileDialog fd(FALSE);
		if(IDOK==fd.DoModal())
		{
			CFile fbmp;
			if(fbmp.Open(fd.GetPathName(),CFile::modeCreate|CFile::modeWrite))
			{
				m_pdib->Write(&fbmp);
			}
		}
	}
}
int GetCodecClsid(const WCHAR* format, CLSID* pClsid)
{
	//[1312] image/bmp
	//	[1312] image/jpeg
	//	[1312] image/gif
	//	[1312] image/tiff
	//	[1312] image/png

	//return -1 if failed
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes
	int nret = -1;
	ImageCodecInfo* pImageCodecInfo = NULL;
	GetImageEncodersSize(&num, &size);
	if(size == 0)
		return -1;  // Failure
	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
		return -1;  // Failure
	GetImageEncoders(num, size, pImageCodecInfo);
	for(int j = 0; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			nret = j;  // Success
		}   
		//OutputDebugStringW(pImageCodecInfo[j].MimeType);
	} // for
	return nret;  // Failure
} // GetCodecClsid
struct GdiplusManager
{
	GdiplusManager();
	~GdiplusManager();
	bool gdiplus_ok;
	ULONG gdiplusToken; 
};
GdiplusManager::GdiplusManager()
{
	Gdiplus::GdiplusStartupInput gdiplusStartupInput; 
	Gdiplus::Status gdiplus_status;
	gdiplus_status = Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL); 
	if(Gdiplus::Ok == gdiplus_status)
		gdiplus_ok = true;		
}
GdiplusManager::~GdiplusManager()
{
	if(gdiplus_ok)
		Gdiplus::GdiplusShutdown(gdiplusToken);
}

GdiplusManager global_gdi_object;//must be global object

void save_dcm_as_image(LPCTSTR filename, LPCTSTR image_format, CDib* m_pdib)
{
	if(NULL == filename || NULL == image_format || NULL == m_pdib)
		return;
	std::string pathname = filename;
	if(end_with(pathname,image_format)==false)
		pathname += image_format;
	CComPtr<IStream> pStream(m_pdib->GetStream()) ;
	CLSID              codecClsid;
	EncoderParameters  encoderParameters;
	long               quality;
	Status             stat;
	Image image(pStream);
	std::string file_extension = image_format;
	if(file_extension == ".tif")
		file_extension = ".tiff";
	if(file_extension == ".jpg")
		file_extension = ".jpeg";
	_bstr_t clsid_name = "image/";
	clsid_name = clsid_name + file_extension.substr(1).c_str();
	GetCodecClsid(clsid_name, &codecClsid);
	int n = GetCodecClsid(clsid_name, &codecClsid);
	if(n<0)
	{
		AfxMessageBox(_T("GetCodecClsid Failed"));
		return;
	}
	encoderParameters.Count = 0;
	//////////specify parameter for jpg and tif
	if(file_extension.length()>4)
	{
		encoderParameters.Count = 1;
		encoderParameters.Parameter[0].Guid = EncoderQuality;
		encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
		encoderParameters.Parameter[0].NumberOfValues = 1;
		quality = 100;
		encoderParameters.Parameter[0].Value = &quality;
	}
	DeleteFile(pathname.c_str());
	stat = image.Save(_bstr_t(pathname.c_str()), &codecClsid, &encoderParameters);
	if( Ok != stat)
	{
		std::basic_string<TCHAR> gdiplus_error;
		switch(stat)
		{
		case GenericError: gdiplus_error = _T("GenericError"); break;
		case InvalidParameter: gdiplus_error = _T("InvalidParameter"); break;
		case OutOfMemory: gdiplus_error = _T("OutOfMemory"); break;
		case ObjectBusy: gdiplus_error = _T("ObjectBusy"); break;
		case FileNotFound: gdiplus_error = _T("FileNotFound"); break;
		default: gdiplus_error = _T("unknown GDI+ error");break;
		}
		std::basic_stringstream<TCHAR> tss;
		tss<< _T("Failed to save ")<< filename<< _T(", GDI+ error:")<< gdiplus_error<<_T(", Windows Last Error:")<<GetLastError();
		AfxMessageBox( tss.str().c_str());
	}
}
void CdcmviewerView::OnFileSaveAs()
{
	// save dicom as bmp/jpeg/png/gif/tif
	if(!m_pdib.get())
		return;
	static char BASED_CODE szFilters[] = "Jpeg Files (*.jpg)|*.jpg|Bitmap Files (*.bmp)|*.bmp|Png Files (*.png)|*.png|GIF Files (*.gif)|*.gif|TIFF Files (*.tif)|*.tif||";

	CFileDialog ofdlg (FALSE, ".bmp", NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | 
		OFN_PATHMUSTEXIST |OFN_EXPLORER, szFilters, AfxGetMainWnd());
	if(IDOK==ofdlg.DoModal())
	{
		CString pathname = ofdlg.GetPathName();
		CString fileext = ofdlg.GetFileExt();
		DWORD index = ofdlg.m_ofn.nFilterIndex;
		if(index==2)
		{
			CFile fbmp;
			if(end_with((LPCTSTR)pathname,_T(".bmp"))==false)
				pathname += _T(".bmp");
			if(fbmp.Open(pathname,CFile::modeCreate|CFile::modeWrite))
			{
				m_pdib->Write(&fbmp);
			}
			fbmp.Close();
		}
		else if( 3 == index)
		{
			save_dcm_as_image(pathname,".png",m_pdib.get());
		}
		else if( 4 == index)
		{
			save_dcm_as_image(pathname,".gif",m_pdib.get());
		}
		else if( 5 == index)
		{ 
			save_dcm_as_image(pathname,".tif",m_pdib.get());
		}
		else
		{ 
			save_dcm_as_image(pathname,".jpg",m_pdib.get());
		}
	}

}

void CdcmviewerView::OnFitToWindow()
{
	m_fit_to_window = !m_fit_to_window;
	SetScrollRange();
	this->UpdateWindow();
}

void CdcmviewerView::OnUpdateFitToWindow(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_fit_to_window);
}

void CdcmviewerView::get_scroll_rect(CRect& whole_rect)
{	//return view size
	//if in fit to window mode, return size of client rect 
	//otherwise return DICOM image size
	if(m_fit_to_window)
	{
		GetClientRect(&whole_rect);
	}
	else
	{
		whole_rect.left = 0;
		whole_rect.right = sizeTotal.cx;
		whole_rect.top = 0;
		whole_rect.bottom = sizeTotal.cy;
	}
}
void CdcmviewerView::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
}
