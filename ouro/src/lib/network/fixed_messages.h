// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

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
	Used to make mandatory conventions for intermediate protocols (between front-end and back-end).
	Frontends that do not use the ouro set of protocol auto-binding mechanisms can use this to enforce protocol appointments.
*/
class FixedMessages : public Singleton<FixedMessages>
{
public:

	// Fixed protocol data structure
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
	typedef OUROUnordered_map<std::string, MSGInfo> MSGINFO_MAP;

private:
	MSGINFO_MAP _infomap;
	bool _loaded;
};

}
}
#endif // OURO_FIXED_NETWORK_MESSAGES_H
