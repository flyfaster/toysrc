#include "TheApp.h"
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/evtloop.h>
CTheApp theApp;
// A dummy CWnd pointing to a wxWindow's HWND
class CDummyWindow: public CWnd
{
public:
	CDummyWindow(HWND hWnd);
	~CDummyWindow(void);
};

CDummyWindow::CDummyWindow(HWND hWnd)
{
	Attach(hWnd);
}

// Don't let the CWnd destructor delete the HWND
CDummyWindow::~CDummyWindow()
{
	Detach();
}


BOOL CTheApp::InitInstance()
{
	if ( !CWinApp::InitInstance() )
		return FALSE;

	// TODO: cmd line parsing
	WXDLLIMPEXP_BASE void wxSetInstance(HINSTANCE hInst);
	wxSetInstance(m_hInstance);
	wxApp::m_nCmdShow = m_nCmdShow;
	int argc = 0;
	wxChar **argv = NULL;
	wxEntryStart(argc, argv);
	if ( !wxTheApp || !wxTheApp->CallOnInit() )
		return FALSE;

	// Demonstrate creation of an initial wxWidgets main window.
	// Wrap wxWidgets window in a dummy MFC window and
	// make the main window.
	if (wxTheApp && wxTheApp->GetTopWindow())
	{
		m_pMainWnd = new CDummyWindow((HWND) wxTheApp->GetTopWindow()->GetHWND());
	}

	return TRUE;
}

int CTheApp::ExitInstance()
{
#if !START_WITH_MFC_WINDOW
	delete m_pMainWnd;
#endif

	if ( wxTheApp )
		wxTheApp->OnExit();
	wxEntryCleanup();

	return CWinApp::ExitInstance();
}

// Override this to provide wxWidgets message loop compatibility
BOOL CTheApp::PreTranslateMessage(MSG *msg)
{
	//wxEventLoop *evtLoop = wxEventLoop::GetActive();
	//if ( evtLoop && evtLoop->PreProcessMessage(msg) )
	//	return TRUE;

	return CWinApp::PreTranslateMessage(msg);
}

BOOL CTheApp::OnIdle(LONG WXUNUSED(lCount))
{
	return wxTheApp && wxTheApp->ProcessIdle();
}