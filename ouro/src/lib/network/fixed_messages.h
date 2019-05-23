// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_FIXED_NETWORK_MESSAGES_H
#define OURO_FIXED_NETWORK_MESSAGES_H

#include "common/common.h"
#include "common/singleton.h"
#include "helper/debug_helper.h"
#include "network/common.h"

namespace Ouroboros { 
namespace Network
{
/*
	Used to enforce the contract between the intermediate protocol (between the front end and the back end).
	The front end that does not use the kbe package protocol auto-binding mechanism can use this to force the agreement protocol.
*/
class FixedMessages : public Singleton<FixedMessages>
{
public:

	// fixed protocol data structure
	struct MSGInfo
	{
		MessageID msgid;
		std::string msgname;
		//std::wstring descr;
	};

public:
	FixedMessages();
	~FixedMessages();

	bool loadConfig(std::string fileName, bool notFoundError = true);

	FixedMessages::MSGInfo* isFixed(const char* msgName);
	bool isFixed(MessageID msgid);

public:
	typedef KBEUnordered_map<std::string, MSGInfo> MSGINFO_MAP;

private:
	MSGINFO_MAP _infomap;
	bool _loaded;
};

}
}
#endif // OURO_FIXED_NETWORK_MESSAGES_H
