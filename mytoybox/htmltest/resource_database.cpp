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
#include "resource_database.h"

resource_database::resource_database():zErrMsg(0), rc(0)
{

}

int resource_database::open(const std::string& database_filepath)
{
  std::cout << __FUNCTION__ << " " << database_filepath<<std::endl;
  db_path_=database_filepath;
  rc=sqlite3_open(database_filepath.c_str(), &db_);
  if(rc)
  {
    db_=0;
    std::cerr<< __FUNCTION__ << " failed to open " << database_filepath<<std::endl;
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
  ss <<"INSERT INTO url_resource_table(resource_url, local_file, sha1data, filesize, used_milliseconds)"
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

static int select_callback(void* NotUsed, int argc, char** argv, char** azColName)
{
  for(int i=0; i<argc; i++)
  {
    std::cout << azColName[i] << "=" << (argv[i]?argv[i]:"NULL") <<std::endl;
  }
  return 0;
}

int resource_database::load_database()
{
  std::cout << __FUNCTION__<<std::endl;
  std::string sql="SELECT * from url_resource_table"; 
  return exec_sql(sql, select_callback);
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
  clear();
  rc = sqlite3_exec(db_, sql.c_str(), callback, 0, &zErrMsg);
  if(rc!=SQLITE_OK)
  {
    std::cerr << __FUNCTION__ << " ERROR " << rc 
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
		std::cout << __FUNCTION__ << " skip duplicated " << imgurl << std::endl;
		return 0;
	}
	if (std::find(img_url_list.begin(), img_url_list.end(), imgurl)
			== img_url_list.end()) {
		std::cout << __FUNCTION__ << " " << imgurl << std::endl;
		img_url_list.push_back(imgurl);
	} else {
		std::cout << __FUNCTION__ << " duplicated " << imgurl << std::endl;
	}
	return 0;
}

int resource_database::add_page_url(const std::string& urlpath, int depth)
{
	if(page_url_depth_table.find(urlpath) != page_url_depth_table.end()) {
		std::cout << __FUNCTION__ << " ignore duplicated url: "<< urlpath<<std::endl;
		return 0;
	}
	page_url_depth_table[urlpath]=depth;
  if (std::find(page_url_list.begin(), page_url_list.end(), urlpath) == page_url_list.end())
  {
	  page_url_list.push_back(urlpath);
	  std::cout << __FUNCTION__ << " add page, depth: "<< depth <<" "<< urlpath <<std::endl;
  }
  else
  {
    std::cout << __FUNCTION__ << " existing page: "<< urlpath<<std::endl;
  }
return 0;
}

std::deque< std::string > resource_database::get_img_list()
{
return img_url_list;
}

std::deque< std::string > resource_database::get_page_list()
{
return page_url_list;
}
