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
#include <algorithm>
#include <string>
#include <boost/algorithm/string.hpp>
#include <iomanip>
#include <iostream>
#include <sstream>
#include "url_class.h"
#include "clock_timer.h"
url_class::url_class()
{

}

url_class::url_class(const std::string& url_s)
{
  resource_url_ = url_s;
  parse(resource_url_);
}

url_class::url_class(const url_class& other)
{
  resource_url_ = other.resource_url_;
  host_ = other.host_;
  path_ = other.path_;
  protocol_ = other.protocol_;
  query_ = other.query_;
}

url_class::~url_class()
{

}

url_class& url_class::operator=(const url_class& other)
{
  if ( this == &other)
    return *this;
  resource_url_ = other.resource_url_;
  host_ = other.host_;
  path_ = other.path_;
  protocol_ = other.protocol_;
  query_ = other.query_;
    return *this;
}

bool url_class::operator==(const url_class& other) const
{
///TODO: return ...;
}

void url_class::parse(const std::string& url_s)
{using namespace std;
resource_url_ = url_s;
    const std::string prot_end("://");
    std::string::const_iterator prot_i = search(url_s.begin(), url_s.end(),
                                           prot_end.begin(), prot_end.end());
    protocol_.reserve(distance(url_s.begin(), prot_i));
    transform(url_s.begin(), prot_i,
              back_inserter(protocol_),
              ptr_fun<int,int>(tolower)); // protocol is icase
    if( prot_i == url_s.end() )
        return;
    advance(prot_i, prot_end.length());
    std::string::const_iterator path_i = find(prot_i, url_s.end(), '/');
    host_.reserve(distance(prot_i, path_i));
    transform(prot_i, path_i,
              back_inserter(host_),
              ptr_fun<int,int>(tolower)); // host is icase
    std::string::const_iterator query_i = find(path_i, url_s.end(), '?');
    path_.assign(path_i, query_i);
    if( query_i != url_s.end() )
        ++query_i;
    query_.assign(query_i, url_s.end());
}

std::string url_class::get_save_filename(const std::string& urlpath)
{
  // if urlpath ends with .jpg or .png, .gif
  // urlpath ends with other
  std::string filename;
  if (urlpath.find("://")==std::string::npos)
  {
    std::cout<<__FUNCTION__<<" invalid url: "<<urlpath<<std::endl;
    return filename;
  }
  size_t path_separator_pos=urlpath.find_last_of("/");
  if (path_separator_pos == std::string::npos)
    return filename;
  filename = urlpath.substr(path_separator_pos+1);
  
  if (boost::algorithm::ends_with(urlpath, ".jpg"))
  {
    return filename;
  }
  if (boost::algorithm::ends_with(urlpath, ".JPG"))
  {
    return filename;
  }
  if (boost::algorithm::ends_with(urlpath, ".png"))
  {
    return filename;
  }
  if (boost::algorithm::ends_with(urlpath, ".PNG"))
  {
    return filename;
  }
  if (boost::algorithm::ends_with(urlpath, ".gif"))
  {
    return filename;
  }
  if (boost::algorithm::ends_with(urlpath, ".GIF"))
  {
    return filename;
  }
  std::stringstream ss;
  ss << clock_timer::get_time_str()
    << ".jpg"; // assume it is jpg filename
    filename = ss.str();
    return filename;
}
std::string url_class::get_url()
{
return resource_url_;
}

std::string url_class::get_absolute_path(const std::string& relative_path)
{
	// http://www.66.ca/data/attachment/forum/201506/27/141841p36hn6hiqznq6iiq.jpg
  std::string absolute_path;
  if (relative_path.find("://")!=std::string::npos)
    return relative_path; // this is absolute path 
  if(relative_path.length()==0)
    return absolute_path;
  // path starts with '/'
  if(relative_path.at(0)=='/')
  {
    absolute_path=protocol_ + "://"+host_ + relative_path;    
  }
  else 
  {
    size_t path_separator_pos=resource_url_.find_last_of("/");
    if(path_separator_pos != resource_url_.npos)
    {
      absolute_path= resource_url_.substr(0,path_separator_pos+1)+relative_path;
    }
  }
  // path not start with '/'
  return absolute_path;
}
