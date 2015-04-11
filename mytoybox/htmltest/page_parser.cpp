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
#include <iomanip>
#include <string>
#include <boost/asio/ip/tcp.hpp>
#include <openssl/sha.h>
#include <fstream>
#include <sqlite3.h>
#include <deque>
#include "gumbo.h"
#include "url_class.h"
#include "resource_database.h"
#include "page_parser.h"
#include "url_class.h"
#include "http_downloader.h"

page_parser::page_parser():res_db_(0), depth_(0)
{

}

page_parser::page_parser(const page_parser& other)
{

}

page_parser::~page_parser()
{

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
            std::cout << "Unable to connect: " //<< s.error().message() << "\n";
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
  if (node->v.element.tag == GUMBO_TAG_A &&
      (href = gumbo_get_attribute(&node->v.element.attributes, "href"))) {
    std::cout <<__FUNCTION__<<" "<< href->value << std::endl;
  // TODO ignore links to external server.
    if (strlen(href->value))
    {
    url_class absoluteurl( url_.get_absolute_path(href->value));
    if (absoluteurl.host_ ==url_.host_ && res_db_)    
      res_db_->add_page_url(url_.get_absolute_path(href->value), depth_+1);
    else
      std::cout <<__FUNCTION__<<" ignore external link "<< href->value << std::endl;
    }
  }
  if (node->v.element.tag == GUMBO_TAG_IMG &&
      (href = gumbo_get_attribute(&node->v.element.attributes, "src"))) {
    std::cout <<__FUNCTION__<<" img: "<< href->value << std::endl;
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

int page_parser::parse_page(const std::string& surl, int depth)
{
  set_depth(depth);
  url_.parse(surl);
  std::string html_src= get_page(surl);
  if (html_src.length()<8)
  {
      std::cout << __FUNCTION__ <<" failed to read " << surl<<std::endl;
      return __LINE__;
  }
  std::cout << surl<< " page source length:"<< html_src.length()<<std::endl;
  parse(html_src);  
  return 0;
}

void page_parser::set_resource_database(resource_database* db)
{
  res_db_ = db;
}

void page_parser::set_depth(int d)
{
depth_ = d;
}
