// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_WATCH_POOLS_HANDLER_CELL_H
#define OURO_WATCH_POOLS_HANDLER_CELL_H

#include "common/common.h"
#include "helper/debug_helper.h"

namespace Ouroboros { 

class WatchObjectPool
{
public:
	static bool initWatchPools();
	static bool finiWatchPools();
};

}

#endif // OURO_WATCH_POOLS_HANDLER_CELL_H
