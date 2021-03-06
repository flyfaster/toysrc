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


#ifndef HTTP_DOWNLOADER_H
#define HTTP_DOWNLOADER_H
#include "resource_database.h"

class http_downloader
{

public:
    http_downloader();
    http_downloader(const http_downloader& other);
    virtual ~http_downloader();
    virtual http_downloader& operator=(const http_downloader& other);
    virtual bool operator==(const http_downloader& other) const;
    image_digest_t digest;
    bool download_image(const char* surl);
    std::ofstream outputfile_;
    SHA_CTX ctx;
    void reset_digest();
    void min_file_size(long fs);
    void image_short_min(long short_min);
    void image_long_min(long long_min);
    void set_image_save_location(std::string path);
    class resource_database* db_;
    std::string image_save_folder;
private:
    long min_file_size_;
    long image_short_min_;
    long image_long_min_;
};

bool get_image_size(const char *fn, int *x,int *y);

#endif // HTTP_DOWNLOADER_H
