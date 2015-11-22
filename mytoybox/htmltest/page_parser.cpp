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
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <boost/asio/ip/tcp.hpp>
#include <openssl/sha.h>
#include <fstream>
#include <sqlite3.h>
#include <deque>
#include <curl/curl.h>
#include <set>
#include "gumbo.h"
#include "url_class.h"
#include "resource_database.h"
#include "page_parser.h"
#include "url_class.h"
#include "http_downloader.h"
#include "clock_timer.h"

page_parser::page_parser():res_db_(0), depth_(0), recursive_(false), max_depth_(1)
{

}

page_parser::page_parser(const page_parser& other)
{

}

page_parser::~page_parser()
{

}

void page_parser::recursive(bool enable) {
	recursive_ = enable;
}

page_parser& page_parser::operator=(const page_parser& other)
{
    return *this;
}

bool page_parser::operator==(const page_parser& other) const
{
///TODO: return ...;
}

std::string page_parser::get_page(const std::string& surl)
{
    std::string page_content;
    url_class u(surl);
    try {
        boost::asio::ip::tcp::iostream s;
        // The entire sequence of I/O operations must complete within 60 seconds.
        // If an expiry occurs, the socket is automatically closed and the stream
        // becomes bad.
//    s.expires_from_now(boost::posix_time::seconds(60));
        // Establish a connection to the server.
        s.connect(u.host_.c_str(), "http"); // www.boost.org
        if (!s)
        {
            std::cout << __METHOD_NAME__ << " Unable to connect: " //<< s.error().message() << "\n";
                      << u.host_ <<std::endl;
            return page_content;
        }
        // Send the request. We specify the "Connection: close" header so that the
        // server will close the socket after transmitting the response. This will
        // allow us to treat all data up until the EOF as the content.
        s << "GET " << u.path_ << " HTTP/1.0\r\n";
        s << "Host: " << u.host_ << "\r\n";
        s << "Accept: */*\r\n";
        s << "Connection: close\r\n\r\n";
        // By default, the stream is tied with itself. This means that the stream
        // automatically flush the buffered output before attempting a read. It is
        // not necessary not explicitly flush the stream at this point.
        // Check that response is OK.
        std::string http_version;
        s >> http_version;
        unsigned int status_code;
        s >> status_code;
        std::string status_message;
        std::getline(s, status_message);
        if (!s || http_version.substr(0, 5) != "HTTP/")
        {
            std::cout << "Invalid response\n";
            return page_content;
        }
        if (status_code != 200)
        {
            std::cout << "Response returned with status code " << status_code << "\n";
            return page_content;
        }
        // Process the response headers, which are terminated by a blank line.
        std::stringstream ss;
        std::string header;
        while (std::getline(s, header) && header != "\r")
            ss << header << "\n";
        ss << "\n";
        // Write the remaining data to output.
        ss << s.rdbuf();
        return ss.str();
    }
    catch (std::exception& e)
    {
        std::cout << "Exception: " << e.what() << "\n";
    }
    return page_content;
}

void page_parser::search_for_links(GumboNode* node) {
	if (node->type != GUMBO_NODE_ELEMENT) {
		return;
	}
	GumboAttribute* href;
	if (recursive_ && depth_ < max_depth_ && node->v.element.tag == GUMBO_TAG_A && (href =
			gumbo_get_attribute(&node->v.element.attributes, "href"))) {
		std::cout << __METHOD_NAME__ << " " << href->value << std::endl;
		// TODO ignore links to external server.
		if (strlen(href->value)) {
			std::string urlfullpath = url_.get_absolute_path(href->value);
			url_class absoluteurl(urlfullpath);
			if (recursive_) {
				if (absoluteurl.host_ == url_.host_ && res_db_) {
					if(is_interested(urlfullpath))
					res_db_->add_page_url(urlfullpath,
							depth_ + 1);
				}
				else
					std::cout << __METHOD_NAME__ << " ignore external link "
							<< href->value << std::endl;
			}
		} else {
			std::cout << __METHOD_NAME__ << " ignore link " << href->value
					<< std::endl;
		}
	}
	if (node->v.element.tag == GUMBO_TAG_IMG
			&& (href = gumbo_get_attribute(&node->v.element.attributes, "src"))) {
//		std::cout << __METHOD_NAME__ //<< " img: " << href->value << std::endl
//				<< " absolute url: " << url_.get_absolute_path(href->value)
//				<< std::endl;
		if (res_db_ && strlen(href->value)>4)
			res_db_->add_image_url(url_.get_absolute_path(href->value));
//     http_downloader downloader;
//     downloader.download_image(href->value);
	}

	if (node->v.element.tag == GUMBO_TAG_IMG
			&& (href = gumbo_get_attribute(&node->v.element.attributes, "file"))) {
		std::cout << __METHOD_NAME__ << " img: " << href->value << std::endl
				<< "absolute url: " << url_.get_absolute_path(href->value)
				<< std::endl;
		if (res_db_ && strlen(href->value))
			res_db_->add_image_url(url_.get_absolute_path(href->value));
//     http_downloader downloader;
//     downloader.download_image(href->value);
	}

	GumboVector* children = &node->v.element.children;
	for (unsigned int i = 0; i < children->length; ++i) {
		search_for_links(static_cast<GumboNode*>(children->data[i]));
	}
}

void page_parser::parse(const std::string& html_src)
{
    GumboOutput* output = gumbo_parse(html_src.c_str());
    page_parser::search_for_links(output->root);
    gumbo_destroy_output(&kGumboDefaultOptions, output);
}

static int writer(char *data, size_t size, size_t nmemb,
                  std::string *writerData)
{
  if (writerData == NULL)
    return 0;
  writerData->append(data, size*nmemb);
  return size * nmemb;
}

template<typename T>
using deleted_unique_ptr = std::unique_ptr<T,std::function<void(T*)>>;
std::string page_parser::get_page3(const std::string& surl) {
	CURL *conn = NULL;
	  CURLcode code;
	  std::string title;
	  static char errorBuffer[CURL_ERROR_SIZE];
	    conn = curl_easy_init();
		deleted_unique_ptr<CURL> scopedcurl(
				conn,
		    [](CURL* curlinst) { curl_easy_cleanup(curlinst); });
	    if (conn == NULL)
	    {
	      std::cout << __METHOD_NAME__<<" Failed to create CURL connection\n";
	      return title;
	    }
	    code = curl_easy_setopt(conn, CURLOPT_ERRORBUFFER, errorBuffer);
	    if (code != CURLE_OK)
	    {
	    	std::cout << __METHOD_NAME__<<" Failed to set error buffer "<< code<<std::endl;
	      return title;
	    }

	    code = curl_easy_setopt(conn, CURLOPT_URL, surl.c_str());
	    if (code != CURLE_OK)
	    {
	    	std::cout << __METHOD_NAME__<<" Failed to set URL "<< errorBuffer<<std::endl;
	      return title;
	    }

	    code = curl_easy_setopt(conn, CURLOPT_FOLLOWLOCATION, 1L);
	    if (code != CURLE_OK)
	    {
	    	std::cout << __METHOD_NAME__<<" Failed to set redirect option "<< errorBuffer<<std::endl;
	      return title;
	    }

	    code = curl_easy_setopt(conn, CURLOPT_WRITEFUNCTION, writer);
	    if (code != CURLE_OK)
	    {
	    	std::cout << __METHOD_NAME__<<" Failed to set writer "<< errorBuffer<<std::endl;
	      return title;
	    }

	    code = curl_easy_setopt(conn, CURLOPT_WRITEDATA, &title);
	    if (code != CURLE_OK)
	    {
	    	std::cout << __METHOD_NAME__<<" Failed to set write data "<< errorBuffer<<std::endl;
	      return title;
	    }
	    curl_global_init(CURL_GLOBAL_DEFAULT);
	    curl_easy_setopt(conn, CURLOPT_TIMEOUT_MS, resource_database::http_timeout_ms);
	    // Retrieve content for the URL
	    code = curl_easy_perform(conn);
//	    curl_easy_cleanup(conn);
	    if (code != CURLE_OK)
	    {
	    	std::cout << __METHOD_NAME__<<" Failed to get"<<surl<<" "<< errorBuffer<<std::endl;
	      return title;
	    }
	    return title;
}

std::string page_parser::get_temp_filename() {
	  std::stringstream ss;
	  ss << clock_timer::get_time_str()
	    << ".html"; // assume it is jpg filename
	  return ss.str();
}

int page_parser::parse_page(const std::string& surl, int depth) {
	set_depth(depth);
	url_.parse(surl);
	std::string html_src = get_page3(surl);
	if (html_src.length() < 8) {
		std::cout << __METHOD_NAME__ << " failed to read " << surl << std::endl;
		return __LINE__;
	}
	std::cout << current_datetime_str() <<__METHOD_NAME__ <<" " << surl << " page source length:" << html_src.length()
			<< std::endl;
	if (save_html_src_) {
		std::string filename = get_temp_filename();
		std::ofstream outputfile_;
		outputfile_.open(filename.c_str(), std::ios::out);
		outputfile_ << html_src;
		outputfile_.close();
	}
	parse(html_src);
	return 0;
}

void page_parser::set_resource_database(resource_database* db) {
	res_db_ = db;
}

void page_parser::set_depth(int d) {
	depth_ = d;
}

size_t callbackfunction2(void *ptr, size_t size, size_t nmemb, void* userdata) {
	if (!userdata) {
		std::cerr << __METHOD_NAME__ << " ERROR No stream to accept "
				<< size * nmemb << " bytes data\n";
		return 0;
	}
	page_parser* downloader = (page_parser*) (userdata);
	downloader->ss_.write((const char*) ptr, size * nmemb);
	return size * nmemb;
}

std::string page_parser::get_page2(const std::string& surl) {
	std::cout << __METHOD_NAME__ << " open " << surl << std::endl;
	page_src_filename = get_temp_filename();
	ss_.open(page_src_filename.c_str(), std::ios_base::binary | std::ios_base::in | std::ios_base::out );
	CURL* curlCtx = curl_easy_init();
	curl_easy_setopt(curlCtx, CURLOPT_URL, surl.c_str());
	curl_easy_setopt(curlCtx, CURLOPT_WRITEDATA, this);
	curl_easy_setopt(curlCtx, CURLOPT_WRITEFUNCTION, callbackfunction2);
	curl_easy_setopt(curlCtx, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(curlCtx, CURLOPT_TIMEOUT_MS, resource_database::http_timeout_ms);
	CURLcode rc = curl_easy_perform(curlCtx);
	if (rc) {
		std::cerr << __METHOD_NAME__ << " ERROR download " << surl << std::endl;
		return "";
	}
	long res_code = 0;
	curl_easy_getinfo(curlCtx, CURLINFO_RESPONSE_CODE, &res_code);
	if (!((res_code == 200 || res_code == 201)
			&& rc != CURLE_ABORTED_BY_CALLBACK)) {
		std::cout << __METHOD_NAME__ << " ERROR unexpected Response code:"
				<< res_code << " for " << surl << std::endl;
		return "";
	}
	curl_easy_cleanup(curlCtx);
	ss_.flush();
	std::cout << __METHOD_NAME__ << " get " << ss_.tellg() << " bytes for "
			<< surl << std::endl;
	ss_.close();
	return "";
}

bool page_parser::is_interested(const std::string& surl) {
	if (pagesite.size()==0)
		return true;
	for(auto sline: pagesite) {
		if(surl.find(sline)!=surl.npos)
			return true;
	}
	return false;
}
