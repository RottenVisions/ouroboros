// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_APP_PROFILE_H
#define OURO_APP_PROFILE_H

#include "common/common.h"
#include "helper/debug_helper.h"
#include "helper/profile.h"
#include "helper/eventhistory_stats.h"

namespace Ouroboros{

extern ProfileVal SCRIPTCALL_PROFILE;
extern ProfileVal SCRIPTCALL_CREATEACCOUNT_PROFILE;
extern ProfileVal SCRIPTCALL_ACCOUNTLOGIN_PROFILE;
extern ProfileVal SCRIPTCALL_CHARGE_PROFILE;

extern EventHistoryStats g_privateClientEventHistoryStats;
extern EventHistoryStats g_publicClientEventHistoryStats;

}
#endif
