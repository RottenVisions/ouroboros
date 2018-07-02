// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com
#ifndef OURO_GLOBAL_DATA_SERVER_H
#define OURO_GLOBAL_DATA_SERVER_H

#include "common/common.h"
#include "helper/debug_helper.h"

namespace Ouroboros{
namespace Network
{
	class Channel;
}

class GlobalDataServer
{
public:
	enum DATA_TYPE
	{
		GLOBAL_DATA,
		BASEAPP_DATA,
		CELLAPP_DATA
	};

public:
	GlobalDataServer(DATA_TYPE dataType);
	~GlobalDataServer();

	/** Write data */
	bool write(Network::Channel* pChannel, COMPONENT_TYPE componentType, const std::string& key, const std::string& value);

	/** delete data */
	bool del(Network::Channel* pChannel, COMPONENT_TYPE componentType, const std::string& key);

	/** Add the components that the server needs to care about */
	void addConcernComponentType(COMPONENT_TYPE ct){ concernComponentTypes_.push_back(ct); }

	/** Broadcast a data change to the component of interest */
	void broadcastDataChanged(Network::Channel* pChannel, COMPONENT_TYPE componentType, const std::string& key,
							const std::string& value, bool isDelete = false);

	/** A new client login */
	void onGlobalDataClientLogon(Network::Channel* client, COMPONENT_TYPE componentType);

private:
	DATA_TYPE dataType_;

	std::vector<COMPONENT_TYPE> concernComponentTypes_;						// The type of component that GlobalDataServer needs to care about
	typedef std::map<std::string, std::string> DATA_MAP;
	typedef DATA_MAP::iterator DATA_MAP_KEY;
	DATA_MAP dict_;
} ;

}

#endif // OURO_GLOBAL_DATA_SERVER_H
