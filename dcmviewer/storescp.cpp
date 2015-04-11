#include "stdafx.h"
#include "storescp.h"
#include <strsafe.h>
#include "dcmviewer_const.h"
bool check_ae_callback(const std::string& ae)
{
	return true;
}

std::map<std::string,std::string> g_accepted_files; // stored file list by C_STORE SCP
std::map<std::string,std::string> g_accepted_sops;
boost::mutex g_store_mutex; //
void StoreSCP(dicom::ServiceBase& pdu,const dicom::DataSet& command, dicom::DataSet& data)
{
	// data is pdu.Read(data);
	if(data.find(dicom::TAG_SOP_INST_UID)!=data.end())
	{
		boost::mutex::scoped_lock scoped_lock(g_store_mutex);
		dicom::UID tmp;
		data.operator()(dicom::TAG_SOP_INST_UID).Get<dicom::UID>(tmp);
		if(g_accepted_sops.find(tmp.str())!=g_accepted_sops.end())
			return;
	}
	std::ofstream fos;
	TCHAR pathbuf[MAX_PATH + 1024];
	pathbuf[0] = 0;
	TCHAR storefilename[MAX_PATH+1024];
	storefilename[0] = 0;
	GetModuleFileName(NULL, pathbuf, sizeof(pathbuf)/sizeof(pathbuf[0]) );
	size_t len = _tcslen(pathbuf)-1;
	while(len>0)
	{
		if(pathbuf[len] == _T('\\'))
		{
			pathbuf[len+1] = 0;
			SYSTEMTIME st;
			GetLocalTime(&st);
			boost::mutex::scoped_lock scoped_lock(g_store_mutex);
			DWORD now = GetTickCount();
			while(true)
			{
				StringCchPrintf(storefilename, sizeof(storefilename)/sizeof(storefilename[0]),
					_T("%s%04d%02d%02d%08X.DCM"),pathbuf, st.wYear,st.wMonth,st.wDay,now);
				if(g_accepted_files.find(storefilename)==g_accepted_files.end())
				{
					g_accepted_files[storefilename] = _T("newfile");
					break;
				}
				now--; // i dont think I will accept more than one image per tick.
			}
			break;
		}
		len--;
	}
	fos.open(storefilename,std::ios::binary|std::ios::out);
	if(fos.good())
	{
		dicom::WriteToStream(data,fos);
		fos.flush();
		fos.close();
		::PostMessage(g_main_wnd, WM_DICOM_STORE_SCP, 0, 0);
	}

}