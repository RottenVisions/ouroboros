// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#include "ourokey.h"
#include "common.h"
#include "helper/debug_helper.h"


namespace Ouroboros
{
OURO_SINGLETON_INIT(OUROKey);

//-------------------------------------------------------------------------------------
OUROKey::OUROKey(const std::string& pubkeyname, const std::string& prikeyname):
OURO_RSA()
{
	if(pubkeyname.size() > 0 || prikeyname.size() > 0)
	{
		OURO_ASSERT(pubkeyname.size() > 0);
		
		if(g_componentType != CLIENT_TYPE)
		{
			OURO_ASSERT(prikeyname.size() > 0);

			bool key1 = loadPrivate(prikeyname);
			bool key2 = loadPublic(pubkeyname);
			OURO_ASSERT(key1 == key2);

			if(!key1 && !key2)
			{
				bool ret = generateKey(pubkeyname, prikeyname);
				OURO_ASSERT(ret);
				key1 = loadPrivate(prikeyname);
				key2 = loadPublic(pubkeyname);
				OURO_ASSERT(key1 && key2);
			}
		}
		else
		{
			bool key = loadPublic(pubkeyname);
			OURO_ASSERT(key);
		}
	}
}

//-------------------------------------------------------------------------------------
OUROKey::OUROKey():
OURO_RSA()
{
}

//-------------------------------------------------------------------------------------
OUROKey::~OUROKey()
{
}

//-------------------------------------------------------------------------------------
bool OUROKey::isGood() const
{
	if(g_componentType == CLIENT_TYPE)
	{
		return rsa_public != NULL;
	}

	return rsa_public != NULL && rsa_private != NULL;
}

//-------------------------------------------------------------------------------------
} 
