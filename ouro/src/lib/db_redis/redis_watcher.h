// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_REDIS_WATCHER_H
#define OURO_REDIS_WATCHER_H

#include "db_interface_redis.h"

namespace Ouroboros{ 

class RedisWatcher
{
public:
	static void querystatistics(const char* strCommand, uint32 size);
	static void initializeWatcher();
};

}
#endif // OURO_REDIS_WATCHER_H
