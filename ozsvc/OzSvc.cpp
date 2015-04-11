// OzSvc.cpp : Implementation of WinMain

#include "stdafx.h"
#include "resource.h"
#include "OzSvc_i.h"

#include <stdio.h>
#include <sstream>
#include <xutility>
#include <exception>
#include <iomanip>
#include "TcpServer.h"


typedef std::basic_stringstream<TCHAR> tsstream;
typedef std::basic_string<TCHAR> tstring;
#ifdef _UNICODE
#define __FUNCTIONT__ __FUNCTIONW__
#else
#define __FUNCTIONT__ __FUNCTION__
#endif

class COzSvcModule : public CAtlServiceModuleT< COzSvcModule, IDS_SERVICENAME >
{
public :
	DECLARE_LIBID(LIBID_OzSvcLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_OZSVC, "{08E6E609-0FBD-4098-8006-D703FCEB8C17}")
	HRESULT InitializeSecurity() throw()
	{
		// TODO : Call CoInitializeSecurity and provide the appropriate security settings for 
		// your service
		// Suggested - PKT Level Authentication, 
		// Impersonation Level of RPC_C_IMP_LEVEL_IDENTIFY 
		// and an appropiate Non NULL Security Descriptor.
		CSecurityDescriptor sd;
		sd.InitializeFromThreadToken();
		HRESULT hr = CoInitializeSecurity(sd, -1, NULL, NULL,
			RPC_C_AUTHN_LEVEL_PKT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);

		return S_OK;
	}
	COzSvcModule()
	{
		m_status.dwControlsAccepted |= SERVICE_ACCEPT_PAUSE_CONTINUE|SERVICE_ACCEPT_STOP|SERVICE_ACCEPT_SHUTDOWN;
	}
	HRESULT RegisterAppId(bool bService=false)
	{
		HRESULT hr = S_OK;
		hr = __super::RegisterAppId(bService);
		if (!bService)
			return hr;
		//Changing the Startup type and description of the service
		SC_HANDLE hSCM = NULL;
		SC_LOCK sclLock = NULL;
		SC_HANDLE hService = NULL;
		try
		{
			hSCM = ::OpenSCManager(NULL, NULL, GENERIC_WRITE | SC_MANAGER_LOCK);
			if (hSCM == NULL)
			{
				throw std::runtime_error("Couldn't open service manager");
			}
			sclLock = LockServiceDatabase(hSCM);
			if(sclLock == NULL)
			{
				throw std::runtime_error("Couldn't Lock Service Database");
			}

			hService = OpenService(
				hSCM, // SCManager database
				m_szServiceName, // name of service
				SERVICE_CHANGE_CONFIG); // need CHANGE access
			if (hService == NULL)
			{
				throw std::runtime_error("Couldn't Open Service");
			}

			if (! ChangeServiceConfig(
				hService, // handle of service
				SERVICE_WIN32_OWN_PROCESS, // service type: no change
				SERVICE_AUTO_START, // change service start type
				SERVICE_NO_CHANGE, // error control: no change
				NULL, // binary path: no change
				NULL, // load order group: no change
				NULL, // tag ID: no change
				NULL, // dependencies: no change
				NULL, // account name: no change
				NULL, // password: no change
				NULL) ) // display name: no change
			{
				throw std::runtime_error("Couldn't Change Service Config");
			}
			// To Change the description of the service
			SERVICE_DESCRIPTION sdBuf;
			memset(&sdBuf, 0, sizeof(sdBuf));
			sdBuf.lpDescription = _T("Description of My Auto Start Service");
			if( !ChangeServiceConfig2(
				hService, // handle to service
				SERVICE_CONFIG_DESCRIPTION, // change: description
				&sdBuf) ) // value: new description
			{
				throw std::runtime_error("Change Service Config Failed");
			}
		}
		catch (const std::runtime_error& e)
		{
			hr = HRESULT_FROM_WIN32( GetLastError());
			std::stringstream ss;
			ss << "Error code:" << std::hex << std::setfill('0') << std::setw(8) << std::internal 
				<< hr << ", description: " << e.what();
			OutputDebugStringA(ss.str().c_str());
		}
		if (sclLock)
			UnlockServiceDatabase(sclLock);
		if (hService)
			::CloseServiceHandle(hService);
		if (hSCM)
			::CloseServiceHandle(hSCM);
		return hr;
	} 

    void OnStop() throw()
    {
        SingleTcpServer::Instance().stop();
        __super::OnStop();
    }

    void OnPause() throw()
    {
    }

    void OnContinue() throw()
    {
        SingleTcpServer::Instance().run();
    }

    void OnInterrogate() throw()
    {
    }

    void OnShutdown() throw()
    {
        OnStop();
    }

};

#ifndef _CONSOLE
#endif
COzSvcModule _AtlModule;
//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
                                LPTSTR /*lpCmdLine*/, int nShowCmd)
{
    return _AtlModule.WinMain(nShowCmd);
}

boost::function0<void> console_ctrl_function;

BOOL WINAPI console_ctrl_handler(DWORD ctrl_type)
{
    switch (ctrl_type)
    {
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
    case CTRL_CLOSE_EVENT:
    case CTRL_SHUTDOWN_EVENT:
        console_ctrl_function();
        return TRUE;
    default:
        return FALSE;
    }
}

int main(int argc, char* argv[])
{
    for (int i=0; i<argc; i++)
        std::cout << "argv[" << i << "]: " << argv[i] << std::endl;
    if (argc>1)
        return 0;
    try
    {
        //TcpServer& server = SingleTcpServer::Instance();
        // Set console control handler to allow server to be stopped.
        console_ctrl_function = boost::bind(&TcpServer::stop, &SingleTcpServer::Instance());
        SetConsoleCtrlHandler(console_ctrl_handler, TRUE);

        // Run the server until stopped.
        SingleTcpServer::Instance().run();
    }
    catch (std::exception& e)
    {
        std::cerr << "exception: " << e.what() << "\n";
    }
    std::cout << "Program stopped!" << std::endl;
    return 0;
}
