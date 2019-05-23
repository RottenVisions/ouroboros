// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#include "profile.h"	
namespace Ouroboros{	

ProfileVal SCRIPTCALL_PROFILE("scriptCall");
ProfileVal SCRIPTCALL_CREATEACCOUNT_PROFILE("onRequestCreateAccount");
ProfileVal SCRIPTCALL_ACCOUNTLOGIN_PROFILE("onRequestAccountLogin");
ProfileVal SCRIPTCALL_CHARGE_PROFILE("onRequestCharge");

EventHistoryStats g_privateClientEventHistoryStats("PrivateClientEvents");
EventHistoryStats g_publicClientEventHistoryStats("PublicClientEvents");

}
