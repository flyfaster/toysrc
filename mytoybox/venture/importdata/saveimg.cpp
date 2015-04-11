/*
 * saveimg.cpp
 *
 *  Created on: Feb 3, 2015
 *      Author: onega
 */

// wget -O /cygdrive/c/tmp/test.png "http://chart.finance.yahoo.com/z?s=GOOG&t=1d&q=l&l=on&z=l"
#include <chrono>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <array>
// https://code.google.com/p/yahoo-finance-managed/wiki/miscapiImageDownload
// g++ -std=c++14 -g -o saveimg saveimg.cpp
std::string run_cmd(const std::string cmdstr)
{
  std::stringstream ss;
  std::array<char, 2048> buffer;
    FILE *stream =popen(cmdstr.c_str(), "r");
    if (stream){
       while (!feof(stream))
       {
            if (fgets(buffer.data(), buffer.size(), stream) != NULL)
            {
	       		ss << buffer.data();
            }
	    //usleep(1);
       }
       pclose(stream);
    }
    return ss.str();
}

int saveimg(const std::string symbol)
{
	std::chrono::system_clock::time_point now =
				std::chrono::system_clock::now();
	time_t tt = std::chrono::system_clock::to_time_t(now);
			tm local_tm = *localtime(&tt);
	std::stringstream ss;
	ss <<"wget -O " << local_tm.tm_year + 1900
			<< std::setw(2) << std::setfill('0') << local_tm.tm_mon + 1
			<< std::setw(2)	<< std::setfill('0') << local_tm.tm_mday
	<< "-"
	<< symbol << ".png "
	<< "\"http://chart.finance.yahoo.com/z?s=" << symbol <<"&t=1d&q=l&l=on&z=l\"";
	std::cout << run_cmd(ss.str());
	return 0;
}

int main(int argc, char* argv[])
{
	saveimg("GOOG");
	saveimg("MSFT");
	saveimg("C");
	saveimg("QQQ");
	return 0;
}


