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

// reference: http://www.askyb.com/cpp/openssl-sha1-hashing-example-in-cpp/
#include <openssl/sha.h>
#include <curl/curl.h>
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "http_downloader.h"
#include "url_class.h"

http_downloader::http_downloader() {
	min_file_size_ = 32 * 1024;
	image_long_min_ = 640;
	image_short_min_ = 320;
	reset_digest();
}

void http_downloader::min_file_size(long fs) {
	min_file_size_ = fs;
}

http_downloader::http_downloader(const http_downloader& other)
{

}

http_downloader::~http_downloader()
{

}

http_downloader& http_downloader::operator=(const http_downloader& other)
{
    return *this;
}

bool http_downloader::operator==(const http_downloader& other) const
{
///TODO: return ...;
}

size_t callbackfunction(void *ptr, size_t size, size_t nmemb, void* userdata)
{
    if (!userdata)
    {
      std::cerr << "ERROR No stream to accept " << size * nmemb << " bytes data\n";        
        return 0;
    }  
    http_downloader* downloader=(http_downloader*)(userdata);
//     std::ofstream *pfstream=(std::ofstream*)userdata;
//     pfstream->write((const char*)ptr, size*nmemb);
//     if(!pfstream->good())
    downloader->outputfile_.write((const char*)ptr, size*nmemb);
    if(!downloader->outputfile_.good())
    {
      downloader->reset_digest();
      std::cerr << "ERROR write to file: " << size * nmemb << " bytes data\n";   
      return 0;
    }
    SHA1_Update(&downloader->ctx, ptr, size*nmemb);
    return size*nmemb;
}

void http_downloader::reset_digest()
{
	memset(digest, 0, sizeof(digest));
}

long get_file_size(std::string filename)
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

bool http_downloader::download_image(const char* surl)
{  
  SHA1_Init(&ctx);
  std::string saveas_filename=url_class::get_save_filename(surl);
  if(saveas_filename.length()==0)
    return false;
  outputfile_.open(saveas_filename.c_str(), std::ios::out | std::ios::binary);
  
  if(!outputfile_.good())
  {
    std::cout<<"failed to create file "<<saveas_filename <<std::endl;
    return false;
  }
  std::cout << __FUNCTION__ << " open " << saveas_filename<<std::endl;

    CURL* curlCtx = curl_easy_init();
    curl_easy_setopt(curlCtx, CURLOPT_URL, surl);
    curl_easy_setopt(curlCtx, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(curlCtx, CURLOPT_WRITEFUNCTION, callbackfunction);
    curl_easy_setopt(curlCtx, CURLOPT_FOLLOWLOCATION, 1);

    CURLcode rc = curl_easy_perform(curlCtx);
    if (rc)
    {
		std::cerr<< __FUNCTION__ << " ERROR download "<<surl<<std::endl;
		outputfile_.close();
		remove(saveas_filename.c_str());
		return false;
    }
    long res_code = 0;
    curl_easy_getinfo(curlCtx, CURLINFO_RESPONSE_CODE, &res_code);
    if (!((res_code == 200 || res_code == 201) && rc != CURLE_ABORTED_BY_CALLBACK))
    {
        std::cout<<"ERROR unexpected Response code:" << res_code << " for "<<surl<<std::endl;
		outputfile_.close();
		remove(saveas_filename.c_str());
        return false;
    }
    curl_easy_cleanup(curlCtx);
    long filesize = get_file_size(saveas_filename);
    if (filesize < min_file_size_) {
    	std::cout << __PRETTY_FUNCTION__ << " file size "<< filesize<< ", ignore small file " << saveas_filename << std::endl;
    	remove(saveas_filename.c_str());
    	return true;
    }
     SHA1_Final(digest, &ctx);
     int width=0, height=0;
     bool funcret = get_image_size(saveas_filename.c_str(), &width, &height);
     if(funcret) {

    	 bool goodimage = true;
    	 if (width <image_long_min_ && height < image_long_min_)
    		 goodimage = false;
    	 if (width <image_short_min_ || height < image_short_min_)
    		 goodimage = false;
    	 if(!goodimage) {
    		 remove(saveas_filename.c_str());
    		 std::cout << __func__ << " "<<saveas_filename<< " is removed, image size " << width << "x"<<height<<std::endl;
    	 } else {
    		 std::cout << __func__ << " "<<saveas_filename<< " image size " << width << "x"<<height<<std::endl;
    	 }

     }
     else
    	 std::cout << __func__ << " "<<saveas_filename<< " failed to get image size " <<std::endl;
    return true;
}

void http_downloader::image_short_min(long short_min) {
	image_short_min_ = short_min;
}

void http_downloader::image_long_min(long long_min) {
	image_long_min_ = long_min;
}
