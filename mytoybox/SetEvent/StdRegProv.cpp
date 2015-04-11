#include <string>
#include <sstream>
#include <map>
#include <atlbase.h>
#include <Wbemidl.h>
#include <comdef.h>
#include <Wbemidl.h>
#include <iomanip>
#include <rpcdce.h>
#include <strsafe.h>
#include <loki/Pimpl.h>
#include <boost/algorithm/string.hpp>
#include "OZException.h"
#include "StdRegProv.h"


CStdRegProv::CStdRegProv(IWbemServices* pWbemSvc, const tstring& objectPath) 
	: WbemSvc(pWbemSvc), ObjectPath(objectPath)
{
	if (WbemSvc)
	{
		CComBSTR ClassPath("StdRegProv");
		HRESULT hr = WbemSvc->GetObject(ClassPath, 0, 
			NULL, &StdRegProv, NULL);
		OZException::CheckHRESULT(__FUNCTION__, __LINE__, ClassPath, hr);	
	}
}


CStdRegProv::~CStdRegProv(void)
{
}

unsigned int CStdRegProv::GetDWORDValue( int nhDefKey, tstring ssSubKeyName, 
	tstring ssValueName, unsigned int& uValue )
{
	CComPtr<IWbemClassObject> pMethodClass, pMethodInst, pOutInst;
	CComBSTR MethodName("GetDWORDValue"), strObjectPath;
	strObjectPath = ObjectPath.c_str();
	HRESULT hr = StdRegProv->GetMethod(MethodName, 0, 
		&pMethodClass, NULL); 
	OZException::CheckHRESULT(__FUNCTION__, __LINE__, MethodName, hr);	
	if (pMethodClass)
		hr = pMethodClass->SpawnInstance(0, &pMethodInst);
	OZException::CheckHRESULT(__FUNCTION__, __LINE__, _T("SpawnInstance"), hr);	
	_variant_t hDefKey( ((long)HKEY_LOCAL_MACHINE), VT_I4);			
	hr = pMethodInst->Put(L"hDefKey", 0, &hDefKey, 0);
	OZException::CheckHRESULT(__FUNCTION__, __LINE__, _T("hDefKey"), hr);	
	_variant_t sSubKeyName( ssSubKeyName.c_str());			
	hr = pMethodInst->Put(L"sSubKeyName", 0, &sSubKeyName, 0);
	OZException::CheckHRESULT(__FUNCTION__, __LINE__, _T("sSubKeyName"), hr);
	_variant_t sValueName( ssValueName.c_str());			
	hr = pMethodInst->Put(L"sValueName", 0, &sValueName, 0);
	OZException::CheckHRESULT(__FUNCTION__, __LINE__, _T("sValueName"), hr);			
	hr = WbemSvc->ExecMethod(strObjectPath, 
		MethodName, 
		0, 
		NULL, 
		pMethodInst, 
		&pOutInst, 
		NULL);
	OZException::CheckHRESULT(__FUNCTION__, __LINE__, strObjectPath, hr);	
	_variant_t outvalue;
	hr = pOutInst->Get(_bstr_t(L"uValue"), 0, &outvalue, NULL, 0);
	OZException::CheckHRESULT(__FUNCTION__, __LINE__, _T("Failed to get uValue"), hr);
	uValue = outvalue;
	_variant_t varReturnValue;
	hr = pOutInst->Get(_bstr_t(L"ReturnValue"), 0, &varReturnValue, NULL, 0);
	OZException::CheckHRESULT(__FUNCTION__, __LINE__, _T("Failed to get ReturnValue"), hr);	
	unsigned int ret = varReturnValue;
	return ret;
}

unsigned int CStdRegProv::GetDWORDValue( 
	tstring sValuePath, /* HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\InstallDate */ 
	unsigned int& uValue )
{
	int nhDefKey=0; tstring ssSubKeyName; 
		tstring ssValueName;
	tstring::size_type pos = (sValuePath.find(_T('\\')));
	if (pos == tstring::npos)
		OZException::CheckHRESULT(__FUNCTION__, __LINE__, sValuePath.c_str(), E_INVALIDARG);	
	tstring sroot;
	sroot = sValuePath.substr(0, pos);
	if (sroot==_T("HKEY_LOCAL_MACHINE"))
		nhDefKey = (int)HKEY_LOCAL_MACHINE;
	if (nhDefKey == 0)
		OZException::CheckHRESULT(__FUNCTION__, __LINE__, sValuePath.c_str(), E_INVALIDARG);
	ssSubKeyName = sValuePath.substr(pos+1);
	if (ssSubKeyName.length()==0)
		OZException::CheckHRESULT(__FUNCTION__, __LINE__, sValuePath.c_str(), E_INVALIDARG);
	if (boost::algorithm::ends_with(ssSubKeyName, _T("\\"))==false)
	{
		pos = ssSubKeyName.find_last_of(_T('\\'));
		ssValueName = ssSubKeyName.substr(pos+1);
		ssSubKeyName = ssSubKeyName.substr(0, pos);
	}
	return GetDWORDValue(nhDefKey, ssSubKeyName, ssValueName, uValue);
}
