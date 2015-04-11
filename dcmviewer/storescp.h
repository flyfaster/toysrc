//#include "stdafx.h"
#pragma once
#ifndef _STORE_SCP_HPP
#define _STORE_SCP_HPP
void StoreSCP(dicom::ServiceBase& base,const dicom::DataSet& data1, dicom::DataSet& data2);
bool check_ae_callback(const std::string& ae);
#endif
