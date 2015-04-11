// OzSvcClient.cpp : Defines the class behaviors for the application.
// 

#include "stdafx.h"
#include "OzSvcClient.h"
#include "OzSvcClientDlg.h"
#include "StdRegProv.h"
#include "Win32_Service.h"
#include "WMIHelper.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// OzIPCApp

BEGIN_MESSAGE_MAP(OzIPCApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// OzIPCApp construction

OzIPCApp::OzIPCApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only OzIPCApp object

OzIPCApp theApp;


// OzIPCApp initialization

BOOL OzIPCApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);
	CWinApp::InitInstance();
	AfxSocketInit(NULL);
	AfxEnableControlContainer();
	OzIPCDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}


int main(int argc, char* argv[])
{
	HRESULT hres = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hres))
	{
		std::cout << "Operation failed at " << __FILE__ << "[" << __LINE__
			<< "] " << std::hex << hres << std::endl;
		return hres;
	}
	hres =  CoInitializeSecurity(NULL, -1, NULL, NULL,
		RPC_C_AUTHN_LEVEL_CONNECT, 
		RPC_C_IMP_LEVEL_IDENTIFY, 
		NULL, EOAC_NONE, 0
		);
	if (FAILED(hres))
	{
		std::cout << "Operation failed at " << __FILE__ << "[" << __LINE__
			<< "] " << std::hex << hres << std::endl;
		return hres;
	}
	//{
	//	WMIHelper wmi_svc;
	//	wmi_svc.Connect("\\\\bajie.ent.rt.verigy.net\\root\\cimv2",
	//		"ent\\onega.zhang",
	//		"Winter125");
	//	CWin32_Service services(&wmi_svc);
	//	hres = services.StopService("Messenger");
	//	std::cout << "Stop Messenger return " << std::hex << hres << std::endl;
	//	hres = services.StartService("Messenger");
	//	std::cout << "Start Messenger return " << std::hex << hres << std::endl;
	//	return 0;
	//}

	//control_service(L"bajie.ent.rt.verigy.net", L"ENT", L"onega.zhang", 
	//					L"Winter125", L"Telnet", L"StartService");
	{
		WMIHelper wmi_svc;
		wmi_svc.Connect("\\\\bajie.ent.rt.verigy.net\\root\\default",
			"ent\\onega.zhang",
			"Winter125");
		CStdRegProv remote_reg(&wmi_svc);
		int ret = 0;
		//if (argc == 4)
		//{
		//	ret = remote_reg.Connect(argv[1],"", argv[2], argv[3]);
		//	std::cout << "Connect  " << argv[1] << " return " << std::hex << ret << std::endl;

		//}
		//else
		//{
		//	ret = remote_reg.Connect("\\\\.\\root\\default", "", "", "");
		//	printf("Usage: %s <wmi namespace> <User name> <Password>\n", argv[0]);

		//}

		ret = remote_reg.SetDWORDValue((unsigned)HKEY_LOCAL_MACHINE, "SOFTWARE\\MPICH2", 
			"testSetDWORDValue", GetTickCount());
		std::cout << "SetDWORDValue returns  " << std::hex << ret << std::endl;

		ret = remote_reg.SetStringValue((unsigned)HKEY_LOCAL_MACHINE, "SOFTWARE\\MPICH2", 
			"testSetStringValue", __FUNCTION__);
		std::cout << "SetStringValue returns  " << std::hex << ret << std::endl;

		DWORD data = 0;
		ret = remote_reg.GetDWORDValue((unsigned)HKEY_LOCAL_MACHINE, "SOFTWARE\\MPICH2",
			"testSetDWORDValue", data);
		std::cout << "GetDWORDValue returns  " << std::hex << ret << std::endl;
		std::cout << "data = " << std::dec << data << std::endl;

		std::string sValue;
		ret = remote_reg.GetStringValue((unsigned)HKEY_LOCAL_MACHINE, "SOFTWARE\\MPICH2", 
			"testSetStringValue", sValue);
		std::cout << "GetStringValue returns  " << std::hex << ret << std::endl;
		std::cout << "sValue = " << sValue << std::endl;

	}

	CoUninitialize();
	return 0;
}