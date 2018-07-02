// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_DB_MYSQL_COMMON_H
#define OURO_DB_MYSQL_COMMON_H

#include "db_context.h"
#include "common/common.h"
#include "common/memorystream.h"
#include "helper/debug_helper.h"

namespace Ouroboros{

// Record all mysql flags set by OURO, check settings when providing sync_item_to_db
extern uint32 ALL_MYSQL_SET_FLAGS;

}
#endif // OURO_DB_MYSQL_COMMON_H
