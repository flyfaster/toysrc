#pragma	   once
#ifndef _LUT_CREATOR_HPP
#define _LUT_CREATOR_HPP

#ifdef DCMVIEWER_ENABLE_CRYPTLIB
#include "cryptlib.h"
#include "sha.h"
#endif
#include "onegadebug.h"



inline double dataset_get_ds(dicom::DataSet &data,dicom::Tag tag,double default_val)
{
	//dbval should be initialed to default value
	//this function read a string field , convert to double
	double dbval;
	dicom::DataSet::const_iterator it = data.find(tag);
	dbval = default_val;
	if(it!=data.end())
	{
		std::string stmp = data.operator()(tag).Get<std::string>();
		if(stmp.length()>0)
			dbval = atof(stmp.c_str());
	}
	return dbval;
}
inline UINT8 get_gray(double output,double min,double window_width)
{
	//output is Output units = m*SV + b. or mapped from modality lut
	UINT8 ret =UINT8((output-min) * 255/window_width) ;
	return ret;
}

template<typename TYPE_LUT,typename TYPE_PIXEL>
void create_lut_from_modality_lut(dicom::DataSet &ds,double min,double window_width,byte *m_lut)
{
	//ds = modality_lut[0];
	//min = window_center - window_width/2
	//window_width: as is
	//m_lut : table to store LUT table for display
	TimeWatcher(__TFUNCTION__);
	const dicom::Tag tag_lut_descriptor = dicom::makeTag(0x28,0x3002);
	std::pair<dicom::DataSet::const_iterator,dicom::DataSet::const_iterator> LUTDescriptors = ds.equal_range(tag_lut_descriptor);
	dicom::DataSet::const_iterator first = LUTDescriptors.first;
	UINT16 entry_count = first->second.Get<UINT16>();
	first++;
	UINT16 pixel_first_mapped_tmp = 0;
	pixel_first_mapped_tmp = first->second.Get<UINT16>();
	first++;
	UINT16 lut_bits_stored = first->second.Get<UINT16>();
	TYPE_PIXEL pixel_first_mapped = (TYPE_PIXEL)pixel_first_mapped_tmp;
	//(0028,3006)  SS/US  LUTData
	const dicom::Tag tag_LUTData = dicom::makeTag(0x28,0x3006);
	std::pair<dicom::DataSet::const_iterator,dicom::DataSet::const_iterator> LUTData = ds.equal_range(tag_LUTData);
	dicom::DataSet::const_iterator lutdata_first = LUTData.first;
	{
		//from min~ first stored pixel value mapped. 
		TYPE_LUT first_lut_value = lutdata_first->second.Get<TYPE_LUT>();
		TYPE_LUT output = first_lut_value;
		byte bgray = get_gray(output,min,window_width);
		int index = 0;
		//type of i use int, to avoid number auto wrap when limit is reached.
		int i = 0;
		for(i=(std::numeric_limits<TYPE_PIXEL>::min)(); i< pixel_first_mapped;i++)
		{
			index = (int(i) )& 0xffff;
			m_lut[index] = bgray;
		}
		//for pixel with entry in modality lut
		for(;lutdata_first!=LUTData.second;lutdata_first++)
		{
			output = lutdata_first->second.Get<TYPE_LUT>();
			bgray = get_gray(output,min,window_width);
			index = (int(i) )& 0xffff;
			m_lut[i] = bgray;
			i++;
		}
		//for pixel great than last entry of modality lut
		TYPE_PIXEL max_index = (std::numeric_limits<TYPE_PIXEL>::max)();
		for( i=pixel_first_mapped+entry_count-1; i<max_index; i++)
		{
			index = (int(i) )& 0xffff;
			m_lut[i] = bgray;
		}				
	}
}

template<typename INT16_TYPE>
void create_linear_lut(double rescale_slope,double rescale_intercept,byte *m_lut,double min, double window_width)
{
	//TimeWatcher(__TFUNCTION__);
	int max = min + (int)window_width;
	//	UINT8 ret =UINT8((output-min) * 255/window_width) ;
	double factor = 255/window_width;
	double offset = min*factor;
	for(int i=0;i<65536;i++)
	{
		//int output = i*n_rescale_slope +n_rescale_intercept;
		INT16_TYPE output = i&0xffff;
		output = INT16_TYPE((double)output*rescale_slope +rescale_intercept);
		if(output<=min)
			m_lut[i] = (byte)0;
		else
			if(output>=max)
				m_lut[i] = (byte)255;
			else
			{
				m_lut[i] = UINT8(output*factor - offset);
			}
	}
}

template<typename DT>
const DT* get_pixel_data(dicom::DataSet& data)
{
	const DT* pixel_data_pointer = NULL;
	typedef	std::vector<UINT16>	OW_TYPE;
	dicom::DataSet::const_iterator it_pixel = data.find(dicom::TAG_PIXEL_DATA);
	if(it_pixel->second.vr() == dicom::VR_OW)
	{
		const OW_TYPE &pixels = it_pixel->second.Get<OW_TYPE>();
		const UINT16 *pixel_data_pointer16 = pixels.begin().operator->();
		pixel_data_pointer =reinterpret_cast<const DT*>(pixel_data_pointer16);
	}
	else
	{
		typedef	std::vector<UINT8>	OB_TYPE;
		pixel_data_pointer = reinterpret_cast<const DT*>(it_pixel->second.Get<OB_TYPE>().begin().operator->() );
	}
	return pixel_data_pointer;
}

#ifdef DCMVIEWER_ENABLE_CRYPTLIB
std::basic_string<TCHAR> to_string(CryptoPP::SecByteBlock &all_digest,int size);

bool verify_exe();
void output_self_digest();
#endif

#endif