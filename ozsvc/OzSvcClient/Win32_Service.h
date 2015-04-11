#pragma  once 
//#include <loki/Pimpl.h>

int control_service(LPCWSTR host_name, LPCWSTR domain_or_host_name, LPCWSTR pszName, 
					LPCWSTR pszPwd, LPCWSTR service_name, LPCWSTR method_name);

class WMIHelper;
class CWin32_Service //: private Loki::RimplOf<CWin32_Service>::Owner
{
public:
	CWin32_Service(WMIHelper* wmi_svc);
	int StartService(const std::string& service_name);
	int StopService(const std::string& service_name);
private:
	WMIHelper* m_wmi;
	int RunService(const std::string& service_name, const std::string& method);
};