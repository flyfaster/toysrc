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


#ifndef CLOCK_TIMER_H
#define CLOCK_TIMER_H

class clock_timer
{
    char m_message[256] ;
    int m_log_level, _time_type;
    timespec start, end, diff;
    int m_lineNo;
    const char* m_filename;
    clock_timer(int log_level, const char* message, int time_type); //CLOCK_REALTIME, CLOCK_MONOTONIC, CLOCK_PROCESS_CPUTIME_ID, CLOCK_THREAD_CPUTIME_ID
    clock_timer(const char* filename, int log_level, const char* message, int time_type);
    clock_timer(const char* filename,int lineNo, int log_level, const char* message, int time_type);
    ~clock_timer();
    void Restart();	// initialize the start time
    void Stop();	// initialize the end time
    void Clear();	// disable printing in dtor
    timespec& GetDuration();	// calculate end - start
    int GetElapsedMilliseconds();
};

#endif // CLOCK_TIMER_H
