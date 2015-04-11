#pragma once
#ifndef _ARRAY_UTIL_HPP
#define _ARRAY_UTIL_HPP

//template<typename DT>
//void print_array(const DT* psrc,int w,int h, const char* desc=0)
//{
//	std::ostringstream oss;
//	if(desc==0)
//		oss<<"array"<<std::endl;
//	else
//		oss<<desc<<std::endl;
//	for(int i=0;i<w;i++)
//	{
//		for(int j=0;j<h;j++)
//		{
//			oss<< psrc[i*h+j] <<" ";
//		}
//		oss<< char(0xd)<<char(0xa);
//	}
//	std::cout<<oss.str();
//}
//template<typename DT>
//void flip(const DT* psrc,int w,int h, DT* dest)
//{
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
//void mirror(const DT* psrc,int w,int h, DT* dest)
//{
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

#endif