/*
 * FindThread.h
 *
 *  Created on: 8 Apr 2010
 *      Author: onega
 */

#ifndef FINDTHREAD_H_
#define FINDTHREAD_H_
#include <string>
typedef std::basic_string<wxChar> std_string;
class FindThread {
public:
	FindThread();
	virtual ~FindThread();
	int operator()();
	void Run();
	void Stop(bool bval);
	bool Continue();
	std_string m_file_name_pattern;
	std_string m_root_path;
	std_string m_content_pattern;

	volatile bool m_stop;
};
void dummy_thread_func();
#endif /* FINDTHREAD_H_ */
