// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#include "profile.h"	
namespace Ouroboros{	

ProfileVal SCRIPTCALL_PROFILE("scriptCall");
ProfileVal ONTIMER_PROFILE("onTimer");

EventHistoryStats g_privateClientEventHistoryStats("PrivateClientEvents");
EventHistoryStats g_publicClientEventHistoryStats("PublicClientEvents");

}
