#include "StdAfx.h"
#include <sstream>
#include <comdef.h>
#include <Wbemidl.h>
#include <iomanip>
#include <rpcdce.h>
#include <strsafe.h>
#include "StdRegProv.h"
#include "WMIHelper.h"

using namespace std;
struct PrivateStdRegProv
{
    CComPtr<IWbemClassObject> pClass;
    CComPtr<IWbemServices> pSvc;
	CComPtr<IWbemLocator> pLoc;
	WMIHelper* m_wmi;
	HRESULT SetProxyBlanket(IUnknown* pUnknown);
	SEC_WINNT_AUTH_IDENTITY_W* pAuthIdentity;
	~PrivateStdRegProv();
	PrivateStdRegProv();
	bool IsLocal();
	static unsigned GethDefKey(HKEY root_key);
};

// class.__Path is \\BAJIE\ROOT\default:StdRegProv
_bstr_t ClassName = "StdRegProv";



unsigned PrivateStdRegProv::GethDefKey( HKEY root_key )
{
	return (unsigned)root_key;
}
bool PrivateStdRegProv::IsLocal()
{
	return pAuthIdentity==NULL || pAuthIdentity->UserLength==0;
}
PrivateStdRegProv::PrivateStdRegProv()
{
	pAuthIdentity = 
		new SEC_WINNT_AUTH_IDENTITY_W;
	ZeroMemory(pAuthIdentity, sizeof(SEC_WINNT_AUTH_IDENTITY_W));
	pAuthIdentity->User = new WCHAR[32];
	pAuthIdentity->Domain = new WCHAR[32];
	pAuthIdentity->Password = new WCHAR[32];
	pAuthIdentity->Flags = SEC_WINNT_AUTH_IDENTITY_UNICODE;
	pAuthIdentity->User[0] = 0;
	pAuthIdentity->UserLength = 0;
	pAuthIdentity->Domain[0] = 0;
	pAuthIdentity->Password[0] = 0;
}

PrivateStdRegProv::~PrivateStdRegProv()
{
	delete [] pAuthIdentity->User;
	delete [] pAuthIdentity->Domain;
	delete [] pAuthIdentity->Password;
	delete pAuthIdentity; 
}

HRESULT PrivateStdRegProv::SetProxyBlanket( IUnknown* pUnknown )
{
	if (pAuthIdentity->User[0]==0)
		return S_OK;

	HRESULT hres = CoSetProxyBlanket(pUnknown, 
		RPC_C_AUTHN_WINNT, 
		RPC_C_AUTHZ_NONE, 
		NULL,
		//COLE_DEFAULT_PRINCIPAL, 
		//RPC_C_AUTHN_LEVEL_PKT_PRIVACY,
		//RPC_C_AUTHN_LEVEL_DEFAULT, 
		RPC_C_AUTHN_LEVEL_CALL, 
		RPC_C_IMP_LEVEL_IMPERSONATE, 
		NULL, 
		EOAC_NONE 
		);
	if (FAILED(hres))
	{
		std::cout << __FUNCTION__ << " "
			<< typeid(pUnknown).name()<< " failed " << std::hex << hres << std::endl;
	}
	return hres;
}

CStdRegProv::CStdRegProv(WMIHelper* wmi)
{
	m_data = new PrivateStdRegProv;
	m_data->m_wmi = wmi;

	HRESULT hres = m_data->m_wmi->GetServices()->GetObject(bstr_t("StdRegProv"), 0, NULL, &m_data->pClass, NULL);
	if (FAILED(hres))
	{
		cout << "GetObject failed with error " << hex << hres << endl;
	}
	m_data->pSvc = m_data->m_wmi->GetServices();
	m_data->pLoc = m_data->m_wmi->GetLocator();
}

CStdRegProv::~CStdRegProv(void)
{
	delete m_data;
	m_data = NULL;
}

//void Dump(std::stringstream& os);



unsigned CStdRegProv::SetDWORDValue(
	unsigned hDefKey,
	const std::string& sSubKeyName,
	const std::string& sValueName,
	unsigned uValue
)
{
	HRESULT hres = S_OK;
	CComPtr<IWbemClassObject>& pClass = m_data->pClass;
	if (pClass == NULL)
		return E_FAIL;
	_bstr_t MethodName = "SetDWORDValue";
    CComPtr<IWbemClassObject> pMethod=NULL;
    hres = pClass->GetMethod(MethodName, 0, &pMethod, NULL);

    CComPtr<IWbemClassObject> pInParams=NULL;
    if (pMethod)
        hres = pMethod->SpawnInstance(0, &pInParams);

	//WMIHelper::PrintObject(pInParams);

	if (pInParams)
	{
		CComVariant user_control_code = hDefKey;
		user_control_code.vt = VT_I4;
		pInParams->Put(L"hDefKey", 0, &user_control_code, 0);wbemCimtypeUint32;
		CComVariant key(sSubKeyName.c_str());
		CComVariant value(sValueName.c_str());
		pInParams->Put(L"sSubKeyName", 0, &key, 0);
		pInParams->Put(L"sValueName", 0, &value, 0);
		CComVariant data = uValue;
		data.vt = VT_I4;
		pInParams->Put(L"uValue", 0, &data, 0);
	}

// Execute Method
	CComVariant method_return_value;

	CComPtr<IWbemClassObject> pOutParams = NULL;
	hres = m_data->pSvc->ExecMethod(ClassName, MethodName, 0,
	NULL, pInParams, &pOutParams, NULL);
	pOutParams->Get(L"ReturnValue", 0, &method_return_value, 0, 0);
	CComVariant pathVar;
	pClass->Get(bstr_t("__PATH"), 0, &pathVar, 0, 0);
	_bstr_t InstancePath = pathVar.bstrVal;

	if (method_return_value.vt == VT_I4)
	{
		cout << (LPCSTR)InstancePath << _T(".") << (LPCSTR)MethodName 
			<< _T(" return ") << method_return_value.lVal << std::endl;
		//Dump(cout);
		return method_return_value.lVal;
	}

	return 0;
}

unsigned CStdRegProv::SetStringValue( unsigned hDefKey, const std::string& sSubKeyName, 
									 const std::string& sValueName, const std::string& sValue )
{
	HRESULT hres = S_OK;
	CComPtr<IWbemClassObject>& pClass = m_data->pClass;
	if (pClass == NULL)
		return E_FAIL;
	_bstr_t MethodName = "SetStringValue";
	CComPtr<IWbemClassObject> pMethod=NULL;
	hres = pClass->GetMethod(MethodName, 0, &pMethod, NULL);

	CComPtr<IWbemClassObject> pInParams=NULL;
	if (pMethod)
		hres = pMethod->SpawnInstance(0, &pInParams);
	if (pInParams)
	{
		CComVariant user_control_code = hDefKey;
		user_control_code.vt = VT_UI4;
		pInParams->Put(L"hDefKey", 0, &user_control_code, 0);wbemCimtypeUint32;
		CComVariant key(sSubKeyName.c_str());
		CComVariant value(sValueName.c_str());
		pInParams->Put(L"sSubKeyName", 0, &key, 0);
		pInParams->Put(L"sValueName", 0, &value, 0);
		CComVariant data(sValue.c_str());
		pInParams->Put(L"sValue", 0, &data, 0);
	}

	// Execute Method
	CComVariant method_return_value;

	CComPtr<IWbemClassObject> pOutParams = NULL;
	hres = m_data->pSvc->ExecMethod(ClassName, MethodName, 0,
		NULL, pInParams, &pOutParams, NULL);
	pOutParams->Get(L"ReturnValue", 0, &method_return_value, 0, 0);
	CComVariant pathVar;
	pClass->Get(bstr_t("__PATH"), 0, &pathVar, 0, 0);
	_bstr_t InstancePath = pathVar.bstrVal;

	if (method_return_value.vt == VT_I4)
	{
		cout << (LPCSTR)InstancePath << _T(".") << (LPCSTR)MethodName 
			<< _T(" return ") << method_return_value.lVal << std::endl;
		//Dump(cout);
		return method_return_value.lVal;
	}

	return ERROR_SUCCESS;
}

DWORD CStdRegProv::GetDWORDValue( int hDefKey, const std::string& sSubKeyName, const std::string& sValueName, DWORD& uValue )
{
	HRESULT hres = S_OK;
	CComPtr<IWbemClassObject>& pClass = m_data->pClass;
	if (pClass == NULL)
		return E_FAIL;
	_bstr_t MethodName = "GetDWORDValue";
	CComPtr<IWbemClassObject> pMethod=NULL;
	hres = pClass->GetMethod(MethodName, 0, &pMethod, NULL);

	CComPtr<IWbemClassObject> pInParams=NULL;
	if (pMethod)
		hres = pMethod->SpawnInstance(0, &pInParams);
	if (pInParams)
	{
		CComVariant user_control_code = hDefKey;
		user_control_code.vt = VT_I4;
		pInParams->Put(L"hDefKey", 0, &user_control_code, 0);wbemCimtypeUint32;
		CComVariant key(sSubKeyName.c_str());
		CComVariant value(sValueName.c_str());
		pInParams->Put(L"sSubKeyName", 0, &key, 0);
		pInParams->Put(L"sValueName", 0, &value, 0);
	}
	// Execute Method
	CComPtr<IWbemClassObject> pOutParams = NULL;
	hres = m_data->pSvc->ExecMethod(ClassName, MethodName, 0,
		NULL, pInParams, &pOutParams, NULL);
	//WMIHelper::PrintObject(pOutParams);

	CComVariant out_uValue;
	pOutParams->Get(L"uValue", 0, &out_uValue, 0, 0);
	if (out_uValue.vt == VT_I4)
	{
		uValue = out_uValue.lVal;
	}
	CComVariant method_return_value;
	pOutParams->Get(L"ReturnValue", 0, &method_return_value, 0, 0);
	if (method_return_value.vt == VT_I4)
	{
		return method_return_value.lVal;
	}
	return 0;
}

DWORD CStdRegProv::GetStringValue( unsigned hDefKey, const std::string& sSubKeyName, const std::string& sValueName, std::string& sValue )
{
	HRESULT hres = S_OK;
	CComPtr<IWbemClassObject>& pClass = m_data->pClass;
	if (pClass == NULL)
		return E_FAIL;
	_bstr_t MethodName = "GetStringValue";
	CComPtr<IWbemClassObject> pMethod=NULL;
	hres = pClass->GetMethod(MethodName, 0, &pMethod, NULL);

	CComPtr<IWbemClassObject> pInParams=NULL;
	if (pMethod)
		hres = pMethod->SpawnInstance(0, &pInParams);
	if (pInParams)
	{
		CComVariant user_control_code = hDefKey;
		user_control_code.vt = VT_UI4;
		pInParams->Put(L"hDefKey", 0, &user_control_code, 0);wbemCimtypeUint32;
		CComVariant key(sSubKeyName.c_str());
		CComVariant value(sValueName.c_str());
		pInParams->Put(L"sSubKeyName", 0, &key, 0);
		pInParams->Put(L"sValueName", 0, &value, 0);
	}

	// Execute Method

	CComPtr<IWbemClassObject> pOutParams = NULL;
	hres = m_data->pSvc->ExecMethod(ClassName, MethodName, 0,
		NULL, pInParams, &pOutParams, NULL);
	CComVariant method_return_value;
	pOutParams->Get(L"ReturnValue", 0, &method_return_value, 0, 0);
	CComVariant data;
	pOutParams->Get(L"sValue", 0, &data, 0, 0);
	if (data.vt == VT_BSTR)
	{
		if (SysStringLen(data.bstrVal)>0)
		{
			sValue = (LPCSTR)(_bstr_t)data;
		}
	}
	if (method_return_value.vt == VT_I4)
	{
		return method_return_value.lVal;
	}

	return ERROR_SUCCESS;
}