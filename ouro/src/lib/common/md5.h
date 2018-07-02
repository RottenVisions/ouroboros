// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OUROBOROS_MD5_H
#define OUROBOROS_MD5_H

#include "openssl/md5.h"
#include <string>

namespace Ouroboros
{

/**
 *	openssl md5
 */
class OURO_MD5
{
public:
	OURO_MD5();
	OURO_MD5(const void * data, int numBytes);
	~OURO_MD5();

	void append(const void * data, int numBytes);
	const unsigned char* getDigest();
	std::string getDigestStr();

	void clear();

	void final();

	bool operator==( const OURO_MD5 & other ) const;
	bool operator!=( const OURO_MD5 & other ) const
		{ return !(*this == other); }

	bool operator<( const OURO_MD5 & other ) const;

	static std::string getDigest(const void * data, int numBytes);

	bool isFinal() const{ return isFinal_; }

private:
	MD5_CTX state_;
	unsigned char bytes_[16];
	bool isFinal_;
};


}

#endif // OUROBOROS_MD5_H
