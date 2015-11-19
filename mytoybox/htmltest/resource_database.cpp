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
#include <string>
#include <sstream>
#include <sqlite3.h>
#include <deque>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <array>
#include <openssl/sha.h>
#include <set>
#include <string.h>
#include "resource_database.h"
#include "digestclass.h"

std::unordered_set<std::string> resource_database::image_url_table;
std::set<image_digest_t, stdarray_compare> resource_database::image_digest_table;
int resource_database::http_timeout_ms=2000; // see CURLOPT_TIMEOUT_MS

resource_database::resource_database():zErrMsg(0), rc(0)
{

}

int resource_database::open(const std::string& database_filepath)
{
  std::cout << __METHOD_NAME__ << " " << database_filepath<<std::endl;
  db_path_=database_filepath;
  rc=sqlite3_open(database_filepath.c_str(), &db_);
  if(rc)
  {
    db_=0;
    std::cerr<< __METHOD_NAME__ << " failed to open " << database_filepath<<std::endl;
  }
}

resource_database::resource_database(const resource_database& other)
{

}

resource_database::~resource_database()
{
  if(db_)
    sqlite3_close(db_);
}

resource_database& resource_database::operator=(const resource_database& other)
{
    return *this;
}

bool resource_database::operator==(const resource_database& other) const
{
///TODO: return ...;
}

static int callback(void* NotUsed, int argc, char** argv, char** azColName)
{
  for(int i=0; i<argc; i++)
  {
    std::cout << azColName[i] << "=" << (argv[i]?argv[i]:"NULL") <<std::endl;
  }
  return 0;
}

int resource_database::add_url_resource(const std::string& resource_url, 
					const std::string& local_file, 
					const std::string& sha1data, 
					std::size_t filesize, 
					int used_milliseconds)
{
  std::stringstream ss;
  ss <<"INSERT INTO images(resource_url, local_file, sha1data, filesize, used_milliseconds)"
  <<"VALUES("
  <<"'"<<resource_url<<"',"
  <<"'"<<local_file<<"',"
  <<"'"<<sha1data<<"',"
  <<filesize<<","
  <<used_milliseconds<<")";
  return exec_sql(ss.str(), callback);
}

int resource_database::create_tables()
{  
  std::stringstream ss;
  ss <<"CREATE TABLE version( schema_version TEXT NOT NULL)";
  rc = exec_sql(ss.str(), callback);
  ss.str(std::string());
  ///////////////////////////////////////
  ss <<"CREATE TABLE pages( page_url TEXT NOT NULL, retry_count INT, depth INT)";
  rc = exec_sql(ss.str(), callback);
  ss.str(std::string());
  //////////////////////////////////////
  ss <<"CREATE TABLE images( resource_url TEXT NOT NULL, local_file TEXT, sha1data TEXT, filesize INT, used_milliseconds INT)";
  rc = exec_sql(ss.str(), callback);
  ss.str(std::string());
  // create other tables. 
  ss <<"INSERT INTO version(schema_version) VALUES(20130818)";
  rc = exec_sql(ss.str(), callback);
  ss.str(std::string());
return 0;
}

static int select_callback(void* first_arg_to_callback, int argc, char** argv, char** azColName)
{
//	 std::cout << __METHOD_NAME__ << " argc="<<argc<<std::endl;
//  for(int i=0; i<argc; i++)
//  {
//    std::cout << azColName[i] << "=" << (argv[i]?argv[i]:"NULL") <<std::endl;
//  }
  const char* table_name = (const char*)first_arg_to_callback;
  if(first_arg_to_callback && strcmp("images", table_name)==0) {
//	  char**)() argc=5
//	  resource_url=http://www.aaa.com/fileuploads/21744/2174404434b29b0a116ac96362aecdc52c805f81.jpg
//	  local_file=/tmp/testdb/2174404434b29b0a116ac96362aecdc52c805f81-1.jpg
//	  sha1data=4249d917530a3f105a8d595a85c0f04391ec93da
//	  filesize=798372
//	  used_milliseconds=483
	  image_digest_t digest;
	  if (digest_class::string_to_digest(argv[2], digest)) {
		  resource_database::image_digest_table.insert(digest);
	  }
	  resource_database::image_url_table.insert(argv[0]);
  }
  return 0;
}

int resource_database::load_database()
{
  std::cout << __METHOD_NAME__<<std::endl;
  std::string sql="SELECT * from images";
  int countbefore = image_url_table.size();
  int ret = exec_sql(sql, select_callback, (void*)"images");
  std::cout << __METHOD_NAME__ << " loaded " << image_url_table.size()-countbefore << " records from images table" << std::endl;
  return ret;
}

void resource_database::clear()
{
  if(zErrMsg)
    sqlite3_free(zErrMsg);
  zErrMsg = 0;
  rc = SQLITE_OK;
}

int resource_database::exec_sql(const std::string& sql, int (*callback)(void*,int,char**,char**) )
{
	rc = exec_sql(sql, callback, NULL);
	return rc;
}
int resource_database::exec_sql(const std::string& sql, int (*callback)(void*,int,char**,char**), void* first_arg_to_callback) {
	  clear();
	  rc = sqlite3_exec(db_, sql.c_str(), callback, first_arg_to_callback, &zErrMsg);
	  if(rc!=SQLITE_OK)
	  {
	    std::cerr << __METHOD_NAME__ << " ERROR " << rc
	    << " execute sql: " << sql<<std::endl;
	  }
	  return rc;
}

std::string resource_database::get_dbpath()
{
return db_path_;
}

int resource_database::add_image_url(const std::string& imgurl) {
	if (image_url_table.find(imgurl) != image_url_table.end()) {
		std::cout << __METHOD_NAME__ << " skip duplicated " << imgurl << std::endl;
		return 0;
	}
	if (std::find(img_url_list.begin(), img_url_list.end(), imgurl)
			== img_url_list.end()) {
		std::cout << __METHOD_NAME__ << " " << imgurl << std::endl;
		img_url_list.push_back(imgurl);
	} else {
		std::cout << __METHOD_NAME__ << " duplicated " << imgurl << std::endl;
	}
	return 0;
}

int resource_database::add_page_url(const std::string& urlpath, int depth)
{
	if(page_url_depth_table.find(urlpath) != page_url_depth_table.end()) {
		std::cout << __METHOD_NAME__ << " ignore duplicated url: "<< urlpath<<std::endl;
		return 0;
	}
	page_url_depth_table[urlpath]=depth;
  if (std::find(page_url_list.begin(), page_url_list.end(), urlpath) == page_url_list.end())
  {
	  page_url_list.push_back(urlpath);
	  std::cout << __METHOD_NAME__ << " add page, depth: "<< depth <<" "<< urlpath <<std::endl;
  }
  else
  {
    std::cout << __METHOD_NAME__ << " existing page: "<< urlpath<<std::endl;
  }
return 0;
}

bool resource_database::check_duplicate_image(const image_digest_t& imgdigest) {
	if(image_digest_table.find(imgdigest)!=image_digest_table.end())
		return true;
	image_digest_table.insert(imgdigest);
	return false;
}

std::deque<std::string> resource_database::get_img_list() {
	std::deque<std::string> ret;
	ret.swap(img_url_list);
	return ret;
}

std::deque< std::string > resource_database::get_page_list()
{
	std::deque<std::string> ret;
	ret.swap(page_url_list);
	return ret;
}

std::string method_name(const std::string& prettyFunction)
{
    size_t colons = prettyFunction.find("::");
    size_t begin = prettyFunction.substr(0,colons).rfind(" ") + 1;
    size_t end = prettyFunction.rfind("(") - begin;
    return prettyFunction.substr(begin,end) + "()";
}

std::string current_datetime_str()
{
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    return buf;
}
