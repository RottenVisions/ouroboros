// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_DB_MYSQL_COMMON_H
#define OURO_DB_MYSQL_COMMON_H

#include "db_context.h"
#include "common/common.h"
#include "common/memorystream.h"
#include "helper/debug_helper.h"

namespace Ouroboros{ 

// Record all mysql tags set by OURO, check the settings when sync_item_to_db is provided
extern uint32 ALL_MYSQL_SET_FLAGS;

}
#endif // OURO_DB_MYSQL_COMMON_H
