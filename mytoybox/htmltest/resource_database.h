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
    std::string get_dbpath();
    std::string db_path_;
    int add_page_url(const std::string& urlpath, int depth);
    int add_image_url(const std::string& imgurl);
    std::deque<std::string> page_url_list, img_url_list;
    std::deque<std::string> get_page_list();
    std::deque<std::string> get_img_list();
};

#endif // RESOURCE_DATABASE_H
