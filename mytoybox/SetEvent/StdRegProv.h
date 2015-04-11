#pragma once
// http://msdn.microsoft.com/en-us/library/aa393664(VS.85).aspx
// StdRegProv is preinstalled in the WMI namespaces root\default and root\cimv2.

class CStdRegProv
{
public:
	typedef std::basic_string<TCHAR> tstring;
	CStdRegProv(IWbemServices* pWbemSvc, const tstring& objectPath);
	~CStdRegProv(void);
	unsigned int GetDWORDValue(
		int hDefKey,
		tstring sSubKeyName,
		tstring sValueName,
		unsigned int& uValue
		);
	unsigned int GetDWORDValue(
		tstring sValuePath, // HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\InstallDate
		unsigned int& uValue
		);
	CComPtr<IWbemClassObject> StdRegProv;
	IWbemServices* WbemSvc;
	tstring ObjectPath;
};

