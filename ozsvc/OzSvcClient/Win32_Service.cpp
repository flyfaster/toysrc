#include "stdafx.h"
#include <comdef.h>
#include <Wbemidl.h>
#include <iomanip>
#include <rpcdce.h>
#include <strsafe.h>
#include "WMIHelper.h"
#include "Win32_Service.h"
using namespace std;

void Dump(std::stringstream& ss);

int control_service(LPCWSTR host_name, LPCWSTR domain_or_host_name, LPCWSTR pszName, LPCWSTR pszPwd, LPCWSTR service_name, LPCWSTR method_name)
{
	// Get the user name and password for the remote computer
	//wchar_t pszName[CREDUI_MAX_USERNAME_LENGTH+1];
	//wchar_t pszPwd[CREDUI_MAX_PASSWORD_LENGTH+1];
	//wchar_t pszDom[MAX_DOMAIN_NAME_LEN];
	//wcscpy(pszName, L"Admin");
	//wcscpy(pszPwd, L"verigy"); 
	//wcscpy(pszDom, L"Admin-PC");

	wchar_t auth_domain[MAX_DOMAIN_NAME_LEN + 16];
	wcscpy(auth_domain, L"ntlmdomain:");
	wcscat(auth_domain, domain_or_host_name);

	bstr_t wmi_query_string = "Select * from Win32_Service where name='";
	//OzSvc'";
	wmi_query_string += service_name;
	wmi_query_string += "'";
	_bstr_t strNetworkResource = "\\\\";
	strNetworkResource += host_name;
	strNetworkResource += "\\root\\cimv2";
	// Control service on remote computer
	// Changing the Startup type and description of the service
	HRESULT hres = 0;

	// Step 2: --------------------------------------------------
	// Set general COM security levels --------------------------
	// Note: If you are using Windows 2000, you need to specify -
	// the default authentication credentials for a user by using
	// a SOLE_AUTHENTICATION_LIST structure in the pAuthList ----
	// parameter of CoInitializeSecurity ------------------------
	std::stringstream cout;
	hres =  CoInitializeSecurity(
		NULL, 
		-1,                          // COM authentication
		NULL,                        // Authentication services
		NULL,                        // Reserved
		RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
		RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
		NULL,                        // Authentication info
		EOAC_NONE,                   // Additional capabilities 
		NULL                         // Reserved
		);
	if (FAILED(hres))
	{
		cout << "Failed to initialize security. Error code = 0x" 
			<< hex << hres << endl;
		Dump(cout);
		return 1;                    // Program has failed.
	}

	// Step 3: ---------------------------------------------------
	// Obtain the initial locator to WMI -------------------------
	CComPtr<IWbemLocator> pLoc = NULL;
	hres = CoCreateInstance(
		CLSID_WbemLocator,             
		0, 
		CLSCTX_INPROC_SERVER, 
		IID_IWbemLocator, (LPVOID *) &pLoc);

	if (FAILED(hres))
	{
		cout << "Failed to create IWbemLocator object."
			<< " Err code = 0x"
			<< hex << hres << endl;
		Dump(cout);
		return 1;                 // Program has failed.
	}

	// Step 4: -----------------------------------------------------
	// Connect to WMI through the IWbemLocator::ConnectServer method

	CComPtr<IWbemServices> pSvc = NULL;
	// Connect to the remote root\cimv2 namespace
	// and obtain pointer pSvc to make IWbemServices calls.
	//---------------------------------------------------------
	// change the computerName and domain 
	// strings below to the full computer name and domain 
	// of the remote computer

	hres = pLoc->ConnectServer(
		strNetworkResource,
		_bstr_t(pszName),                 // User name
		_bstr_t(pszPwd),                  // User password
		_bstr_t(L"MS_409"),               // Locale             
		WBEM_FLAG_CONNECT_USE_MAX_WAIT,                             // Security flags
		_bstr_t(auth_domain),    // Authority         ntlmdomain:domain
		0,                                // Context object 
		&pSvc                             // IWbemServices proxy
		);
	if (FAILED(hres))
	{
		hres = pLoc->ConnectServer(
			strNetworkResource,
			NULL,                 // User name
			NULL,                  // User password
			NULL,               // Locale             
			WBEM_FLAG_CONNECT_USE_MAX_WAIT,                             // Security flags
			NULL,    // Authority         ntlmdomain:domain
			0,                                // Context object 
			&pSvc                             // IWbemServices proxy
			);

	}
	if (FAILED(hres))
	{
		cout << "Could not connect. Error code = 0x" 
			<< hex << hres << endl;
		Dump(cout);
		return 1;                // Program has failed.
	}

	cout << "Connected to ROOT\\CIMV2 WMI namespace" << endl;
	Dump(cout);

	// Step 5: --------------------------------------------------
	// Set security levels on a WMI connection ------------------
	// refer to http://support.microsoft.com/kb/948829
	COAUTHIDENTITY cID;
	cID.User           = (USHORT*)pszName;
	cID.UserLength     = wcslen(pszName); //bstrUsername.length();
	cID.Password       = (USHORT*)pszPwd;
	cID.PasswordLength = wcslen(pszPwd); //bstrPassword.length();
	cID.Domain         = (USHORT*)domain_or_host_name;
	cID.DomainLength   = wcslen(domain_or_host_name); // bstrDomain.length();
	cID.Flags          = SEC_WINNT_AUTH_IDENTITY_UNICODE;

	hres = CoSetProxyBlanket(
		pSvc,                        // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
		NULL,                        // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		&cID,                        // client identity
		EOAC_NONE                    // proxy capabilities 
		);

	if (FAILED(hres))
	{
		cout << "Could not set proxy blanket. Error code = 0x" 
			<< hex << hres << endl;
		Dump(cout);
		return 1;               // Program has failed.
	}

	// Step 6: --------------------------------------------------
	// Use the IWbemServices pointer to make requests of WMI ----
	CComPtr<IEnumWbemClassObject> pEnumerator = NULL;
	hres = pSvc->ExecQuery(
		bstr_t("WQL"), 
		wmi_query_string,
		WBEM_FLAG_FORWARD_ONLY, 
		NULL,
		&pEnumerator);

	if (FAILED(hres))
	{
		cout << "Query " << (LPCSTR)wmi_query_string
			<< " Error code = 0x" 
			<< hex << hres << endl;
		Dump(cout);;
		return 1;               // Program has failed.
	}
	//
	// Must set the security on the enumerator interface as well
	// or you will received an access denied error
	//
	hres = CoSetProxyBlanket(
		pEnumerator,                 // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
		NULL,                        // Server principal name
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		&cID,                        // client identity
		EOAC_NONE                    // proxy capabilities
		);

	if (FAILED(hres))
	{
		cout << "Could not set proxy blanket. Error code = 0x"
			<< hex << hres << endl;
		return 1;               // Program has failed.
	}

	// Step 7: -------------------------------------------------
	// Get the data from the query in step 6 -------------------
	CComPtr<IWbemClassObject> pClass = NULL;
	hres = pSvc->GetObject(bstr_t("Win32_Service"), 0, NULL, &pClass, NULL);

	CComPtr<IWbemClassObject> pMethod=NULL;
	hres = pClass->GetMethod(bstr_t(method_name), 0, &pMethod, NULL);

	CComPtr<IWbemClassObject> pInInst=NULL;
	if (pMethod)
		hres = pMethod->SpawnInstance(0, &pInInst);
	if (pInInst)
	{
		CComVariant user_control_code = (BYTE)128;
		user_control_code.vt = VT_UI1;
		pInInst->Put(L"ControlCode", 0, &user_control_code, wbemCimtypeUint8);
	}
	ULONG uReturn = 1;
	while (pEnumerator)
	{
		CComPtr<IWbemClassObject> pclsObj=NULL;
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, 
			&pclsObj, &uReturn);
		if(0 == uReturn)
		{
			break;
		}
		CComVariant vtProp;
		// Get the value of the Name property
		hr = pclsObj->Get(L"Name", 0, &vtProp, 0, 0);
		wcout << " Name : " << vtProp.bstrVal << endl;

		CComVariant pathVar;
		pclsObj->Get(bstr_t("__PATH"), 0, &pathVar, 0, 0);
		_bstr_t InstancePath = pathVar.bstrVal;


		CComPtr<IWbemClassObject> pOutInst=NULL;
		hr = pSvc->ExecMethod(InstancePath, bstr_t(method_name), 0, NULL,
			pInInst, &pOutInst, NULL);
		//PrintObject(pOutInst);
		CComVariant method_return_value;
		pOutInst->Get(L"ReturnValue", 0, &method_return_value, 0, 0);
		if (method_return_value.vt == VT_I4)
		{
			cout << (LPCSTR)InstancePath << _T(".") << (LPCSTR)_bstr_t(method_name) << _T(" return ") << method_return_value.lVal;
			Dump(cout);
		}
	}
	return 0; //???

}

CWin32_Service::CWin32_Service( WMIHelper* wmi_svc )
	:m_wmi(wmi_svc)
{

}

int CWin32_Service::StartService( const std::string& service_name )
{
	return RunService(service_name, "StartService");
}

int CWin32_Service::StopService( const std::string& service_name )
{
	return RunService(service_name, "StopService");
}

int CWin32_Service::RunService( const std::string& service_name, const std::string& method )
{
	_bstr_t method_name = method.c_str();
	char query_buf[1024];
	sprintf(query_buf, "SELECT * from Win32_Service where name='%s'", service_name.c_str());
	HRESULT hres = S_OK;
	_bstr_t wmi_query_string = query_buf;
	IWbemServices* pSvc = m_wmi->GetServices();

	CComPtr<IEnumWbemClassObject> pEnumerator = NULL;
	hres = pSvc->ExecQuery(
		bstr_t("WQL"), 
		wmi_query_string,
		WBEM_FLAG_FORWARD_ONLY, 
		NULL,
		&pEnumerator);

	if (FAILED(hres))
	{
		cout << "Query " << (LPCSTR)wmi_query_string
			<< " Error code = 0x" 
			<< hex << hres << endl;
		return hres;               // Program has failed.
	}
	CComPtr<IWbemClassObject> pClass = NULL;
	hres = pSvc->GetObject(bstr_t("Win32_Service"), 0, NULL, &pClass, NULL);

	CComPtr<IWbemClassObject> pMethod=NULL;
	hres = pClass->GetMethod(method_name, 0, &pMethod, NULL);

	CComPtr<IWbemClassObject> pInInst=NULL;
	if (pMethod)
		hres = pMethod->SpawnInstance(0, &pInInst);
	ULONG uReturn = 1;
	while (pEnumerator)
	{
		CComPtr<IWbemClassObject> pclsObj=NULL;
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, 
			&pclsObj, &uReturn);
		if(0 == uReturn)
		{
			break;
		}
		CComVariant pathVar;
		pclsObj->Get(bstr_t("__PATH"), 0, &pathVar, 0, 0);
		_bstr_t InstancePath = pathVar.bstrVal;
		CComPtr<IWbemClassObject> pOutInst=NULL;
		hr = pSvc->ExecMethod(InstancePath, bstr_t(method_name), 0, NULL,
			pInInst, &pOutInst, NULL);
		//PrintObject(pOutInst);
		CComVariant method_return_value;
		pOutInst->Get(L"ReturnValue", 0, &method_return_value, 0, 0);
		if (method_return_value.vt == VT_I4)
		{
			cout << (LPCSTR)InstancePath << _T(".") << (LPCSTR)_bstr_t(method_name) 
				<< _T(" return ") << method_return_value.lVal << std::endl;
			return method_return_value.lVal;
		}
	}
	return hres; //???

}