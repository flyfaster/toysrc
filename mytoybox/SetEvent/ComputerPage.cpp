#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/gbsizer.h>
#include <wx/textctrl.h>
#include <wx/Panel.h>
#include <wx/clipbrd.h>
#include <wx/confbase.h>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <iomanip>
#include <map>
#include <atlbase.h>
#include <Wbemidl.h>
#include <comdef.h>
#include <loki/Pimpl.h>
#include <Wbemidl.h>
#include <iomanip>
#include <rpcdce.h>
#include <strsafe.h>
# pragma comment(lib, "wbemuuid.lib")
#include "ComputerPage.h"
#include "COMInitializer.h"
#include "OZException.h"
typedef std::basic_string<TCHAR> tstring;
typedef std::basic_stringstream<TCHAR> tstringstream;

BEGIN_EVENT_TABLE(ComputerPage, wxPanel)
EVT_BUTTON(wxID_ANY, ComputerPage::OnButton)
END_EVENT_TABLE()

template<>
struct Loki::ImplOf<ComputerPage> : public wxEvtHandler
{
    std::basic_stringstream<TCHAR> m_tss;
    COMInitializer m_com_init;
    CComPtr<IWbemServices> m_Services;
    CComPtr<IWbemLocator> m_Locator;
    ImplOf<ComputerPage>();
    void ClearStream();
    HRESULT PrintObject( IWbemClassObject* spInstance );
    HRESULT Connect( LPCTSTR wmi_namespace, LPCTSTR user_name, LPCTSTR pwd );
    HRESULT SetProxyBlanket(IUnknown* punk);
	HRESULT ExecMethod( tstring classname, tstring objpath, tstring methodname, _variant_t& returnvalue );
	HRESULT GetLastErrorWMI( tstring& csErrRef, HRESULT hresErr );
	tstring GetString();
};
Loki::ImplOf<ComputerPage>::ImplOf()
{
	HRESULT		hres = S_OK;
	hres = CoCreateInstance	( CLSID_WbemLocator, 0, 
		CLSCTX_INPROC_SERVER, IID_IWbemLocator	,
		(LPVOID *) &m_Locator	);
	OZException::CheckHRESULT(__FUNCTION__, __LINE__, _T("CoCreateInstance CLSID_WbemLocator"), hres);
}
void Loki::ImplOf<ComputerPage>::ClearStream()
{
    m_tss.str(wxT(""));
}

HRESULT Loki::ImplOf<ComputerPage>::SetProxyBlanket( IUnknown* punk )
{
    //http://msdn.microsoft.com/en-us/library/aa393620(v=VS.85).aspx
    // Setting the Security on IWbemServices and Other Proxies
    HRESULT hr= CoSetProxyBlanket( punk	, 
        RPC_C_AUTHN_WINNT	, 
        RPC_C_AUTHZ_NONE	, 
        NULL, 
        RPC_C_AUTHN_LEVEL_CALL		,
        RPC_C_IMP_LEVEL_IMPERSONATE	, 
        NULL,
        EOAC_NONE);
	OZException::CheckHRESULT(__FUNCTION__, __LINE__, _T("CoSetProxyBlanket"), hr);
	return hr;
}

HRESULT Loki::ImplOf<ComputerPage>::Connect( LPCTSTR wmi_namespace, LPCTSTR user_name, LPCTSTR pwd )
{
    m_Services = NULL;
    HRESULT		hres = S_OK;
    CComBSTR bstrNamespace(wmi_namespace);
	int line_no = 0;
	wxString localnamespace;
	localnamespace.Format(_T("\\\\%s\\"), wxGetHostName());
	wxString localnamespace2;
	localnamespace2.Format(_T("\\\\%s\\"), wxGetFullHostName());
    if ( _tcslen(user_name) ==0 || _tcslen(pwd)==0 
		|| wxStrstr(wmi_namespace, _T("\\\\.\\"))
		|| wxStrstr(wmi_namespace, localnamespace2)
		|| wxStrstr(wmi_namespace, localnamespace2))	
    {
        hres = m_Locator->ConnectServer(bstrNamespace, NULL, NULL	,
            0, NULL, 0, 0, &m_Services	);  line_no = __LINE__;
    }
    else	
    {
        //connect server using password and username
        CComBSTR bstrUsername(user_name), bstrPassword(pwd);
        hres = m_Locator->ConnectServer(bstrNamespace, bstrUsername, 
            bstrPassword, 0, NULL, 0, 0, 
            &m_Services	); line_no = __LINE__;
    }
    OZException::CheckHRESULT(__FUNCTION__, line_no, bstrNamespace, hres);
    hres = SetProxyBlanket( m_Services);
    return hres;
}

HRESULT Loki::ImplOf<ComputerPage>::PrintObject( IWbemClassObject* spInstance )
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

HRESULT Loki::ImplOf<ComputerPage>::ExecMethod( tstring classname, 
	tstring objpath, tstring methodname, _variant_t& returnvalue )
{//http://msdn.microsoft.com/en-us/library/aa390421(v=VS.85).aspx
	HRESULT hr = S_OK;
	CComPtr<IWbemClassObject> pClass, pMethodClass, pMethodInst, pOutInst;
	CComBSTR ClassPath, MethodName, strObjectPath;
	strObjectPath=L"\\\\.\\root\\CIMV2:Win32_OperatingSystem.Name=\"Microsoft Windows XP Professional|C:\\\\WINDOWS|\\\\Device\\\\Harddisk0\\\\Partition1\"";
	strObjectPath = objpath.c_str();
	MethodName = methodname.c_str();
	ClassPath = classname.c_str();
	hr = m_Services->GetObject(ClassPath, 0, 
		NULL, &pClass, NULL);
	OZException::CheckHRESULT(__FUNCTION__, __LINE__, ClassPath, hr);
	hr = pClass->GetMethod(MethodName, 0, 
		&pMethodClass, NULL); 
	OZException::CheckHRESULT(__FUNCTION__, __LINE__, MethodName, hr);
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
	OZException::CheckHRESULT(__FUNCTION__, __LINE__, _T("Get ReturnValue"), hr);
	return hr;
}

HRESULT Loki::ImplOf<ComputerPage>::GetLastErrorWMI( tstring& csErrRef, HRESULT hresErr )
{
	CComPtr<IWbemStatusCodeText> pStatus = NULL;
	HRESULT hres = CoCreateInstance(CLSID_WbemStatusCodeText, 0, CLSCTX_INPROC_SERVER,
		IID_IWbemStatusCodeText, (LPVOID *) &pStatus);
	if(S_OK == hres)
	{
		CComBSTR bstrError;
		hres = pStatus->GetErrorCodeText(hresErr, 0, 0, &bstrError);
		if(S_OK != hres)
			bstrError = L"Get last error failed";
		csErrRef = (LPCTSTR)(bstrError);
	}
	return hres;
}

tstring Loki::ImplOf<ComputerPage>::GetString()
{
	tstring ts = m_tss.str();
	m_tss.str(_T(""));
	return ts;
}

LPCTSTR COMPUTER_PAGE_MACHINE = wxT("Machine");
LPCTSTR COMPUTER_PAGE_USER = wxT("User");
LPCTSTR COMPUTER_PAGE_PWD = wxT("Password");

ComputerPage::ComputerPage(wxWindow* parent) :wxPanel(parent, wxID_ANY)
{
    m_machine_btn = new wxButton(this, wxID_ANY, wxT("Machine..."));
    m_machine_input = new wxTextCtrl(this, wxID_ANY);
    m_user_input = new wxTextCtrl(this, wxID_ANY);
    m_pwd_input = new wxTextCtrl(this, wxID_ANY);
    m_user_label = new wxStaticText(this, wxID_ANY, wxT("User"));
    m_pwd_label = new wxStaticText(this, wxID_ANY, wxT("Pwd"));
	wxSizer* buttons_sizer = CreateButtons(this);
    m_LogWnd = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
        wxDefaultSize, wxTE_MULTILINE|wxVSCROLL|wxSIMPLE_BORDER );
    m_query_input = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
        wxDefaultSize, wxTE_MULTILINE|wxVSCROLL|wxSIMPLE_BORDER );

	m_shutdown_btn->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ComputerPage::OnShutdownMachine), NULL, this);
	m_restart_btn->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ComputerPage::OnRestartMachine), NULL, this);
    m_clear_btn->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ComputerPage::OnClearLog), NULL, this);
    m_process_btn->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ComputerPage::OnProcessButtonClicked), NULL, this);
    m_system_btn->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ComputerPage::OnSystemInfo), NULL, this);
    m_query_btn->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ComputerPage::OnWmiQuery), NULL, this);
    m_namespace_btn->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ComputerPage::OnNamespaceClicked), NULL, this);
    m_class_btn->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ComputerPage::OnClassButtonClicked), NULL, this);
	
    wxGridBagSizer* sizer = new wxGridBagSizer();
    int col = 0;
    int row = 0;
    sizer->Add(m_machine_btn, wxGBPosition(row, col));
    col++;
    sizer->Add(m_machine_input, wxGBPosition(row, col));
    row++;
    col=0;
    sizer->Add(m_user_label, wxGBPosition(row, col));
    col++;
    sizer->Add(m_user_input, wxGBPosition(row, col));
    col++;
    sizer->Add(m_pwd_label, wxGBPosition(row, col));
    col++;
    sizer->Add(m_pwd_input, wxGBPosition(row, col));
    col=0;
    row++;
	sizer->Add(buttons_sizer, wxGBPosition(row, col), wxGBSpan(1, 9), wxEXPAND | wxHORIZONTAL);
    row++;
    int col_count = 9;
    col=0;
    sizer->Add(m_LogWnd, wxGBPosition(row, 0), wxGBSpan(1, col_count), wxEXPAND | wxALL);
    sizer->AddGrowableRow(row);
    sizer->AddGrowableCol(col_count-1);
    sizer->Add(m_query_input, wxGBPosition(0, 4), wxGBSpan(2, 5), wxEXPAND | wxALL);    
	SetSizer(sizer);
    sizer->SetSizeHints(this);
    m_machine_input->SetValue(wxConfigBase::Get()->Read(COMPUTER_PAGE_MACHINE, wxT(".")));
    m_user_input->SetValue(wxConfigBase::Get()->Read(COMPUTER_PAGE_USER, wxEmptyString));
    m_pwd_input->SetValue(wxConfigBase::Get()->Read(COMPUTER_PAGE_PWD, wxEmptyString));
}

ComputerPage::~ComputerPage(void)
{

}

void ComputerPage::OnButton( wxCommandEvent& event )
{

}

void ComputerPage::OnRestartMachine( wxCommandEvent& event )
{
	if (m_machine_input->GetValue().length()<1)
		return;
	wxDateTime m_start_time = wxDateTime::Now();
	ULONG uCount = 1, uReturned=0;
	wxString wmi_namespace = wxString::Format(wxT("\\\\%s\\root\\cimv2"), m_machine_input->GetValue());
	try 
	{
		CComPtr<IEnumWbemClassObject> pEnumObject;
		HRESULT hRes = d->Connect( 
			wmi_namespace,
			m_user_input->GetValue(),
			m_pwd_input->GetValue());		
		hRes = d->m_Services->ExecQuery(_bstr_t(L"WQL"), 
			_bstr_t("SELECT CSName, Name,  LastBootUpTime, FreePhysicalMemory, Status  from Win32_OperatingSystem"),
			WBEM_FLAG_RETURN_IMMEDIATELY |
			WBEM_FLAG_FORWARD_ONLY,
			NULL,
			&pEnumObject);
		OZException::CheckHRESULT(__FUNCTION__, __LINE__, _T("ExecQuery"), hRes);		
		hRes = d->SetProxyBlanket(pEnumObject);		
		_bstr_t os_name;
		tstringstream& m_tss = d->m_tss;
		HRESULT& hr = hRes;
		do 
		{			
			CComPtr<IWbemClassObject> pClassObject;
			hr = pEnumObject->Next(WBEM_INFINITE,uCount, &pClassObject, &uReturned);
			OZException::CheckHRESULT(__FUNCTION__, __LINE__, _T("pEnumObject->Next(WBEM_INFINITE)"), hr);
			if (uReturned<=0)
				break;
			_variant_t   varProperty;  
			hr   =   pClassObject->Get(CComBSTR("Name"),0,&varProperty,NULL,0); 
			if ( SUCCEEDED(hr) &&  (varProperty.vt == VT_BSTR))
				os_name = varProperty;
		}while(uReturned>0);
		if (os_name.length()>1)
		{
			tstring tname = (LPCTSTR)os_name;
			boost::algorithm::replace_all(tname, _T("\\"), _T("\\\\"));
			// figure out WMI object path
			// "\\\\.\\root\\CIMV2:Win32_OperatingSystem.Name=\"Microsoft Windows XP Professional|C:\\\\WINDOWS|\\\\Device\\\\Harddisk0\\\\Partition1\"";
			tstringstream tss;
			tss << _T("\\\\") << m_machine_input->GetValue() << _T("\\root\\CIMV2:Win32_OperatingSystem.Name=\"")
				<< tname << _T("\"");
			tstring objectpath = tss.str();
			_variant_t ret_value;
			hr = d->ExecMethod(_T("Win32_OperatingSystem"), 
				objectpath, 
				_T("Reboot"),
				ret_value);
			m_tss << _T("Reboot ") << objectpath << _T(" return ") << std::dec<< (long)ret_value << _T(", hr = ") 
				<< std::hex << hr << std::endl;
		}
	}
	catch (const OZException& e)
	{
		d->m_tss<< _T("Exception in ") << (LPCTSTR)_bstr_t(__FUNCTION__) << _T(" ")
			<< e.ToString() << std::endl;
	}
	m_LogWnd->AppendText(d->GetString().c_str());
	DisplayDuration(__FUNCTION__, m_start_time);
}

void ComputerPage::OnClearLog( wxCommandEvent& event )
{
    m_LogWnd->SetValue(wxEmptyString);
}

void ComputerPage::OnProcessButtonClicked( wxCommandEvent& evt )
{
    if (m_machine_input->GetValue().length()<1)
        return;
    wxDateTime m_start_time = wxDateTime::Now();
    ULONG uCount = 1, uReturned=0;
    wxString wmi_namespace = wxString::Format(wxT("\\\\%s\\root\\cimv2"), m_machine_input->GetValue());
    wxString msg;
    msg.Format(wxT("Get processes information on %s.\n"), m_machine_input->GetValue());
    m_LogWnd->AppendText(msg);
    SaveConfig();
    try 
    {
        CComPtr<IEnumWbemClassObject> pEnumObject;
        HRESULT hRes = d->Connect( 
            wmi_namespace,
            m_user_input->GetValue(),
            m_pwd_input->GetValue());
        hRes = d->m_Services->ExecQuery(_bstr_t(L"WQL"), _bstr_t("SELECT * FROM Win32_Process"),
            WBEM_FLAG_RETURN_IMMEDIATELY |
            WBEM_FLAG_FORWARD_ONLY,
            NULL,
            &pEnumObject);
        hRes = d->SetProxyBlanket(pEnumObject);
        do 
        {
            CComPtr<IWbemClassObject> pClassObject;
            hRes = pEnumObject->Next(WBEM_INFINITE,uCount, &pClassObject, &uReturned);
            if (uReturned<=0)
                break;
            d->ClearStream();
            d->PrintObject(pClassObject);
            m_LogWnd->AppendText(d->m_tss.str());
        }while(uReturned>0);
    }
	catch (const OZException& e)
	{
		d->m_tss<< _T("Exception in ") << (LPCTSTR)_bstr_t(__FUNCTION__) << _T(" ")
			<< e.ToString() << std::endl;
	}
	m_LogWnd->AppendText(d->GetString().c_str());
    DisplayDuration(__FUNCTION__, m_start_time);
}

void ComputerPage::OnSystemInfo( wxCommandEvent& evt )
{
    // ip, memory size, memory usage, cpu type, cpu usage, disk size, free disk space
    // Win32_PhysicalMemory  Win32_OperatingSystem  Win32_DiskDrive
    if (m_machine_input->GetValue().length()<1)
        return;
    wxDateTime m_start_time = wxDateTime::Now();
    ULONG uCount = 1, uReturned=0;
    wxString wmi_namespace = wxString::Format(wxT("\\\\%s\\root\\cimv2"), m_machine_input->GetValue());
    wxString msg;
    msg.Format(wxT("Get processes information on %s.\n"), m_machine_input->GetValue());
    m_LogWnd->AppendText(msg);
    try 
    {
        CComPtr<IEnumWbemClassObject> pEnumObject;
        HRESULT hRes = d->Connect( 
            wmi_namespace,
            m_user_input->GetValue(),
            m_pwd_input->GetValue());
        hRes = ExecQuery(wxT("SELECT * FROM Win32_PhysicalMemory"));
        hRes = ExecQuery(wxT("SELECT * FROM Win32_OperatingSystem"));
        hRes = ExecQuery(wxT("SELECT Caption,TotalCylinders FROM Win32_DiskDrive"));
    }
	catch (const OZException& e)
	{
		d->m_tss<< _T("Exception in ") << (LPCTSTR)_bstr_t(__FUNCTION__) << _T(" ")
			<< e.ToString() << std::endl;
	}
	m_LogWnd->AppendText(d->GetString().c_str());
    DisplayDuration(__FUNCTION__, m_start_time);
    SaveConfig();
}

HRESULT ComputerPage::ExecQuery( LPCTSTR wmi_query )
{
    CComPtr<IEnumWbemClassObject> pEnumObject;
    ULONG uCount = 1, uReturned=0;
   HRESULT hRes = d->m_Services->ExecQuery(_bstr_t(L"WQL"), _bstr_t( (LPCTSTR)wmi_query),
        WBEM_FLAG_RETURN_IMMEDIATELY |
        WBEM_FLAG_FORWARD_ONLY,
        NULL,
        &pEnumObject);
    hRes = d->SetProxyBlanket(pEnumObject);
    do 
    {
        CComPtr<IWbemClassObject> pClassObject;
        hRes = pEnumObject->Next(WBEM_INFINITE,uCount, &pClassObject, &uReturned);
        if (uReturned<=0)
            break;
        d->ClearStream();
        d->PrintObject(pClassObject);
        m_LogWnd->AppendText(d->m_tss.str());
    }while(uReturned>0);
    return hRes;
}

void ComputerPage::OnWmiQuery( wxCommandEvent& evt )
{
    if (m_machine_input->GetValue().length()<1)
        return;
    if (m_query_input->GetValue().Len()<1)
        return;
    wxDateTime m_start_time = wxDateTime::Now();
    ULONG uCount = 1, uReturned=0;
    wxString wmi_namespace = GetWmiNamespace();
    wxString msg;
    msg.Format(wxT("Perform WMI query on %s.\n"), m_machine_input->GetValue());
    m_LogWnd->AppendText(msg);
    try 
    {
        CComPtr<IEnumWbemClassObject> pEnumObject;
        HRESULT hRes = d->Connect( 
            wmi_namespace,
            m_user_input->GetValue(),
            m_pwd_input->GetValue());
        hRes = ExecQuery(m_query_input->GetValue());
    }
	catch (const OZException& e)
	{
		d->m_tss<< _T("Exception in ") << (LPCTSTR)_bstr_t(__FUNCTION__) << _T(" ")
			<< e.ToString() << std::endl;
	}
	m_LogWnd->AppendText(d->GetString().c_str());
    DisplayDuration(__FUNCTION__, m_start_time);
    SaveConfig();
}

void ComputerPage::DisplayDuration( LPCSTR name, wxDateTime& start_time )
{
	wxString msg;
	wxTimeSpan duration = wxDateTime::Now() - start_time;
	if (duration.GetSeconds()>4)
	{
		msg.Printf(_T("%s spends %d seconds\n"), wxString::FromAscii(name), duration.GetSeconds().ToLong());
	}
	else
	{
		msg.Printf(_T("%s spends %d milliseconds\n"), wxString::FromAscii(name), duration.GetMilliseconds().ToLong());
	}
	m_LogWnd->AppendText(msg);
}

void ComputerPage::SaveConfig()
{
    wxConfigBase::Get()->Write(COMPUTER_PAGE_MACHINE, m_machine_input->GetValue());
    wxConfigBase::Get()->Write(COMPUTER_PAGE_USER, m_user_input->GetValue());
    wxConfigBase::Get()->Write(COMPUTER_PAGE_PWD, m_pwd_input->GetValue());
}

void ComputerPage::OnNamespaceClicked( wxCommandEvent& evt )
{
    if (m_machine_input->GetValue().length()<1)
        return;
    wxDateTime m_start_time = wxDateTime::Now();
    ULONG uCount = 1, uReturned=0;
    wxString wmi_namespace = wxString::Format(wxT("\\\\%s\\root"), m_machine_input->GetValue());
    wxString msg;
    msg.Format(wxT("Get namespace information on %s.\n"), m_machine_input->GetValue());
    m_LogWnd->AppendText(msg);
    m_namespace_list->Clear();
    try 
    {
        CComPtr<IEnumWbemClassObject> pEnumObject;
        HRESULT hRes = d->Connect( 
            wmi_namespace,
            m_user_input->GetValue(),
            m_pwd_input->GetValue());
        hRes = d->m_Services->CreateInstanceEnum(
            CComBSTR("__NAMESPACE"),
            WBEM_FLAG_FORWARD_ONLY,
            NULL,
            &pEnumObject);
        hRes = d->SetProxyBlanket(pEnumObject);
        CComBSTR property_name("Name");
        do 
        {
            CComPtr<IWbemClassObject> pClassObject;
            hRes = pEnumObject->Next(WBEM_INFINITE,uCount, &pClassObject, &uReturned);
            if (uReturned<=0)
                break;
            _variant_t   varProperty;  
            hRes   =   pClassObject->Get(property_name,0,&varProperty,NULL,0);  
            if (SUCCEEDED(hRes) && varProperty.vt == VT_BSTR)
            {
                m_namespace_list->AppendString((LPCTSTR)_bstr_t(varProperty));
            }
        }while(uReturned>0);
    }
	catch (const OZException& e)
	{
		d->m_tss<< _T("Exception in ") << (LPCTSTR)_bstr_t(__FUNCTION__) << _T(" ")
			<< e.ToString() << std::endl;
	}
	m_LogWnd->AppendText(d->GetString().c_str());
    DisplayDuration(__FUNCTION__, m_start_time);
    SaveConfig();
}

void ComputerPage::OnClassButtonClicked( wxCommandEvent& evt )
{
    if (m_machine_input->GetValue().length()<1)
        return;
    wxDateTime m_start_time = wxDateTime::Now();
    ULONG uCount = 1, uReturned=0;
    wxString wmi_namespace = GetWmiNamespace();
    wxString msg;
    msg.Format(wxT("Get class information in %s.\n"), wmi_namespace);
    m_LogWnd->AppendText(msg);
    try 
    {
        CComPtr<IEnumWbemClassObject> pEnumObject;
        HRESULT hRes = d->Connect( 
            wmi_namespace,
            m_user_input->GetValue(),
            m_pwd_input->GetValue());
        hRes = d->m_Services->CreateClassEnum(
            CComBSTR(""),
            WBEM_FLAG_FORWARD_ONLY,
            NULL,
            &pEnumObject);
        PrintEnumWbemClassObject(pEnumObject);
    }
	catch (const OZException& e)
	{
		d->m_tss<< _T("Exception in ") << (LPCTSTR)_bstr_t(__FUNCTION__) << _T(" ")
			<< e.ToString() << std::endl;
	}
	m_LogWnd->AppendText(d->GetString().c_str());
    DisplayDuration(__FUNCTION__, m_start_time);
    SaveConfig();
}

void ComputerPage::PrintEnumWbemClassObject( IEnumWbemClassObject* pEnumObject )
{
    ULONG uCount = 1, uReturned=0;
    wxString msg;
    HRESULT hRes = d->SetProxyBlanket(pEnumObject);
    do 
    {
        CComPtr<IWbemClassObject> pClassObject;
        hRes = pEnumObject->Next(WBEM_INFINITE,uCount, &pClassObject, &uReturned);
        if (uReturned<=0)
            break;
        d->ClearStream();
        d->PrintObject(pClassObject);
        m_LogWnd->AppendText(d->m_tss.str());
    }while(uReturned>0);
}

wxString ComputerPage::GetWmiNamespace()
{
    wxString wmi_namespace(wxT("\\\\.\\root\\cimv2"));
	if (m_machine_input->GetValue().length())		
		wmi_namespace = wxString::Format(wxT("\\\\%s\\root\\cimv2"), m_machine_input->GetValue());
    if (m_namespace_list->GetValue().length())
    {
        wmi_namespace = wxString::Format(wxT("\\\\%s\\root\\%s"), m_machine_input->GetValue(), m_namespace_list->GetValue());
    }
    return wmi_namespace;
}

wxSizer* ComputerPage::CreateButtons(wxWindow* parent)
{
	m_shutdown_btn = new wxButton(parent, wxID_ANY, wxT("Shutdown"));
	m_restart_btn = new wxButton(parent, wxID_ANY, wxT("Restart"));
	m_process_btn = new wxButton(parent, wxID_ANY, wxT("Processes"));
	m_system_btn = new wxButton(parent, wxID_ANY, wxT("System"));
	m_clear_btn = new wxButton(parent, wxID_ANY, wxT("Clear"));
	m_query_btn = new wxButton(parent, wxID_ANY, wxT("Query"));

	m_namespace_btn = new wxButton(parent, wxID_ANY, wxT("Namespaces"));
	m_namespace_list = new wxComboBox(parent, wxID_ANY);
	m_class_btn = new wxButton(parent, wxID_ANY, wxT("Classes"));
	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(m_shutdown_btn);
	sizer->Add(m_restart_btn);
	sizer->Add(m_process_btn);
	sizer->Add(m_system_btn);
	sizer->Add(m_clear_btn);
	sizer->Add(m_query_btn);
	sizer->Add(m_namespace_list);
	sizer->Add(m_namespace_btn);
	sizer->Add(m_class_btn);
	return sizer;
}

void ComputerPage::OnShutdownMachine( wxCommandEvent& event )
{
	if (m_machine_input->GetValue().length()<1)
		return;
	wxDateTime m_start_time = wxDateTime::Now();
	ULONG uCount = 1, uReturned=0;
	wxString wmi_namespace = wxString::Format(wxT("\\\\%s\\root\\cimv2"), m_machine_input->GetValue());
	try 
	{
		CComPtr<IEnumWbemClassObject> pEnumObject;
		HRESULT hRes = d->Connect( 
			wmi_namespace,
			m_user_input->GetValue(),
			m_pwd_input->GetValue());		
		hRes = d->m_Services->ExecQuery(_bstr_t(L"WQL"), 
			_bstr_t("SELECT CSName, Name,  LastBootUpTime, FreePhysicalMemory, Status  from Win32_OperatingSystem"),
			WBEM_FLAG_RETURN_IMMEDIATELY |
			WBEM_FLAG_FORWARD_ONLY,
			NULL,
			&pEnumObject);
		OZException::CheckHRESULT(__FUNCTION__, __LINE__, _T("ExecQuery"), hRes);		
		hRes = d->SetProxyBlanket(pEnumObject);		
		_bstr_t os_name;
		tstringstream& m_tss = d->m_tss;
		HRESULT& hr = hRes;
		do 
		{			
			CComPtr<IWbemClassObject> pClassObject;
			hr = pEnumObject->Next(WBEM_INFINITE,uCount, &pClassObject, &uReturned);
			OZException::CheckHRESULT(__FUNCTION__, __LINE__, _T("pEnumObject->Next(WBEM_INFINITE)"), hr);
			if (uReturned<=0)
				break;
			_variant_t   varProperty;  
			hr   =   pClassObject->Get(CComBSTR("Name"),0,&varProperty,NULL,0); 
			if ( SUCCEEDED(hr) &&  (varProperty.vt == VT_BSTR))
				os_name = varProperty;
		}while(uReturned>0);
		if (os_name.length()>1)
		{
			tstring tname = (LPCTSTR)os_name;
			boost::algorithm::replace_all(tname, _T("\\"), _T("\\\\"));
			// figure out WMI object path
			// "\\\\.\\root\\CIMV2:Win32_OperatingSystem.Name=\"Microsoft Windows XP Professional|C:\\\\WINDOWS|\\\\Device\\\\Harddisk0\\\\Partition1\"";
			tstringstream tss;
			tss << _T("\\\\") << m_machine_input->GetValue() << _T("\\root\\CIMV2:Win32_OperatingSystem.Name=\"")
				<< tname << _T("\"");
			tstring objectpath = tss.str();
			_variant_t ret_value;

// 			uint32 Win32Shutdown(sint32 Flags, sint32 Reserved ); // http://msdn.microsoft.com/en-us/library/aa394058(v=VS.85).aspx
			CComPtr<IWbemClassObject> pClass, pMethodClass, pMethodInst, pOutInst;
			CComBSTR ClassPath(_T("Win32_OperatingSystem")), MethodName("Win32Shutdown"), strObjectPath;
			strObjectPath=L"\\\\.\\root\\CIMV2:Win32_OperatingSystem.Name=\"Microsoft Windows XP Professional|C:\\\\WINDOWS|\\\\Device\\\\Harddisk0\\\\Partition1\"";
			strObjectPath = objectpath.c_str();
			hr = d->m_Services->GetObject(ClassPath, 0, 
				NULL, &pClass, NULL);
			OZException::CheckHRESULT(__FUNCTION__, __LINE__, ClassPath, hr);
			hr = pClass->GetMethod(MethodName, 0, 
				&pMethodClass, NULL); 
			OZException::CheckHRESULT(__FUNCTION__, __LINE__, MethodName, hr);
			hr = pMethodClass->SpawnInstance(0, &pMethodInst);
			OZException::CheckHRESULT(__FUNCTION__, __LINE__, _T("SpawnInstance"), hr);

			_variant_t Flags( (long)1, VT_I4);
			// Store the value for the in parameters
			hr = pMethodInst->Put(L"Flags", 0, &Flags, 0);
			OZException::CheckHRESULT(__FUNCTION__, __LINE__, _T("Set Flags parameter"), hr);
			hr = d->m_Services->ExecMethod(strObjectPath, 
				MethodName, 
				0, 
				NULL, 
				pMethodInst, 
				&pOutInst, 
				NULL);
			OZException::CheckHRESULT(__FUNCTION__, __LINE__, strObjectPath, hr);
			_variant_t varReturnValue;
			hr = pOutInst->Get(_bstr_t(L"ReturnValue"), 0, &varReturnValue, NULL, 0);
			OZException::CheckHRESULT(__FUNCTION__, __LINE__, _T("Get ReturnValue"), hr);
			m_tss << (LPCTSTR)MethodName << objectpath << _T(" return ") << std::dec<< (long)ret_value << _T(", hr = ") 
				<< std::hex << hr << std::endl;
		}
	}
	catch (const OZException& e)
	{
		d->m_tss<< _T("Exception in ") << (LPCTSTR)_bstr_t(__FUNCTION__) << _T(" ")
			<< e.ToString() << std::endl;
	}
	m_LogWnd->AppendText(d->GetString().c_str());
	DisplayDuration(__FUNCTION__, m_start_time);
}
