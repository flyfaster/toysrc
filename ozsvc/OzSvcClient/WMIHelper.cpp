#include "StdAfx.h"
#include "WMIHelper.h"
#include <comdef.h>
#include <Wbemidl.h>
#include <iomanip>
#include <rpcdce.h>
#include <strsafe.h>

WMIHelper::WMIHelper(void)
{
}

WMIHelper::~WMIHelper(void)
{
}

HRESULT WMIHelper::PrintObject( IWbemClassObject* spInstance )
{
	if(!spInstance)  
		return   S_OK;  
	std::cout<<__FUNCTION__<<"   start   "<<std::endl;  
	LPSAFEARRAY   psa   =   NULL;  
	HRESULT   hres;  
	hres   =   spInstance->GetNames(       NULL,  
		WBEM_FLAG_ALWAYS   |   WBEM_FLAG_NONSYSTEM_ONLY,  
		NULL,  
		&psa);  
	long       lLower,   lUpper;  
	SafeArrayGetLBound(psa   ,   1,   &lLower);  
	SafeArrayGetUBound(psa   ,   1,   &lUpper);  
	for   (long   i   =   lLower;   i   <=   lUpper;   ++i)    
	{  
		CComBSTR       bstrPropName;  
		if   (S_OK   !=     (hres   =   SafeArrayGetElement(psa,   &i,   &bstrPropName))   )  
		{  
			if   (NULL   !=   psa)  
				SafeArrayDestroy(psa);  
			return   hres;  
		}  
		std::cout<<"     "<<(LPCTSTR)_bstr_t(bstrPropName);  
		_variant_t   varProperty;  
		HRESULT   hr   =   spInstance->Get(bstrPropName,0,&varProperty,NULL,0);  
		std::cout<<"   =   ";  
		if(varProperty.vt   !=VT_EMPTY   &&   VT_NULL   !=   varProperty.vt)  
		{  
			_variant_t   vDest;  
			hr   =   VariantChangeType(&vDest,   &varProperty,0,VT_BSTR);  
			if(SUCCEEDED(hr))  
				std::cout<<   (LPCTSTR)_bstr_t(varProperty);  
			else  
			{  
				if(varProperty.vt   ==   (VT_ARRAY|VT_I4))  
				{  
					SAFEARRAY*   psa   =   varProperty.parray;  
					int   *pIntArray   =   NULL;  
					SafeArrayAccessData(psa,(VOID**)&pIntArray);  
					UINT   uDim   =   SafeArrayGetDim(psa);  
					if(1==uDim)  
					{  
						long   lLbound,lRbound;  
						SafeArrayGetLBound(psa,1,&lLbound);  
						SafeArrayGetUBound(psa,1,&lRbound);  
						for(long   i=lLbound;i<=lRbound;i++)  
						{  
							std::cout<<   pIntArray[i]<<"   ";  
						}  
					}  
					SafeArrayUnaccessData(psa);  
				}  
				else  
				{  
					std::cout<<   varProperty.vt   <<   "   type   ";  
				}  
			}  
			std::cout << " type = " << varProperty.vt;
		}  
		else  
		{  
			if(varProperty.vt   ==   VT_EMPTY)  
				std::cout<<   "VT_EMPTY   ";  
			else  
				std::cout<<   "VT_NULL";  
		}  
		std::cout<<std::endl;  
	}  
	if   (NULL   !=   psa)  
		SafeArrayDestroy(psa);  
	std::cout<<__FUNCTION__<<"   end   "<<std::endl;  
	return   S_OK;
}


int WMIHelper::Connect( const std::string& wmi_namespace, const char* user_name, const char* pwd )
{
	HRESULT		hres = S_OK;
	hres = CoCreateInstance	( CLSID_WbemLocator, 0, 
		CLSCTX_INPROC_SERVER, IID_IWbemLocator	,
		(LPVOID *) &m_Locator	);

	if (FAILED(hres))	{
		std::cout << "Failed to create IWbemLocator object.\n";
		return hres;
	}

	CComBSTR bstrNamespace(wmi_namespace.c_str());

	if ( strlen(user_name) ==0 || strlen(pwd)==0)	
	{
		hres = m_Locator->ConnectServer(bstrNamespace, NULL, NULL	,
			0, NULL, 0, 0, &m_Services	);
	}
	else	
	{
		//	connect server using password and username
		CComBSTR bstrUsername(user_name), bstrPassword(pwd);

		hres = m_Locator->ConnectServer(bstrNamespace, bstrUsername, 
			bstrPassword, 0, NULL, 0, 0, 
			&m_Services	);
	}

	if (FAILED(hres))   
	{
		m_Locator = NULL;
		return hres;
	}


	hres = CoSetProxyBlanket( m_Services	, 
		RPC_C_AUTHN_WINNT	, 
		RPC_C_AUTHZ_NONE	, 
		NULL, 
		RPC_C_AUTHN_LEVEL_CALL		,
		RPC_C_IMP_LEVEL_IMPERSONATE	, 
		NULL,
		EOAC_NONE);

	if(FAILED(hres))	
	{
		m_Services = NULL;
		m_Locator = NULL;
		return hres;
	}
	return hres;
}

IWbemServices* WMIHelper::GetServices()
{
	return m_Services;
}

IWbemLocator* WMIHelper::GetLocator()
{
	return m_Locator;
}