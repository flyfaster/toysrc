#include <string>
#include <sstream>
#pragma warning(disable:4819)
#include <boost/algorithm/string.hpp>
#include <windows.h>
#include <atlbase.h>
#include <wincred.h>
#include <Wbemidl.h>
#include <comdef.h>
#include <loki/Pimpl.h>
#include <Wbemidl.h>
#include <iomanip>
#include <rpcdce.h>
#include <strsafe.h>
#include "wmihelper.h"
#include "COMInitializer.h"
#include "OZException.h"

wmihelper::wmihelper(void)
{
    HRESULT		hres = S_OK;
    hres = CoCreateInstance	( CLSID_WbemLocator, 0, 
        CLSCTX_INPROC_SERVER, IID_IWbemLocator	,
        (LPVOID *) &m_Locator	);
}

wmihelper::~wmihelper(void)
{
}

HRESULT wmihelper::PrintObject( IWbemClassObject* spInstance )
{
    if(!spInstance)  
        return   S_OK;  
    LPSAFEARRAY   psa   =   NULL;  
    HRESULT   hres;  

    {
        _variant_t   varClass;  
        hres   =   spInstance->Get(CComBSTR("__Class"),0,&varClass,NULL,0);  
        if (SUCCEEDED(hres) && varClass.vt == VT_BSTR)
        {
            m_tss<<"     " << "__Class \t" << ((LPCTSTR)_bstr_t(varClass)) << std::endl;
        }

        _variant_t   varRelPath;  
        hres   =   spInstance->Get(CComBSTR("__RelPath"),0,&varRelPath,NULL,0);  
        if (varClass != varRelPath && varRelPath.vt == VT_BSTR)
        {
            m_tss<<"     "<< "__RelPath \t" << ((LPCTSTR)_bstr_t(varRelPath)) << std::endl;
        }
    }

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
        m_tss<<"     "<<(LPCTSTR)_bstr_t(bstrPropName);  
        _variant_t   varProperty;  
        HRESULT   hr   =   spInstance->Get(bstrPropName,0,&varProperty,NULL,0);  
        m_tss<<"   =   ";  
        if(varProperty.vt   !=VT_EMPTY   &&   VT_NULL   !=   varProperty.vt)  
        {  
            _variant_t   vDest;  
            hr   =   VariantChangeType(&vDest,   &varProperty,0,VT_BSTR);  
            if(SUCCEEDED(hr))  
                m_tss<<   (LPCTSTR)_bstr_t(varProperty);  
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
                            m_tss<<   pIntArray[i]<<"   ";  
                        }  
                    }  
                    SafeArrayUnaccessData(psa);  
                }  
                else  
                    if (varProperty.vt == (VT_ARRAY | VT_BSTR) )
                    {
                        SAFEARRAY*   psa   =   varProperty.parray;  
                        BSTR   *pDataArray   =   NULL;  
                        SafeArrayAccessData(psa,(VOID**)&pDataArray);  
                        UINT   uDim   =   SafeArrayGetDim(psa);  
                        if(1==uDim)  
                        {  
                            long   lLbound,lRbound;  
                            SafeArrayGetLBound(psa,1,&lLbound);  
                            SafeArrayGetUBound(psa,1,&lRbound);  
                            for(long   i=lLbound;i<=lRbound;i++)  
                            {  
                                m_tss<<   pDataArray[i]<<"   ";  
                            }  
                        }  
                        SafeArrayUnaccessData(psa);  
                    }
                    else
                    {  
                        m_tss    <<   "   type   ";  
                    }  
            }  
            m_tss << " type = " << ToString(varProperty.vt);
        }  
        else  
        {  
            if(varProperty.vt   ==   VT_EMPTY)  
                m_tss<<   "VT_EMPTY   ";  
            else  
                m_tss<<   "VT_NULL";  
        }  
        m_tss<<std::endl;  
    }  
    if   (NULL   !=   psa)  
        SafeArrayDestroy(psa);  
    m_tss <<std::endl;  
    return   S_OK;
}

HRESULT wmihelper::Connect( LPCTSTR wmi_namespace, LPCTSTR user_name, LPCTSTR pwd )
{
    m_Services = NULL;
    HRESULT		hres = S_OK;
    CComBSTR bstrNamespace(wmi_namespace);

    if ( (user_name == NULL) 
        || (pwd == NULL)
        || _tcslen(user_name) ==0 || _tcslen(pwd)==0)	
    {
        hres = m_Locator->ConnectServer(bstrNamespace, NULL, NULL	,
            0, NULL, 0, 0, &m_Services	);
    }
    else	
    {
        //connect server using password and username
        CComBSTR bstrUsername(user_name), bstrPassword(pwd);
        hres = m_Locator->ConnectServer(bstrNamespace, bstrUsername, 
            bstrPassword, 0, NULL, 0, 0, 
            &m_Services	);
    }
	OZException::CheckHRESULT(__FUNCTION__, __LINE__, wmi_namespace, hres);	
    if (user_name)
    {
        m_full_name = user_name;
        if (pwd)
            m_pwd = pwd;
        tstring::size_type slash_pos = m_full_name.find(_T('\\'));
        if (slash_pos!=tstring::npos)
        {
            m_user = m_full_name.substr(slash_pos+1);
            m_domain = m_full_name.substr(0, slash_pos);
        }
        wcscpy(pszPwd, pwd);
        wcscpy(pszDomain, m_domain.c_str());
        wcscpy(pszFullName, m_full_name.c_str());
        wcscpy(pszUserName, m_user.c_str());
    }
    hres = SetProxyBlanket( m_Services);
    if(FAILED(hres))	
    {
        m_Services = NULL;
        return hres;
    }
    return hres;
}

HRESULT wmihelper::SetProxyBlanket( IUnknown* punk )
{
    //http://msdn.microsoft.com/en-us/library/aa393620(v=VS.85).aspx
    // Setting the Security on IWbemServices and Other Proxies
    // http://msdn.microsoft.com/en-us/library/aa390422(VS.85).aspx
    if (m_user.length()==0 ||m_pwd.length()==0)
        return S_OK;
    COAUTHIDENTITY *userAcct =  NULL ;
    COAUTHIDENTITY authIdent;
    memset(&authIdent, 0, sizeof(COAUTHIDENTITY));
    authIdent.PasswordLength = wcslen (pszPwd);
    authIdent.Password = (USHORT*)pszPwd;

    authIdent.User = (USHORT*)pszUserName;
    authIdent.UserLength = wcslen(pszUserName);

    authIdent.Domain = (USHORT*)pszDomain;
    authIdent.DomainLength = wcslen(pszDomain);
    authIdent.Flags = SEC_WINNT_AUTH_IDENTITY_UNICODE;

    userAcct = &authIdent;
    HRESULT hres = CoSetProxyBlanket( punk	, 
        RPC_C_AUTHN_WINNT	, 
        RPC_C_AUTHZ_NONE	, 
        NULL, 
        RPC_C_AUTHN_LEVEL_CALL		,
        RPC_C_IMP_LEVEL_IMPERSONATE	, 
        NULL,
        EOAC_NONE);
    OZException::CheckHRESULT(__FUNCTION__, __LINE__, _T("CoSetProxyBlanket failed."), hres);	
    return hres;

}

void wmihelper::ClearStream()
{
    m_tss.str(_T(""));
}

HRESULT wmihelper::ExecQuery( LPCTSTR wmi_query )
{
    CComPtr<IEnumWbemClassObject> pEnumObject;
    ULONG uCount = 1, uReturned=0;
    tstring msg;

    HRESULT hRes = m_Services->ExecQuery(_bstr_t(L"WQL"), _bstr_t( (LPCTSTR)wmi_query),
        WBEM_FLAG_RETURN_IMMEDIATELY |
        WBEM_FLAG_FORWARD_ONLY,
        NULL,
        &pEnumObject);
    if (FAILED(hRes)) throw hRes;
    hRes = SetProxyBlanket(pEnumObject);
    if (FAILED(hRes)) throw hRes;
    do 
    {
        CComPtr<IWbemClassObject> pClassObject;
        hRes = pEnumObject->Next(WBEM_INFINITE,uCount, &pClassObject, &uReturned);
        if (FAILED(hRes)) throw hRes;
        if (uReturned<=0)
            break;
//         ClearStream();
        PrintObject(pClassObject);
        m_tss.str();

    }while(uReturned>0);
    return hRes;
}

tstring wmihelper::GetString()
{
    return m_tss.str();
}

HRESULT wmihelper::GetLastErrorWMI( tstring& csErrRef, HRESULT hresErr )
{
    CComPtr<IWbemStatusCodeText> pStatus = NULL;

    HRESULT hres = CoCreateInstance(CLSID_WbemStatusCodeText, 0, CLSCTX_INPROC_SERVER,
        IID_IWbemStatusCodeText, (LPVOID *) &pStatus);

    if(S_OK == hres)
    {
        CComBSTR bstrError;
        hres = pStatus->GetErrorCodeText(hresErr, 0, 0, &bstrError);

        if(S_OK != hres)
            bstrError = SysAllocString(L"Get last error failed");

        csErrRef = (LPCTSTR)(bstrError);

    }

    return hres;
}

HRESULT wmihelper::Reboot( tstring machine, tstring user_name, tstring pwd )
{
    HRESULT hr = S_OK;
    tstringstream tss;
    tss << _T("\\\\") << machine << _T("\\root\\cimv2");
    hr = Connect(tss.str().c_str(), user_name.c_str(), pwd.c_str());
    if (FAILED(hr))
    {
        m_tss << _T("Failed at ") << (LPCTSTR)_bstr_t(__FUNCTION__) << _T(" line ") << __LINE__
            << _T(" error ") << std::hex << hr << std::endl;
        return hr;
    }
    tss.str(_T(""));

    CComPtr<IEnumWbemClassObject> pEnumObject;
    ULONG uCount = 1, uReturned=0;
    tstring msg;
    tstring wmi_query = _T("SELECT CSName, Name,  LastBootUpTime, FreePhysicalMemory, Status  from Win32_OperatingSystem");
   hr = m_Services->ExecQuery(_bstr_t(L"WQL"), _bstr_t( (LPCTSTR)wmi_query.c_str()),
        WBEM_FLAG_RETURN_IMMEDIATELY |
        WBEM_FLAG_FORWARD_ONLY,
        NULL,
        &pEnumObject);
    if (FAILED(hr))
    {
        m_tss << _T("Failed at ") << (LPCTSTR)_bstr_t(__FUNCTION__) << _T(" line ") << __LINE__
            << _T(" error ") << std::hex << hr << std::endl;
        return hr;
    }
    hr = SetProxyBlanket(pEnumObject);
    if (FAILED(hr))
    {
        m_tss << _T("Failed at ") << (LPCTSTR)_bstr_t(__FUNCTION__) << _T(" line ") << __LINE__
            << _T(" error ") << std::hex << hr << std::endl;
        return hr;
    }
    _bstr_t os_name;
    do 
    {
        CComPtr<IWbemClassObject> pClassObject;
        hr = pEnumObject->Next(WBEM_INFINITE,uCount, &pClassObject, &uReturned);
        if (FAILED(hr))
        {
            m_tss << _T("Failed at ") << (LPCTSTR)_bstr_t(__FUNCTION__) << _T(" line ") << __LINE__
                << _T(" error ") << std::hex << hr << std::endl;
            return hr;
        }
        if (uReturned<=0)
            break;
        //         ClearStream();
        _variant_t   varProperty;  
        hr   =   pClassObject->Get(CComBSTR("Name"),0,&varProperty,NULL,0); 
        if (FAILED(hr))
            continue;
        if (varProperty.vt == VT_BSTR)
            os_name = varProperty;
        PrintObject(pClassObject);
    }while(uReturned>0);
    if (os_name.length()>1)
    {
        tstring tname = (LPCTSTR)os_name;
        //tname.replace();
        boost::algorithm::replace_all(tname, _T("\\"), _T("\\\\"));
        // figure out WMI object path
        // "\\\\.\\root\\CIMV2:Win32_OperatingSystem.Name=\"Microsoft Windows XP Professional|C:\\\\WINDOWS|\\\\Device\\\\Harddisk0\\\\Partition1\"";
        tss.str(_T(""));
        tss << _T("\\\\") << machine << _T("\\root\\CIMV2:Win32_OperatingSystem.Name=\"")
            << tname << _T("\"");
        tstring objectpath = tss.str();
        _variant_t ret_value;
        hr = ExecMethod(_T("Win32_OperatingSystem"), 
            objectpath, 
            _T("Reboot"),
            ret_value);
        m_tss << _T("Reboot ") << objectpath << _T(" return ") << std::dec<< (long)ret_value << _T(", hr = ") 
            << std::hex << hr << std::endl;
        
    }
    return hr;
}

HRESULT wmihelper::ExecMethod( tstring classname, tstring objpath, tstring methodname, _variant_t& returnvalue )
{
    HRESULT hr = S_OK;
    CComPtr<IWbemClassObject> pClass, pMethodClass, pMethodInst, pOutInst;
    CComBSTR ClassPath("Win32_OperatingSystem"), MethodName("Reboot"), strObjectPath;

    strObjectPath=L"\\\\.\\root\\CIMV2:Win32_OperatingSystem.Name=\"Microsoft Windows XP Professional|C:\\\\WINDOWS|\\\\Device\\\\Harddisk0\\\\Partition1\"";
    strObjectPath = objpath.c_str();
    MethodName = methodname.c_str();
    ClassPath = classname.c_str();

    //     strObjectPath=L"\\\\.\\root\\CIMV2:Win32_OperatingSystem.CSName='ROOT-2010'";
        hr = m_Services->GetObject(ClassPath, 0, 
            NULL, &pClass, NULL);
		OZException::CheckHRESULT(__FUNCTION__, __LINE__, ClassPath, hr);	
        hr = pClass->GetMethod(MethodName, 0, 
            &pMethodClass, NULL); 
		OZException::CheckHRESULT(__FUNCTION__, __LINE__, MethodName, hr);	
        //         if (pMethodClass)
        //             hr = pMethodClass->SpawnInstance(0, &pMethodInst);
        //         if (FAILED(hr)) 
        //         {
        //             fail_at_line = __LINE__; throw hr;
        //         }

        hr = m_Services->ExecMethod(strObjectPath, 
            MethodName, 
            0, 
            NULL, 
            pMethodInst, 
            &pOutInst, 
            NULL);
		OZException::CheckHRESULT(__FUNCTION__, __LINE__, strObjectPath, hr);	
        _variant_t varReturnValue;
        hr = pOutInst->Get(_bstr_t(L"ReturnValue"), 0, &varReturnValue, NULL, 0);
		OZException::CheckHRESULT(__FUNCTION__, __LINE__, _T("Failed to get ReturnValue"), hr);	
    return hr;
        
}

void wmihelper::SaveStream( std::basic_ostream<TCHAR>& tos )
{
    tos << m_tss.str();
    tos.flush();
    m_tss.str(_T(""));
}

HRESULT wmihelper::InitializeSecurity()
{
    if (m_user.length()==0 ||m_pwd.length()==0)
        return S_OK;

// http://msdn.microsoft.com/en-us/library/aa393617(v=VS.85).aspx
    // Auth Identity structure
    SEC_WINNT_AUTH_IDENTITY_W        authidentity;
    SecureZeroMemory( &authidentity, sizeof(authidentity) );

    authidentity.User = (unsigned short*)pszUserName;
    authidentity.UserLength = wcslen( (wchar_t*)authidentity.User );
    authidentity.Domain = (unsigned short*)pszDomain;
    authidentity.DomainLength = wcslen( (wchar_t*)authidentity.Domain );
    authidentity.Password = (unsigned short*)pszPwd;
    authidentity.PasswordLength = wcslen( (wchar_t*)authidentity.Password );
    authidentity.Flags = SEC_WINNT_AUTH_IDENTITY_UNICODE;

    SOLE_AUTHENTICATION_INFO authninfo[2];
    SecureZeroMemory( authninfo, sizeof(SOLE_AUTHENTICATION_INFO)*2 );

    // NTLM Settings
    authninfo[0].dwAuthnSvc = RPC_C_AUTHN_WINNT;
    authninfo[0].dwAuthzSvc = RPC_C_AUTHZ_NONE;
    authninfo[0].pAuthInfo = &authidentity;

    // Kerberos Settings
    authninfo[1].dwAuthnSvc = RPC_C_AUTHN_GSS_KERBEROS ;
    authninfo[1].dwAuthzSvc = RPC_C_AUTHZ_NONE;
    authninfo[1].pAuthInfo = &authidentity;

    SOLE_AUTHENTICATION_LIST    authentlist;

    authentlist.cAuthInfo = 2;
    authentlist.aAuthInfo = authninfo;

    HRESULT hres = CoInitializeSecurity( 
        NULL, 
        -1, 
        NULL, 
        NULL, 
        RPC_C_AUTHN_LEVEL_CALL, 
        RPC_C_IMP_LEVEL_IMPERSONATE,
        &authentlist, 
        EOAC_NONE,
        NULL);
    OZException::CheckHRESULT(__FUNCTION__, __LINE__, _T("CoInitializeSecurity failed."), hres);	
    return hres;
}



// http://msdn.microsoft.com/en-us/library/aa384833(VS.85).aspx

