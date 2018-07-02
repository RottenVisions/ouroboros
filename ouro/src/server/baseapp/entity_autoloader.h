// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_ENTITY_AUTOLOADER_H
#define OURO_ENTITY_AUTOLOADER_H

#include "common/common.h"

namespace Ouroboros{

class InitProgressHandler;
class EntityAutoLoader
{
public:
	EntityAutoLoader(Network::NetworkInterface & networkInterface, InitProgressHandler* pInitProgressHandler);
	~EntityAutoLoader();

	bool process();

	void pInitProgressHandler(InitProgressHandler* p)
		{ pInitProgressHandler_ = p; }

	/** Network Interface
		Automatic entity load information retrieved from the database returns
	*/
	void onEntityAutoLoadCBFromDBMgr(Network::Channel* pChannel, MemoryStream& s);

private:
	Network::NetworkInterface & networkInterface_;
	InitProgressHandler* pInitProgressHandler_;

	std::vector< std::vector<ENTITY_SCRIPT_UID> > entityTypes_;

	// Each time the section of the query result set is taken
	ENTITY_ID start_;
	ENTITY_ID end_;

	bool querying_;
};


}

#endif // OURO_ENTITY_AUTOLOADER_H
