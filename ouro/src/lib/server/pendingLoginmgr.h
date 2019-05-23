// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_PENDING_LOGIN_MGR_H
#define OURO_PENDING_LOGIN_MGR_H

#include "common/common.h"
#include "common/tasks.h"
#include "common/timer.h"
#include "common/singleton.h"
#include "helper/debug_helper.h"
#include "server/components.h"
#include "network/address.h"

namespace Ouroboros { 

namespace Network
{
class NetworkInterface;
class EventDispatcher;
}

/*
	Log in to the server to detect success, but when you have not entered the game world, you need to cache the account for subsequent processing.
*/
class PendingLoginMgr : public Task
{
public:
	struct PLInfos
	{
		PLInfos()
		{
			ctype = UNKNOWN_CLIENT_COMPONENT_TYPE;
			flags = 0;
			deadline = 0;
			entityDBID = 0;
			entityID = 0;
			lastProcessTime = 0;
			forceInternalLogin = false;
			needCheckPassword = true;
		}

		Network::Address addr;
		COMPONENT_CLIENT_TYPE ctype;
		std::string accountName;
		std::string password;
		std::string datas;
		TimeStamp lastProcessTime;
		ENTITY_ID entityID;
		DBID entityDBID;
		uint32 flags;
		uint64 deadline;
		bool forceInternalLogin;
		bool needCheckPassword;
	};

	typedef KBEUnordered_map<std::string, PLInfos*> PTINFO_MAP;

public:
	PendingLoginMgr(Network::NetworkInterface & networkInterface);
	~PendingLoginMgr();

	Network::EventDispatcher & dispatcher();

	bool add(PLInfos* infos);
	
	bool process();
	
	PendingLoginMgr::PLInfos* remove(std::string& accountName);
	PendingLoginMgr::PLInfos* find(std::string& accountName);

	void removeNextTick(std::string& accountName);

private:
	Network::NetworkInterface & networkInterface_;

	bool start_;
	
	PTINFO_MAP pPLMap_;

};

}

#endif // OURO_PENDING_LOGIN_MGR_H
