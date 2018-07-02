// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_SYNC_APP_DATAS_HANDLER_H
#define OURO_SYNC_APP_DATAS_HANDLER_H

// common include
#include "helper/debug_helper.h"
#include "common/common.h"
// #define NDEBUG
// windows include	
#if OURO_PLATFORM == PLATFORM_WIN32
#else
// linux include
#endif

namespace Ouroboros{

class SyncAppDatasHandler : public Task
{
public:
	struct ComponentInitInfo
	{
		COMPONENT_ID cid;
		COMPONENT_ORDER startGroupOrder;
		COMPONENT_ORDER startGlobalOrder;
	};

	SyncAppDatasHandler(Network::NetworkInterface & networkInterface);
	~SyncAppDatasHandler();
	
	bool process();

	void pushApp(COMPONENT_ID cid, COMPONENT_ORDER startGroupOrder, COMPONENT_ORDER startGlobalOrder);

private:
	Network::NetworkInterface &		networkInterface_;
	uint64							lastRegAppTime_;
	std::vector<ComponentInitInfo>	apps_;

};


}

#endif // OURO_SYNC_APP_DATAS_HANDLER_H
