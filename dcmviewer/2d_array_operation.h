#pragma once
#ifndef _TWOD_ARRAY_OPERATION_HPP
#define _TWOD_ARRAY_OPERATION_HPP

#include <iostream>
#include <sstream>
void flip_array(UINT8* dibits,int m_rows, int m_cols,int bytes_per_pixel);

//template<typename DT>
//void array_rotateL90(const DT* psrc,int h,int w,DT* dest)
//{
//	//psrc[h][w] , dest[w][h]
//	if(0==psrc || h<= 0 || w<=0 || 0==dest)
//		return;
//	int index = 0;
//	int constant1 = (w-1)*h;
//	for(int i = 0; i < h; i++)
//	{
//		int var = 0;
//		for(int j = 0; j< w; j++)
//		{
//			//dest[ (w-1-j)*h +i] = psrc[index];
//			//dest[ (w-1)*h - j*h +i] = psrc[index];
//			dest[ constant1 - var +i] = psrc[index];
//			var += h;
//			index++;
//		}   
//	}
//}

//template<typename DT>
//void array_rotateR90(const DT* psrc,int h,int w,DT* dest)
//{
//	//psrc[h][w] , dest[w][h]
//	if(0==psrc || h<= 0 || w<=0 || 0==dest)
//		return;
//	int var = 0;
//	int constant1 = h - 1;
//	for(int j=0;j<w;j++)
//	{
//		//var = j*h
//		int var2 = 0; // i*w;
//		for(int i=0;i<h ;i++)
//		{
//			//dest[var+constant1-i] = psrc[ i*w+j];
//			dest[var+constant1-i] = psrc[ var2+j];
//			var2 += w;
//		}
//		var += h;
//	}
//}
//template<typename DT2>
//void array_rotate180(const DT2* psrc,int n,int m,DT2* dest)
//{
//	//psrc[n][m]
//	int constant1 = (m-1)*n + n-1;
//	int var = 0;// i * n
//	int index = 0;
//	for(int i=0;i<m;i++)
//	{
//		for(int j=0;j<n;j++)
//		{
//			//   dest[(m-1-i)*n+n-1-j] = psrc[i*n+j];
//			dest[constant1 -j -var] = psrc[index];
//			index++;
//		}
//		var += n; //i*n
//	}  
//}

//template<typename DT>
//void array_flip(const DT* psrc,int w,int h, DT* dest)
//{
//	if(0==psrc || h<= 0 || w<=0 || 0==dest)
//		return;
//	int offset = (w-1)*h;
//	int row_start = 0;
//	int offset2 = 0;
//	for(int i=0;i<w;i++)
//	{
//		offset2 = offset - row_start;// (w-1)*h - i*h
//		for(int j=0;j<h;j++)
//		{
//			//dest[ (w-1-i)*h+j] = psrc[i*h+j];
//			dest[ offset2 +j] = psrc[row_start +j];
//		}   
//		row_start += h; // i*h
//	}   
//}
//template<typename DT>
//void array_mirror(const DT* psrc,int w,int h, DT* dest)
//{
//	if(0==psrc || h<= 0 || w<=0 || 0==dest)
//		return;
//	int h_1 = h - 1;
//	int index = 0;
//	for(int i=0;i<w;i++)
//	{
//		for(int j=0;j<h;j++)
//		{
//			dest[index+h_1-j-j] = psrc[index];
//			index ++;
//		}   
//	}   
//}   

template<typename DT>
void find_min_max(const DT* pixel_data,int total_pixels,int& minval,int& maxval)
{
	if(NULL==pixel_data || total_pixels<=0)
		return;
	int hist[65536];
	memset(hist,0,sizeof(hist));
	minval = (std::numeric_limits<DT>::max)();
	maxval = (std::numeric_limits<DT>::min)();
	int index = 0;
	for(int i=0;i<total_pixels;i++)
	{
		DT p = pixel_data[i];
		index = p & 0xffff;
		hist[index] = hist[index] + 1;
		if(p<minval)
			minval = p;
		else
			if(p>maxval)
				maxval = p;
	}
	int start = minval;
	int end = maxval;
	//for GE MEDICAL SYSTEMS CT image, the padding value seems to be incorrect
	//so I decide to treat bounding value as this, suppose one of m_min,m_max is used as
	//padding value, and I think m_max, m_min would not be center of image.
	for(int i = start+1; i<end;i++)
	{
		index = i & 0xffff;
		if(hist[index]>64)
		{
			minval = i;
			break;
		}
	}
	for(int i=end-1;i>start;i--)
	{
		index = i & 0xffff;
		if(hist[index]>64)
		{
			maxval = i;
			break;
		}
	}
}

template<typename DT>
void find_min_max_ignore_padding(
								 const DT* pixel_data,
								 int total_pixels,
								 int& minval,
								 int& maxval,
								 int pixel_padding_value,
								 int* m_pixel_to_short_table

								 )
{
	if(NULL==pixel_data || total_pixels<=0)
		return;

	int index = 0;
	int hist[65536];
	memset(hist,0,sizeof(hist));
	minval = (std::numeric_limits<DT>::max)();
	maxval = (std::numeric_limits<DT>::min)();
	for(int i=0;i<total_pixels;i++)
	{
		DT p =m_pixel_to_short_table[ UINT16(pixel_data[i]) ] ;
		if( p == pixel_padding_value)
			continue;
		//index = p & 0xffff;
		index = p;
		hist[index] = hist[index] + 1;
		if(p<minval)
			minval = p;
		else
			if(p>maxval)
				maxval = p;
	}
	int start = minval;
	int end = maxval;
	for(int i = start+1; i<end;i++)
	{
		//index = i & 0xffff;
		index = m_pixel_to_short_table[i];
		if(hist[index]>64)
		{
			minval = index;
			break;
		}
	}
	for(int i=end-1;i>start;i--)
	{
		//index = i & 0xffff;
		index = m_pixel_to_short_table[i];
		if(hist[index]>64)
		{
			maxval = index;
			break;
		}
	}
}

#endif