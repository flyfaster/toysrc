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
#include "http_downloader.h"
#include "url_class.h"

http_downloader::http_downloader()
{
  reset_digest();
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
      std::cerr<<"ERROR download "<<surl<<std::endl;      
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
     SHA1_Final(digest, &ctx);
    return true;
}