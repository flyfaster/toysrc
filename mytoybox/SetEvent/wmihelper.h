#pragma once
typedef std::basic_string<TCHAR> tstring;
typedef std::basic_stringstream<TCHAR> tstringstream;
class wmihelper
{
public:
    wmihelper(void);
    ~wmihelper(void);
    HRESULT PrintObject( IWbemClassObject* spInstance );
    tstringstream m_tss;
    CComPtr<IWbemServices> m_Services;
    CComPtr<IWbemLocator> m_Locator;
    void ClearStream();
    tstring GetString();
    HRESULT SetProxyBlanket( IUnknown* punk );

    HRESULT Connect( LPCTSTR wmi_namespace, LPCTSTR user_name, LPCTSTR pwd );


    HRESULT ExecQuery( LPCTSTR wmi_query );

    static HRESULT	GetLastErrorWMI (tstring&	csErrRef, HRESULT hresErr);

    HRESULT Reboot(tstring machine, tstring user_name, tstring pwd);

    HRESULT ExecMethod(tstring classname, tstring objpath, tstring methodname, _variant_t& returnvalue);
    void SaveStream(std::basic_ostream<TCHAR>& tos);

    HRESULT InitializeSecurity();
    tstring m_user;
    tstring m_pwd;
    tstring m_domain;
    tstring m_full_name;
    wchar_t pszPwd[CREDUI_MAX_PASSWORD_LENGTH+1];
    wchar_t pszFullName[CREDUI_MAX_USERNAME_LENGTH+1];
    wchar_t pszDomain[CREDUI_MAX_USERNAME_LENGTH+1];
    wchar_t pszUserName[CREDUI_MAX_USERNAME_LENGTH+1];

};

std::basic_string<TCHAR> ToString(VARTYPE vt);

