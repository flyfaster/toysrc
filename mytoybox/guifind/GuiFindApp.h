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
#include <map>
#include <atomic>
#include <list>

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
	void StartFind();
	GuiFindDlg *m_find_dlg;
	VerboseDialog *m_log_wnd;
	std::map<std_string, std_string> properties;
	void SetProperty(std_string key, std_string value);
	std::atomic<int> findthreadcount;
	std::atomic<bool> needrefresh;
	boost::mutex m_result_mx;
	std::list<wxString> m_result_list;
};
DECLARE_APP(GuiFindApp)
#endif /* GUIFINDAPP_H_ */
