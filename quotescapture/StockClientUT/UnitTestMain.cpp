
#include "../stdafx.h"
#include "StockClientUT.h"
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/ui/mfc/TestRunner.h>
#include <winsock2.h>
#include <wx/app.h>
#include <string>
#include "../MainApp.h"

using namespace std;

//std::string wx2std(wxString s){
//	std::string s2;
//	if(s.wxString::IsAscii()) {
//		s2=s.wxString::ToAscii();
//	} else {
//		const wxWX2MBbuf tmp_buf = wxConvCurrent->cWX2MB(s);
//		const char *tmp_str = (const char*) tmp_buf;
//		s2=std::string(tmp_str, strlen(tmp_str));
//	}
//	return s2;
//} 

VOID CALLBACK TimerProc(HWND hwnd,
                        UINT uMsg,
                        UINT_PTR idEvent,
                        DWORD dwTime
                        );

BOOL CALLBACK EnumWindowsProc(HWND hwnd,
                              LPARAM lParam
                              );
BOOL CALLBACK EnumWindowsProc(HWND hwnd,
                              LPARAM lParam
                              )
{
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (GetCurrentProcessId()==pid)
    {
        TCHAR buf[MAX_PATH];
		_stprintf_s(buf, MAX_PATH, _T("%s-%d"), wxString::FromAscii(STOCK_CLIENT_UT_SUITE), pid);
        SetWindowText(hwnd, buf);
        KillTimer(NULL, lParam);
        return FALSE;
    }
    return TRUE;
}

VOID CALLBACK TimerProc(HWND hwnd,
                        UINT uMsg,
                        UINT_PTR idEvent,
                        DWORD dwTime
                        )
{
    EnumWindows(EnumWindowsProc, NULL);
}

class CGUITesterRunnerApp : public CWinApp
{
public:
    CGUITesterRunnerApp(){};
    virtual BOOL InitInstance();
	virtual int ExitInstance();
};

BOOL CGUITesterRunnerApp::InitInstance()
{
    UINT_PTR m_timer = 0;
    m_timer = SetTimer(NULL, 100, 1000, TimerProc);
    AfxEnableControlContainer();
    CppUnit::MfcUi::TestRunner runner;
    CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry(
		wx2std(wxString::FromAscii(STOCK_CLIENT_UT_SUITE)) );
    runner.addTest(registry.makeTest());
    runner.run();	
    EnumWindows(EnumWindowsProc, m_timer);
    return FALSE;
}

int CGUITesterRunnerApp::ExitInstance()
{
	//release global singleton object
	return 0;
}

CGUITesterRunnerApp theApp;

struct WinSockInit
{
public:
    WinSockInit()
    {
        WSADATA wsaData;
        m_error_code = WSAStartup(MAKEWORD(2,2), &wsaData);
    }
    ~WinSockInit()
    {
        WSACleanup();
    }
    int m_error_code;
} sock_initializer;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
    if (sock_initializer.m_error_code!=NO_ERROR)
        return sock_initializer.m_error_code;
    int nRetCode = 0;

    // initialize MFC and print and error on failure
    if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
    {
        // TODO: change error code to suit your needs
        _tprintf(_T("Fatal Error: MFC initialization failed\n"));
        nRetCode = 1;
    }
    else
    {
        // TODO: code your application's behavior here.
        printf("TestCppUnit Start!...\n");

        //	Create the event manager and test controller
        CPPUNIT_NS::TestResult controller;

        // Add a listener that collects test result
        CPPUNIT_NS::TestResultCollector result;
        controller.addListener( &result );        

        // Add a listener that print dots as test run.
        CPPUNIT_NS::BriefTestProgressListener progress;
        controller.addListener( &progress );      

        // Add the top suite to the test runner
        CPPUNIT_NS::TestRunner runner;

        CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry(
			wx2std(wxString::FromAscii(STOCK_CLIENT_UT_SUITE)));
        runner.addTest(registry.makeTest());
        runner.run( controller );


        // Print test in a compiler compatible format.
        CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
        outputter.write(); 

		//release global singleton object

        printf("TestCppUnit Console Mode Finish!\n");

        nRetCode = result.wasSuccessful() ? 0 : 1;
    }
    if (IsDebuggerPresent() && nRetCode)
    {
        int sleep_seconds = 8;
        std::cout << "Sleep " << sleep_seconds << " seconds to display error!" << std::endl;
        Sleep(1000*sleep_seconds);
    }
    return nRetCode;
}

//void DebugPrint(char * format, ...)
//{
//#ifndef   VXWORKS
//    char tmp_buf[512] = {0};
//    va_list args;
//    int     len=0;
//    char    *buffer=0;
//
//    // retrieve the variable arguments
//    va_start( args, format );
//
//    len = _vscprintf( format, args ) // _vscprintf doesn't count
//        + 1; // terminating '\0'
//    if (len<sizeof(tmp_buf))
//        buffer = tmp_buf;
//    else
//        buffer = (char*)malloc( len * sizeof(char) );
//
//    vsprintf( buffer, format, args ); // C4996
//    // Note: vsprintf is deprecated; consider using vsprintf_s instead
//    OutputDebugString(buffer);
//    if (buffer != tmp_buf)
//        free( buffer );
//#endif
//}

std::basic_string<TCHAR> GetExecutablePath()
{
    TCHAR module_buf[MAX_PATH+1024] = {0};
    int module_name_len = GetModuleFileName(NULL, module_buf, sizeof(module_buf));
    while (module_name_len>0)
    {
        if (module_buf[module_name_len]==_T('\\'))
        {
            module_buf[module_name_len+1] = 0;
            break;
        }
        module_name_len--;
    }
    return module_buf;
}

int GetParameter(const TCHAR* section, const TCHAR* parameter_name, int default_value)
{
    std::basic_string<TCHAR> cfg_name = GetExecutablePath();
	cfg_name = cfg_name + std::basic_string<TCHAR>(wxString::FromAscii(STOCK_CLIENT_UT_SUITE));
	cfg_name += _T(".ini");
    int nret = GetPrivateProfileInt(section, parameter_name, default_value, cfg_name.c_str());
    if (nret == default_value)
    {
        TCHAR buf[16];
        _itot_s(default_value, buf, 15, 10);
        WritePrivateProfileString(section, parameter_name, buf, cfg_name.c_str());
    }
    return nret;
}

std::basic_string<TCHAR> GetParameter(const TCHAR* section, const TCHAR* parameter_name, 
									  const std::basic_string<TCHAR>& default_value)
{
    std::basic_string<TCHAR> cfg_name = GetExecutablePath();
	cfg_name = cfg_name + std::basic_string<TCHAR>(wxString::FromAscii(STOCK_CLIENT_UT_SUITE));
	cfg_name += _T(".ini");
    TCHAR buf[1024];
    int nret = GetPrivateProfileString(section, parameter_name, default_value.c_str(), 
        buf, sizeof(buf),
        cfg_name.c_str());
    if (default_value == buf)
    {
        WritePrivateProfileString(section, parameter_name, default_value.c_str(), 
			cfg_name.c_str());
    }
    return buf;
}

std::basic_string<TCHAR> GetModuleName()
{
	TCHAR event_name_buf[MAX_PATH]={0};
	std::basic_string<TCHAR> module_name = _T("main"); // initial value for Embedded which only run single process.
	DWORD nSize = sizeof(event_name_buf)/sizeof(*event_name_buf)-1;
	DWORD ret = GetModuleFileName(NULL, event_name_buf, nSize);
	if (ret < nSize)
	{
		ret--;  // trim the path, only left file name part
		while (event_name_buf[ret]!=_T('\\') && ret>0)
			ret--;
		module_name= (event_name_buf+ret+1);
	}
	return module_name;
}
