// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_OBCMD_TOOL_H
#define OURO_OBCMD_TOOL_H

#include "server/ouromain.h"
#include "server/python_app.h"
#include "server/serverconfig.h"
#include "common/timer.h"
#include "network/endpoint.h"
#include "resmgr/resmgr.h"
#include "thread/threadpool.h"

namespace Ouroboros{

class OBCMD : public PythonApp,
	public Singleton<OBCMD>
{
public:
	enum TimeOutType
	{
		TIMEOUT_TICK = TIMEOUT_PYTHONAPP_MAX + 1
	};

	OBCMD(Network::EventDispatcher& dispatcher,
		Network::NetworkInterface& ninterface,
		COMPONENT_TYPE componentType,
		COMPONENT_ID componentID);

	~OBCMD();

	bool run();

	void handleTimeout(TimerHandle handle, void * arg);
	void handleMainTick();

		/*Initialize related interfaces*/
	bool initializeBegin();
	bool inInitialize();
	bool initializeEnd();
	void finalise();
	void onInstallPyModules();
	virtual bool installPyModules();

	bool initDB();

	virtual void onShutdownBegin();
	virtual void onShutdownEnd();

	static int creatDir(const char *pDir);

protected:
	TimerHandle																mainProcessTimer_;

};

}

#endif // OURO_OBCMD_TOOL_H

