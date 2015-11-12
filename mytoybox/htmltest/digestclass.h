/*
 * digestclass.h
 *
 *  Created on: Nov 11, 2015
 *      Author: onega
 */

#ifndef DIGESTCLASS_H_
#define DIGESTCLASS_H_
#include <array>
#include <openssl/sha.h>

typedef std::array<unsigned char, SHA_DIGEST_LENGTH> image_digest_t;

class digest_class {
public:
	digest_class();
	virtual ~digest_class();
	static int sha1checksum(const std::string& filename, image_digest_t& digest);
	int remove_duplicated_file(const std::string& path);
	class resource_database* db_;
};

#endif /* DIGESTCLASS_H_ */
