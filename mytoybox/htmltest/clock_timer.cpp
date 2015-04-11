/*
    Copyright (c) 2013 <copyright holder> <email>

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use,
    copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following
    conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
*/
#include <string.h>
#include <time.h>
#include "clock_timer.h"

void clock_timer::Stop()
{
	clock_gettime(_time_type, &end);
}

clock_timer::clock_timer(int log_level, const char* message, int time_type)
	:m_log_level(log_level),m_lineNo(0), m_filename(NULL)
{
	strncpy ( m_message, message, sizeof(m_message) );
	m_message[sizeof(m_message)-1]='\0'; // make sure it is terminated with null
	Clear();
	clock_gettime(time_type, &start); _time_type=time_type; 
}

clock_timer::clock_timer(const char* filename, int log_level, const char* message, int time_type)
	:m_log_level(log_level),m_lineNo(0), m_filename(filename)
{
	strncpy ( m_message, message, sizeof(m_message) );
	m_message[sizeof(m_message)-1]='\0'; // make sure it is terminated with null
	Clear();
	clock_gettime(time_type, &start); _time_type=time_type;
}

clock_timer::clock_timer(const char* filename, int lineNo, int log_level, const char* message, int time_type)
	:m_log_level(log_level),m_lineNo(lineNo), m_filename(filename)
{
	strncpy ( m_message, message, sizeof(m_message) );
	m_message[sizeof(m_message)-1]='\0'; // make sure it is terminated with null
	Clear();
	clock_gettime(time_type, &start); _time_type=time_type;
}

clock_timer::~clock_timer()
{
	if((start.tv_nsec==0) && (start.tv_sec==0))
		return;
	if((end.tv_nsec==0) && (end.tv_sec==0))
	clock_gettime(_time_type, &end);	
}

void clock_timer::Restart() {
	Clear();
	clock_gettime(_time_type, &start);
}

timespec& clock_timer::GetDuration() {
	if (diff.tv_nsec || diff.tv_sec)
    		 return diff;
	if((end.tv_nsec==0) && (end.tv_sec==0))
		clock_gettime(_time_type, &end);

		if ((end.tv_nsec-start.tv_nsec)<0) {
			diff.tv_sec = end.tv_sec-start.tv_sec-1;
			diff.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
		} else {
			diff.tv_sec = end.tv_sec-start.tv_sec;
			diff.tv_nsec = end.tv_nsec-start.tv_nsec;
		}
		return diff;
}

void clock_timer::Clear() {
	start.tv_nsec = start.tv_sec = 0;
	diff.tv_nsec = diff.tv_sec =0;
    	end.tv_nsec = end.tv_sec = 0;
}

int clock_timer::GetElapsedMilliseconds()
{
	return GetDuration().tv_sec*1000 + GetDuration().tv_nsec/1000000;
}