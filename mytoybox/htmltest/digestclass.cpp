/*
 * digestclass.cpp
 *
 *  Created on: Nov 11, 2015
 *      Author: onega
 */
#include "resource_database.h"
#include "digestclass.h"
#include "boost/filesystem.hpp"
#include <string.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

digest_class::digest_class() {
	// TODO Auto-generated constructor stub

}

digest_class::~digest_class() {
	// TODO Auto-generated destructor stub
}

int digest_class::sha1checksum(const std::string& filename, image_digest_t& digest) {
	memset(digest.data(), 0, digest.size());
	std::array< char, 1024*64> buf;
    SHA_CTX ctx;
    SHA1_Init(&ctx);
    std::ifstream ifs (filename.c_str(),std::ios_base::binary);
    if (!ifs.good()) {
    	std::cout << __METHOD_NAME__ << " errno=" << errno << filename << std::endl;
    	return errno;
    }
    while(!ifs.eof()) {
    	size_t extracted = ifs.read(buf.data(), buf.size()).gcount();
    	if(extracted>0) {
    		SHA1_Update(&ctx, buf.data(), extracted);
    	}
    }
    SHA1_Final(digest.data(), &ctx);
    return 0;
}

int digest_class::remove_duplicated_file(const std::string& pathname) {
	boost::filesystem::path p (pathname);
	boost::filesystem::recursive_directory_iterator end_itr;
	image_digest_t digest;
    // cycle through the directory
    for (boost::filesystem::recursive_directory_iterator itr(p); itr != end_itr; ++itr)
    {
        // If it's not a directory, list it. If you want to list directories too, just remove this check.
        if (boost::filesystem::is_regular_file(itr->path())) {
            // assign current file name to current_file and echo it out to the console.
            std::string current_file = itr->path().string();
            int ret = sha1checksum(current_file, digest);
            if (db_->check_duplicate_image(digest)) {
            	std::cout << __METHOD_NAME__ << " found duplicate " << current_file << std::endl;
            } else {
            	if(ret)
            		std::cout << __METHOD_NAME__ << " failed " << current_file << std::endl;
            	else
            		std::cout << __METHOD_NAME__ << " finished " << current_file << std::endl;
            }
        }
    }
    std::cout << __METHOD_NAME__ << " size " << db_->image_digest_table.size() << std::endl;
    return 0;
}

std::string digest_class::digest_to_string(const image_digest_t& digest) {
    std::string sha1str;
    std::stringstream ss;
    for(auto data: digest)
   	 ss << std::hex <<std::setw(2)<<std::setfill('0') << (int)data;
    sha1str = ss.str();
    return sha1str;
}

bool digest_class::string_to_digest(const std::string& input,
		image_digest_t& output) {
	memset(output.data(), 0, output.size());
    static const char* const lut = "0123456789abcdef"; // "0123456789ABCDEF";
    size_t len = input.length();
    if (len!=output.size()*2) {
    	std::cout << __METHOD_NAME__ << " unexpected digest string length " << len << std::endl;
    	return false;
    }
    if (len & 1) throw std::invalid_argument("odd length");

    for (size_t i = 0; i < len; i += 2)
    {
        char a = input[i];
        const char* p = std::lower_bound(lut, lut + 16, a);
        if (*p != a) throw std::invalid_argument("not a hex digit");

        char b = input[i + 1];
        const char* q = std::lower_bound(lut, lut + 16, b);
        if (*q != b) throw std::invalid_argument("not a hex digit");
        output[i/2] = (((p - lut) << 4) | (q - lut));
    }
    return true;
}
