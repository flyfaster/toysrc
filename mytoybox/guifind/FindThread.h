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
std::wstring ToStdWstring(const std::string& str);

class FindThread {
public:
	FindThread();
	virtual ~FindThread();
	int operator()();
	void Run();
	void Stop(bool bval);
	bool Continue();
	void Work();
	void Publish();
	std_string m_file_name_pattern;
	std_string m_file_name_pattern_exclude;
	std_string m_root_path;
	std_string m_content_pattern;
	std::deque<std_string> dirs;
	std::deque<std_string> match_files_list;
	std::unordered_set<std_string> visited;
	std::vector<boost::wregex> exclude_patterns, expect_patterns;
	int total_file_count;
	int match_file_count;
	int exclude_count;
	volatile bool m_stop;
};
void dummy_thread_func();
#endif /* FINDTHREAD_H_ */
