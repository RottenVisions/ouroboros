// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_WATCH_POOLS_HANDLER_H
#define OURO_WATCH_POOLS_HANDLER_H

#include "common/common.h"
#include "helper/debug_helper.h"

namespace Ouroboros { 

class WatchPool
{
public:
	static bool initWatchPools();
	static bool finiWatchPools();
};

}

#endif
