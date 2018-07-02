// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_BASEAPP_INIT_PROGRESS_HANDLER_H
#define OURO_BASEAPP_INIT_PROGRESS_HANDLER_H

// common include
#include "helper/debug_helper.h"
#include "common/common.h"

namespace Ouroboros{

class EntityAutoLoader;
class InitProgressHandler : public Task
{
public:
	InitProgressHandler(Network::NetworkInterface & networkInterface);
	~InitProgressHandler();

	bool process();

	void setAutoLoadState(int8 state);

	/** Network Interface
		Automatic entity load information retrieved from the database returns
	*/
	void onEntityAutoLoadCBFromDBMgr(Network::Channel* pChannel, MemoryStream& s);


	void setError();

private:
	Network::NetworkInterface & networkInterface_;
	int delayTicks_;
	EntityAutoLoader* pEntityAutoLoader_;
	int8 autoLoadState_;
	bool error_;
	bool baseappReady_;
};


}

#endif // OURO_BASEAPP_INIT_PROGRESS_HANDLER_H
