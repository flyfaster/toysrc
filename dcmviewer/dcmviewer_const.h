//for const variables and global variables
#pragma once
#ifndef _DCMVIEWER_CONST_HPP
#define _DCMVIEWER_CONST_HPP

//store scp send the following message to main window when an image is received
const int WM_DICOM_STORE_SCP = WM_USER + __LINE__;

typedef std::map<std::string,std::string> TP_FILE_CONTAINER;
extern  TP_FILE_CONTAINER g_accepted_files; // stored file list by C_STORE SCP
extern boost::mutex g_store_mutex; //

extern HWND g_main_wnd; // main window, cannot use theApp for some proc run inside boost thread

extern LPCTSTR DCMVIEWER_REG_PATH;
const DWORD default_local_port = 104;

extern LPCTSTR DCMVIEWER_REG_PATH;
extern LPCTSTR ADDRESS_BOOK_PATH;

const size_t MAX_AE_LEN = 16; // application entity
const size_t MAX_AS_LEN = 4; //fixed age string
const size_t MAX_AT_LEN = 4; //fixed attribute tag
const size_t MAX_CS_LEN = 16; // code string
const size_t MAX_DA_LEN = 8; // fixed (may be 10) date
const size_t MAX_DS_LEN = 16; //decimal string
const size_t MAX_DT_LEN = 26;
const size_t MAX_FL_LEN = 4; //fixed, floating point single
const size_t MAX_FD_LEN = 8; //fixed,floating point double
const size_t MAX_IS_LEN = 12; //integer string
const size_t MAX_LO_LEN = 64; //long string
const size_t MAX_LT_LEN = 10240;// long text
const size_t MAX_OB_LEN = -1;
const size_t MAX_OF_LEN = 2^32-4;
const size_t MAX_OW_LEN = -1;
const size_t MAX_PN_LEN = 64;
const size_t MAX_SH_LEN = 16; //short string
const size_t MAX_SL_LEN = 4;//fixed, signed long
const size_t MAX_SQ_LEN = -1;
const size_t MAX_SS_LEN = 2;//fiexed, signed short
const size_t MAX_ST_LEN = 1024;//short text
const size_t MAX_TM_LEN = 16;//time
const size_t MAX_UI_LEN = 64;
const size_t MAX_UL_LEN = 4;//fiexed, unsigned long
const size_t MAX_UN_LEN = -1;
const size_t MAX_US_LEN = 2;//fiexed
const size_t MAX_UT_LEN = 2^32 - 2;//unlimited text

#define DICOM_SOPRTIMAGESTORAGE			"1.2.840.10008.5.1.4.1.1.481.1"
#define DICOM_SOPRTDOSESTORAGE			"1.2.840.10008.5.1.4.1.1.481.2"
#define DICOM_SOPRTSTRUCTURESETSTORAGE		"1.2.840.10008.5.1.4.1.1.481.3"
#define DICOM_SOPRTBREAMS                       "1.2.840.10008.5.1.4.1.1.481.4"
#define DICOM_SOPRTPLANSTORAGE			"1.2.840.10008.5.1.4.1.1.481.5"
#define DICOM_SOPRTBRACHYTREATMENT              "1.2.840.10008.5.1.4.1.1.481.6"
#define DICOM_SOPRTTREATMENTSUMMARY             "1.2.840.10008.5.1.4.1.1.481.7"

#endif
