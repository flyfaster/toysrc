/*
 * GuiFindApp.h
 *
 *  Created on: 7 Apr 2010
 *      Author: onega
 */

#ifndef GUIFINDAPP_H_
#define GUIFINDAPP_H_
#include <boost/thread.hpp>
#include <string>
#include <boost/shared_ptr.hpp>

typedef std::basic_string<wxChar> std_string;
class FindThread;
class GuiFindDlg;
class VerboseDialog;
class GuiFindApp : public wxApp {
public:
	GuiFindApp();
	virtual ~GuiFindApp();
	bool OnInit();
	int OnExit();
	boost::shared_ptr<boost::thread> m_find_thread;
	FindThread* m_find_handler;
	void StartFind(std_string filenamepatter,
			std_string contentpattern,
			std_string rootpath);
	GuiFindDlg *m_find_dlg;
	VerboseDialog *m_log_wnd;
};
DECLARE_APP(GuiFindApp)
#endif /* GUIFINDAPP_H_ */
