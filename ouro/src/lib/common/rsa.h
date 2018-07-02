// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OUROBOROS_RSA_H
#define OUROBOROS_RSA_H

#include <string>

namespace Ouroboros
{


/**
 *	openssl rsa
 */
class OURO_RSA
{
public:
	OURO_RSA(const std::string& pubkeyname,
		const std::string& prikeyname);

	OURO_RSA();

	virtual ~OURO_RSA();

	bool generateKey(const std::string& pubkeyname,
		const std::string& prikeyname, int keySize = 1024, int e = 65537);

	std::string encrypt(const std::string& instr);
	int encrypt(const std::string& instr, std::string& outCertifdata);
	int decrypt(const std::string& inCertifdata, std::string& outstr);
	std::string decrypt(const std::string& instr);

	static void hexCertifData(const std::string& inCertifdata);

	bool loadPublic(const std::string& keyname);
	bool loadPrivate(const std::string& keyname);

	virtual bool isGood() const { return rsa_public != NULL && rsa_private != NULL; }

protected:
	void* rsa_public, *rsa_private;
};


}

#endif // OUROBOROS_RSA_H
