// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#include "md5.h"
#include "common.h"

namespace Ouroboros
{

//-------------------------------------------------------------------------------------
OURO_MD5::OURO_MD5()
{
	MD5_Init(&state_);
	isFinal_ = false;
}

//-------------------------------------------------------------------------------------
OURO_MD5::OURO_MD5(const void * data, int numBytes)
{
	MD5_Init(&state_);
	isFinal_ = false;

	append(data, numBytes);
}

//-------------------------------------------------------------------------------------
OURO_MD5::~OURO_MD5()
{
}

//-------------------------------------------------------------------------------------
void OURO_MD5::append(const void * data, int numBytes)
{
	MD5_Update(&state_, (const unsigned char*)data, numBytes);
}

//-------------------------------------------------------------------------------------
const unsigned char* OURO_MD5::getDigest()
{
	final();
	return bytes_;
}

//-------------------------------------------------------------------------------------
std::string OURO_MD5::getDigestStr()
{
	const unsigned char* md = getDigest();

	char tmp[3]={'\0'}, md5str[33] = {'\0'};
	for (int i = 0; i < 16; ++i)
	{
		sprintf(tmp,"%2.2X", md[i]);
		strcat(md5str, tmp);
	}

	return md5str;
}

//-------------------------------------------------------------------------------------
void OURO_MD5::final()
{
	if(!isFinal_)
	{
		MD5_Final(bytes_, &state_);
		isFinal_ = true;
	}
}

//-------------------------------------------------------------------------------------
void OURO_MD5::clear()
{
	memset(this, 0, sizeof(*this));
}

//-------------------------------------------------------------------------------------
bool OURO_MD5::operator==(const OURO_MD5 & other) const
{
	return memcmp(this->bytes_, other.bytes_, sizeof(bytes_)) == 0;
}

//-------------------------------------------------------------------------------------
bool OURO_MD5::operator<(const OURO_MD5 & other) const
{
	return memcmp(this->bytes_, other.bytes_, sizeof(bytes_)) < 0;
}

//-------------------------------------------------------------------------------------
std::string OURO_MD5::getDigest(const void * data, int numBytes)
{
	OURO_MD5 md5 = OURO_MD5(data, numBytes);
	return md5.getDigestStr();
}

//-------------------------------------------------------------------------------------
} 
