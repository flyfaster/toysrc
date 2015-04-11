#pragma once

struct PrivateStdRegProv;
class WMIHelper;

class CStdRegProv 
{
public:
	CStdRegProv(WMIHelper* wmi_svc);
	~CStdRegProv(void);

	unsigned SetDWORDValue(
		unsigned hDefKey,
		const std::string& sSubKeyName,
		const std::string& sValueName,
		unsigned uValue
	);
	unsigned SetStringValue(
		unsigned hDefKey,
		const std::string& sSubKeyName,
		const std::string& sValueName,
		const std::string& sValue
		);

	//************************************
	// Method:    GetDWORDValue
	// Brief:     Return 0 if successful. 
	// Returns:   DWORD
	// Parameter: int hDefKey
	// Parameter: const std::string & sSubKeyName
	// Parameter: const std::string & sValueName
	// Parameter: DWORD & uValue
	//************************************
	DWORD GetDWORDValue(int hDefKey, 
		const std::string& sSubKeyName,
		const std::string& sValueName,
		DWORD& uValue
		);

	DWORD GetStringValue(
		unsigned hDefKey,
		const std::string& sSubKeyName,
		const std::string& sValueName,
		std::string& sValue
		);
private:
	PrivateStdRegProv* m_data;
};
