/*
 * wxdumpsyms.cpp
 *
 *  Created on: Feb 2, 2015
 *      Author: onega
 */
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/datetime.h"
#include "wx/image.h"
#include "wx/bookctrl.h"
#include "wx/artprov.h"
#include "wx/imaglist.h"
#include "wx/sysopt.h"
#include "wxdumpsyms.h"
#include "MainWnd.h"
IMPLEMENT_APP(wxdumpsyms)

wxdumpsyms::wxdumpsyms() {
	// TODO Auto-generated constructor stub

}

wxdumpsyms::~wxdumpsyms() {
	// TODO Auto-generated destructor stub
}

bool wxdumpsyms::OnInit() {
	wxFrame* m_find_dlg=new MainWnd(NULL);
    m_find_dlg->Center();
    m_find_dlg->Show(true);
    SetTopWindow(m_find_dlg);
    return true;
}

int wxdumpsyms::OnExit() {
	//      wxLogMessage(_T("GuiFindApp::OnExit()"));
	        wxLog::FlushActive();
	        return 0;
}
