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
#include <sstream>
#include <iomanip>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <chrono>
#include "resource_database.h"
#include "http_downloader.h"
#include "url_class.h"
#include "digestclass.h"

http_downloader::http_downloader() {
	min_file_size_ = 32 * 1024;
	image_long_min_ = 640;
	image_short_min_ = 320;
	db_ = nullptr;
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
      std::cerr << __func__<< " ERROR No stream to accept " << size * nmemb << " bytes data\n";
        return 0;
    }  
    http_downloader* downloader=(http_downloader*)(userdata);
    downloader->outputfile_.write((const char*)ptr, size*nmemb);
    if(!downloader->outputfile_.good())
    {
      downloader->reset_digest();
      std::cerr << __func__<< " ERROR write to file: " << size * nmemb << " bytes data\n";
      return 0;
    }
    SHA1_Update(&downloader->ctx, ptr, size*nmemb);
    return size*nmemb;
}

void http_downloader::reset_digest()
{
	memset(digest.data(), 0, sizeof(digest));
}

long get_file_size(std::string filename)
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

std::string contents;
// This is the callback function that is called by
// curl_easy_perform(curl)
size_t handle_data(void *ptr, size_t size, size_t nmemb, void *stream) {
	const boost::regex content_length_regex("Content-Length: [0-9]{1,}"); // Regex do video do youtube...
	const boost::regex content_length_remove_regex("Content-Length: ");
	int numbytes = size * nmemb;
	char lastchar = *((char *) ptr + numbytes - 1);
	std::string nothing = "";
	*((char *) ptr + numbytes - 1) = '\0';
	std::string last_char = ((char *) ptr);
	if (regex_search(last_char, content_length_regex) == 1)
			{
		contents = regex_replace(last_char, content_length_remove_regex,
				nothing);
	} else
		return size * nmemb;
}

template<typename T>
using deleted_unique_ptr = std::unique_ptr<T,std::function<void(T*)>>;

long remote_filesize(std::string url) {
	contents.clear();
	CURL *curl;
	CURLcode res;
	double length = 0.0;
	curl = curl_easy_init();
	deleted_unique_ptr<CURL> scopedcurl(
	    curl,
	    [](CURL* curlinst) { curl_easy_cleanup(curlinst); });
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, handle_data);
		curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 500);
		res = curl_easy_perform(curl);
	}
	if (contents.size()<1) {
		return (-1);
	} else {
		return atol(contents.c_str());
	}
}

bool http_downloader::download_image(const char* surl)
{  
	long remotesize = remote_filesize(surl);
    if (remotesize>1 && remotesize < min_file_size_) {
    	std::cout << __METHOD_NAME__ << " file size "<< remotesize<< ", ignore small file " << surl << std::endl;
    	return true;
    }
  SHA1_Init(&ctx);
  std::string saveas_filename=url_class::get_save_filename(surl);
  if(saveas_filename.length()==0)
    return false;
  saveas_filename = image_save_folder + saveas_filename;
  auto t1 = std::chrono::high_resolution_clock::now();
  // generate unique name
  boost::filesystem::path fullpath(saveas_filename);
  boost::filesystem::path dir = fullpath.parent_path();
  std::string stem = fullpath.stem().string(); // file name without extension
  std::string extension = boost::filesystem::extension(saveas_filename);
  int i=1;
  while(boost::filesystem::exists(saveas_filename))
  //for(int i=1; boost::filesystem::exists(saveas_filename); i++)
  {
	  std::stringstream ss;
	  ss << image_save_folder << stem <<"-"<<i<<extension;
	  saveas_filename = ss.str();
	  i++;
  }

  outputfile_.open(saveas_filename.c_str(), std::ios::out | std::ios::binary);
  
  if(!outputfile_.good())
  {
    std::cout<<current_datetime_str()<<" "<< __METHOD_NAME__<<" failed to create file "<<saveas_filename <<std::endl;
    return false;
  }
  std::cout <<current_datetime_str()<<" "<< __METHOD_NAME__ << " save "<<surl<< " to " << saveas_filename<<std::endl;

    CURL* curlCtx = curl_easy_init();
	deleted_unique_ptr<CURL> scopedcurl(
			curlCtx,
	    [](CURL* curlinst) { curl_easy_cleanup(curlinst); });
    curl_easy_setopt(curlCtx, CURLOPT_URL, surl);
    curl_easy_setopt(curlCtx, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(curlCtx, CURLOPT_WRITEFUNCTION, callbackfunction);
    curl_easy_setopt(curlCtx, CURLOPT_FOLLOWLOCATION, 1);

    CURLcode rc = curl_easy_perform(curlCtx);
    if (rc)
    {
		std::cerr<< __METHOD_NAME__ << " ERROR download "<<surl<<std::endl;
		outputfile_.close();
		remove(saveas_filename.c_str());
		return false;
    }
    long res_code = 0;
    curl_easy_getinfo(curlCtx, CURLINFO_RESPONSE_CODE, &res_code);
    if (!((res_code == 200 || res_code == 201) && rc != CURLE_ABORTED_BY_CALLBACK))
    {
        std::cout<< __METHOD_NAME__ << " ERROR unexpected Response code:" << res_code << " for "<<surl<<std::endl;
		outputfile_.close();
		remove(saveas_filename.c_str());
        return false;
    }
    outputfile_.close();
    long filesize = get_file_size(saveas_filename);
    if (filesize < min_file_size_) {
    	std::cout << __METHOD_NAME__ << " file size "<< filesize<< ", ignore small file " << saveas_filename << std::endl;
    	remove(saveas_filename.c_str());
    	return true;
    }
     SHA1_Final(digest.data(), &ctx);
     auto t2 = std::chrono::high_resolution_clock::now();
     int used_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
     // check sha1 digest see if the file is duplicated.
     if (db_->check_duplicate_image(digest)) {
    	 std::cout << __METHOD_NAME__ << " duplicated sha1 digest " << saveas_filename << std::endl;
		remove(saveas_filename.c_str());
		return true;
     }
     std::string sha1str = digest_class::digest_to_string(digest);
     db_->add_url_resource(surl, saveas_filename, sha1str, filesize, used_ms);
     // check image dimension
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
    		 std::cout << __METHOD_NAME__ << " "<<saveas_filename<< " is removed, image size " << width << "x"<<height<<std::endl;
    	 } else {
    		 std::cout << __METHOD_NAME__ << " "<<saveas_filename<< " image size " << width << "x"<<height<<std::endl;
    	 }
     }
     else
    	 std::cout << __METHOD_NAME__ << " "<<saveas_filename<< " failed to get image size " <<std::endl;
    return true;
}

void http_downloader::image_short_min(long short_min) {
	image_short_min_ = short_min;
}

void http_downloader::image_long_min(long long_min) {
	image_long_min_ = long_min;
}

void http_downloader::set_image_save_location(std::string path) {
	image_save_folder = path;
	if(path.length()>0 && boost::ends_with(path, "/")==false)
		image_save_folder.append("/");
}
