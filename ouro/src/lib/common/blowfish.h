// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com


#ifndef OUROBOROS_BLOWFISH_H
#define OUROBOROS_BLOWFISH_H

#include "openssl/blowfish.h"
#include <string>

namespace Ouroboros { 


class OUROBlowfish
{
public:
	// per block size
	static const int BLOCK_SIZE = 64 / 8;

	// key minimum and maximum size
	static const int MIN_KEY_SIZE = 32 / 8;
	static const int MAX_KEY_SIZE = 448 / 8;

	// the size of the default key
	static const int DEFAULT_KEY_SIZE = 128 / 8;

	typedef std::string Key;

	virtual ~OUROBlowfish();
	OUROBlowfish(const Key & key);
	OUROBlowfish(int keySize = DEFAULT_KEY_SIZE);

	const Key & key() const { return key_; }
	const char * strBlowFishKey() const;
	bool isGood() const { return isGood_; }

	int encrypt(const unsigned char * src, unsigned char * dest, int length);
	int decrypt(const unsigned char * src, unsigned char * dest, int length);

	BF_KEY * pBlowFishKey() { return (BF_KEY*)pBlowFishKey_; }
protected:
	bool init();

	Key key_;
	int keySize_;
	bool isGood_;

	void * pBlowFishKey_;
};

}

#endif // OUROBOROS_BLOWFISH_H
