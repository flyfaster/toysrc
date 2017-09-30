/*
 * FindThread.cpp
 *
 *  Created on: 8 Apr 2010
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
#include "wx/gbsizer.h"
#include <wx/textctrl.h>
#include "wx/dirdlg.h"

#include <iostream>
#include <locale>
#include <deque>
#include <vector>
#include <unordered_set>
#include <chrono>
#include <boost/thread.hpp>
#include <boost/regex.hpp>
#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>
#include <boost/range/iterator_range.hpp>
#include "GuiFindDlg.h"
#include "GuiFindApp.h"
#include "FindThread.h"

void LogMessage(wxString msg)
{
	wxGetApp().m_find_dlg->AppendResult(msg);
}

FindThread::FindThread() {
	m_stop=false;
//	wxString msg;
//	msg.Printf(_T("FindThread::FindThread(%p)"), this);
//	LogMessage(msg);
}

FindThread::~FindThread() {
	LogMessage(_T("FindThread::~FindThread()"));
}

int FindThread::operator()()
{
	Run();
	wxGetApp().findthreadcount--;
	return 0;
}

void thread_sleep(int secs) {
	boost::xtime xt;
	boost::xtime_get(&xt, boost::TIME_UTC_);
	xt.sec += secs;
	boost::thread::sleep(xt);
}

std::wstring ToStdWstring(const std::string& str) {
	if(str.length()<1)
		return std::wstring();
	std::wstring wstr(str.size(), L' ');
	std::mbstowcs(&wstr[0], str.c_str(), str.size());
//    std::wstring_convert< std::codecvt<wchar_t,char,std::mbstate_t> > conv ;
//    std::wstring wstr = conv.from_bytes(str) ;
    return wstr;
}

void prepare_regex_patterns(std::vector<boost::wregex>& exclude_patterns, std_string src) {
	  typedef boost::tokenizer<boost::char_separator<wxChar>,std::wstring::const_iterator, std::wstring >
	    tokenizer;
	  boost::char_separator<wxChar> sep(_T(":;"));
	  {
		  tokenizer tokens(src, sep);
		  for (tokenizer::iterator tok_iter = tokens.begin();
			   tok_iter != tokens.end(); ++tok_iter)
		  {
			  std::wcout << *tok_iter << std::endl;
			  exclude_patterns.push_back(boost::wregex(*tok_iter, boost::regex::icase));
		  }
	  }
}

bool regex_match(std::vector<boost::wregex>& exclude_patterns, std_string entry_path_str) {
	bool exclude_match = false;
	boost::wsmatch excludewhat;
    // what[0] contains the whole string
    // what[1] contains the response code
    // what[2] contains the separator character
    // what[3] contains the text message.
	for(auto exclude: exclude_patterns) {
		if(boost::regex_match( entry_path_str, excludewhat, exclude)) {
			exclude_match = true;
			break;
		}
	}
	return exclude_match;
}

void FindThread::Run()
{
    auto t1 = std::chrono::high_resolution_clock::now();
    match_files_list.clear();
    exclude_patterns.clear();
    expect_patterns.clear();
    dirs.clear();
    visited.clear();
//	LogMessage(_T("FindThread::operator()() start"));
	total_file_count=0; match_file_count = 0; exclude_count = 0;

	typedef boost::tokenizer<boost::char_separator<wxChar>,std::wstring::const_iterator, std::wstring > tokenizer;
	boost::char_separator<wxChar> sep(_T(":;"));
	{
		tokenizer tokens(m_root_path, sep);
		for (tokenizer::iterator tok_iter = tokens.begin();
		tok_iter != tokens.end(); ++tok_iter)
		{
			std::wcout << *tok_iter << std::endl;
			dirs.push_back(*tok_iter);
			if(visited.count(*tok_iter)==0)
				visited.insert(*tok_iter);
		}
	}
	prepare_regex_patterns(exclude_patterns, m_file_name_pattern_exclude);
	prepare_regex_patterns(expect_patterns, m_file_name_pattern);

	while(Continue() && dirs.size()) {
		try {
			Work();
		} catch(boost::filesystem::filesystem_error& err) {
			std::cerr << err.what() << std::endl;
		}
	}
	Publish();
    auto t2 = std::chrono::high_resolution_clock::now();
    // integral duration: requires duration_cast
    auto int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
	wxString msg;
	msg.Printf(_T("FindThread operation completed in %li milliseconds total_file_count=%d match_file_count=%d exclude_count=%d"),
			int_ms.count(), total_file_count, match_file_count, exclude_count);
	LogMessage(msg);
}

void FindThread::Stop(bool bval)
{
	m_stop=bval;
}

bool FindThread::Continue()
{
	return !m_stop;
}

void FindThread::Work() {
	while(dirs.size()) {
		std_string onedir = *dirs.begin();
		dirs.pop_front();

		boost::filesystem::path p(onedir); // should split to multiple path
        if(!Continue()) {
    		wxString msg;
    		msg.Printf(_T("FindThread::operator()() operation cancelled in %p total_file_count=%d match_file_count=%d"), this, total_file_count, match_file_count);
    		LogMessage(msg);
        	break;
        }
	    if(boost::filesystem::is_directory(p)) {
		        for(auto& entry : boost::make_iterator_range(boost::filesystem::directory_iterator(p), {})) {
	//	            std::cout << entry << "\n";
		            if(!Continue()) {
		        		wxString msg;
		        		msg.Printf(_T("FindThread::operator()() operation cancelled in %p total_file_count=%d match_file_count=%d"), this, total_file_count, match_file_count);
		        		LogMessage(msg);
		            	break;
		            }
		            std_string entry_path_str = ToStdWstring(entry.path().string());
		            if ( boost::filesystem::is_directory(entry.status()) )
					{
		      		  if(visited.count(entry_path_str)==0) {
		      			  visited.insert(entry_path_str);
		      			  dirs.push_back(entry_path_str);
		      		  }
					}
		            bool expect_match=true;
		            if(expect_patterns.size())
		            	expect_match=regex_match(expect_patterns, entry_path_str);
		            if(expect_match)
		            {
		            	bool exclude_match = regex_match(exclude_patterns, entry_path_str);
		            	if(exclude_match) {
		            		exclude_count++;
		            	}
		            	if(!exclude_match) {
		            		match_files_list.push_back(entry_path_str);	// should move this list to GUI in batch to improve performance
//							wxGetApp().m_find_dlg->AppendResult(entry_path_str);
							match_file_count++;
		            	}
		            	if(wxGetApp().needrefresh.exchange(false)) {
		            		Publish();
		            	}
		            }
		            total_file_count++; // how many files searched
		        }
	    } else {
	    	std::cout << p << " is NOT a directory \n";
	    }
	}
}

void FindThread::Publish() {
//	boost::mutex::scoped_lock lk(wxGetApp().m_result_mx);
//	for(auto onepath: match_files_list)
//		wxGetApp().m_result_list.push_back(onepath);
//	match_files_list.clear();
}
//void dummy_thread_func()
//{
//	wxGetApp().m_find_dlg->AppendResult(_T("dummy_thread_func() start"));
//	boost::xtime xt;
//	boost::xtime_get(&xt, boost::TIME_UTC_);
//	xt.sec += 1;
//	boost::thread::sleep(xt);
//	wxGetApp().m_find_dlg->AppendResult(_T("dummy_thread_func() end"));
//}
