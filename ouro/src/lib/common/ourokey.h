// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OUROBOROS_KEY_H
#define OUROBOROS_KEY_H

#include "rsa.h"
#include "common/singleton.h"

namespace Ouroboros
{


/**
 *	Key management of the engine
 */
class OUROKey : public OURO_RSA, public Singleton<OUROKey>
{
public:
	OUROKey(const std::string& pubkeyname,
		const std::string& prikeyname);

	OUROKey();
	virtual ~OUROKey();

	virtual bool isGood() const;
};


}

#endif // OUROBOROS_KEY_H
