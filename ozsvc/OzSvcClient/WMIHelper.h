#pragma once
#include <Wbemidl.h>

class WMIHelper
{
public:
	WMIHelper(void);
	~WMIHelper(void);
	static HRESULT   PrintObject(IWbemClassObject*   spInstance); 

	//************************************
	// Method:    Connect
	// Brief:     
	// Returns:   int = 0 if successful
	// Parameter: const std::string & wmi_namespace
	// Parameter: const char * user_name
	// Parameter: const char * pwd
	//************************************
	int Connect(const std::string& wmi_namespace, 
		const char* user_name,
		const char* pwd);

	IWbemServices* GetServices();
	IWbemLocator* GetLocator();
private:
	CComPtr<IWbemServices> m_Services;
	CComPtr<IWbemLocator> m_Locator;
};
