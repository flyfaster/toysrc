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


#ifndef RESOURCE_DATABASE_H
#define RESOURCE_DATABASE_H
#include <sqlite3.h>
#include <set>
#include <array>
#include <openssl/sha.h>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <string.h>
//#include <boost/bimap.hpp>

typedef std::array<unsigned char, SHA_DIGEST_LENGTH> image_digest_t;

struct stdarray_compare {
    bool operator() (const image_digest_t& lhs, const image_digest_t& rhs) const{
        return memcmp(lhs.data(), rhs.data(), lhs.size())<0;
    }
};

inline bool operator<(const image_digest_t& lhs, const image_digest_t& rhs)
{
  return memcmp(lhs.data(), rhs.data(), lhs.size())<0;
}


class resource_database
{
public:
    resource_database();
    resource_database(const resource_database& other);
    virtual ~resource_database();
    virtual resource_database& operator=(const resource_database& other);
    virtual bool operator==(const resource_database& other) const;
    sqlite3* db_;
    char* zErrMsg;
    int rc;
    int add_url_resource(const std::string& resource_url, 
			 const std::string& local_file, 
			 const std::string& sha1data,
			 std::size_t filesize,
			 int used_milliseconds
			);
    int create_tables();
    int load_database();
    int open(const std::string& database_filepath);
    void clear();
    int exec_sql(const std::string& sql, int (*callback)(void*,int,char**,char**));
    int exec_sql(const std::string& sql, int (*callback)(void*,int,char**,char**), void* first_arg_to_callback);
    std::string get_dbpath();
    std::string db_path_;
    int add_page_url(const std::string& urlpath, int depth);
    int add_image_url(const std::string& imgurl);

    bool check_duplicate_image(const image_digest_t& imgdigest); // return true if it is duplicated.
    std::deque<std::string> page_url_list, img_url_list;
    std::deque<std::string> get_page_list();
    std::deque<std::string> get_img_list();
    std::unordered_map<std::string, int> page_url_depth_table; // key: page url, value: depth
    static std::unordered_set<std::string> image_url_table;
    static std::set<image_digest_t, stdarray_compare> image_digest_table;
    static int http_timeout_ms;
//    std::set<image_digest_t> image_digest_table;
};

std::string method_name(const std::string& prettyFunction);
std::string current_datetime_str();


#define __METHOD_NAME__ method_name(__PRETTY_FUNCTION__)

#endif // RESOURCE_DATABASE_H
