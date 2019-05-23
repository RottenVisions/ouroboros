// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#include "blowfish.h"
#include "helper/debug_helper.h"
#include "openssl/rand.h"

namespace Ouroboros { 

//-------------------------------------------------------------------------------------
OUROBlowfish::OUROBlowfish(const Key & key):
key_(key),
keySize_(key.size()),
isGood_(false),
pBlowFishKey_(NULL)
{
	if (init())
	{
		//DEBUG_MSG(fmt::format("OUROBlowfish::OUROBlowfish(): Using Blowfish key: {}\n", 
		//	this->strBlowFishKey()));
	}
}

//-------------------------------------------------------------------------------------
OUROBlowfish::OUROBlowfish(int keySize):
	key_(keySize, 0),
	keySize_(keySize),
	isGood_(false),
	pBlowFishKey_(NULL)
{
	RAND_bytes((unsigned char*)const_cast<char *>(key_.c_str()), 
		key_.size());

	if (this->init())
	{
		DEBUG_MSG(fmt::format("OUROBlowfish::OUROBlowfish(): Using Blowfish key: {}\n", 
			this->strBlowFishKey()));
	}
}

//-------------------------------------------------------------------------------------
OUROBlowfish::~OUROBlowfish()
{
	delete pBlowFishKey();
	pBlowFishKey_ = NULL;
}

//-------------------------------------------------------------------------------------
bool OUROBlowfish::init()
{
	pBlowFishKey_ = new BF_KEY;

	if ((MIN_KEY_SIZE <= keySize_) && (keySize_ <= MAX_KEY_SIZE))
	{
		BF_set_key(this->pBlowFishKey(), key_.size(), (unsigned char*)key_.c_str() );
		isGood_ = true;
	}
	else
	{
		ERROR_MSG(fmt::format("OUROBlowfish::init: "
			"invalid length {}\n",
			keySize_));

		isGood_ = false;
	}

	return isGood_;
}

//-------------------------------------------------------------------------------------
const char * OUROBlowfish::strBlowFishKey() const
{
	static char buf[1024];
	char *c = buf;

	for (int i=0; i < keySize_; i++)
	{
		c += sprintf(c, "%02hhX ", (unsigned char)key_[i]);
	}

	c[-1] = '\0';
	return buf;
}

//-------------------------------------------------------------------------------------
int OUROBlowfish::encrypt( const unsigned char * src, unsigned char * dest,
	int length )
{
	// BLOCK_SIZEµÄÕûÊý±¶
	if(length % BLOCK_SIZE != 0)
	{
		CRITICAL_MSG(fmt::format("Blowfish::encrypt: "
			"Input length ({}) is not a multiple of block size ({})\n",
			length, (int)(BLOCK_SIZE)));
	}

	uint64 * pPrevBlock = NULL;
	for (int i=0; i < length; i += BLOCK_SIZE)
	{
		if (pPrevBlock)
		{
			*(uint64*)(dest + i) = *(uint64*)(src + i) ^ (*pPrevBlock);
		}
		else
		{
			*(uint64*)(dest + i) = *(uint64*)(src + i);
		}

		BF_ecb_encrypt(dest + i, dest + i, this->pBlowFishKey(), BF_ENCRYPT);
		pPrevBlock = (uint64*)(src + i);
	}

	return length;
}

//-------------------------------------------------------------------------------------
int OUROBlowfish::decrypt( const unsigned char * src, unsigned char * dest,
	int length )
{
	if (length % BLOCK_SIZE != 0)
	{
		ERROR_MSG(fmt::format("Blowfish::decrypt: "
			"Input stream size ({}) is not a multiple of the block size ({})\n",
			length, (int)(BLOCK_SIZE)));

		return -1;
	}

	uint64 * pPrevBlock = NULL;
	for (int i=0; i < length; i += BLOCK_SIZE)
	{
		BF_ecb_encrypt(src + i, dest + i, this->pBlowFishKey(), BF_DECRYPT);

		if (pPrevBlock)
		{
			*(uint64*)(dest + i) ^= *pPrevBlock;
		}

		pPrevBlock = (uint64*)(dest + i);
	}

	return length;
}

//-------------------------------------------------------------------------------------

}
