// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com


#ifndef OURO_SERVER_APP_H
#define OURO_SERVER_APP_H

#include "common/common.h"
#if OURO_PLATFORM == PLATFORM_WIN32
#pragma warning (disable : 4996)
#endif

#include <stdarg.h>
#include "helper/debug_helper.h"
#include "helper/watcher.h"
#include "helper/profile.h"
#include "helper/profiler.h"
#include "helper/profile_handler.h"
#include "xml/xml.h"
#include "server/common.h"
#include "server/components.h"
#include "server/serverconfig.h"
#include "server/signal_handler.h"
#include "server/shutdown_handler.h"
#include "common/smartpointer.h"
#include "common/timer.h"
#include "common/singleton.h"
#include "network/interfaces.h"
#include "network/event_dispatcher.h"
#include "network/network_interface.h"
#include "thread/threadpool.h"


namespace Ouroboros{

namespace Network
{

class Channel;
}

class Shutdowner;
class ComponentActiveReportHandler;

class ServerApp :
	public SignalHandler,
	public TimerHandler,
	public ShutdownHandler,
	public Network::ChannelTimeOutHandler,
	public Network::ChannelDeregisterHandler,
	public Components::ComponentsNotificationHandler
{
public:
	enum TimeOutType
	{
		TIMEOUT_SERVERAPP_MAX
	};

public:
	ServerApp(Network::EventDispatcher& dispatcher,
			Network::NetworkInterface& ninterface,
			COMPONENT_TYPE componentType,
			COMPONENT_ID componentID);

	~ServerApp();

	virtual bool initialize();
	virtual bool initializeBegin() { return true; }
	virtual bool inInitialize(){ return true; }
	virtual bool initializeEnd() {return true; }
	virtual void finalise();
	virtual bool run();

	virtual bool initThreadPool();

	virtual bool installSignals();

	virtual bool initializeWatcher();

	virtual bool loadConfig();
	const char* name(){return COMPONENT_NAME_EX(componentType_);}

	virtual void handleTimeout(TimerHandle, void * pUser);

	GAME_TIME time() const { return g_ourotime; }
	Timers & timers() { return timers_; }
	double gameTimeInSeconds() const;
	void handleTimers();

	thread::ThreadPool& threadPool() { return threadPool_; }

	Network::EventDispatcher & dispatcher()				{ return dispatcher_; }
	Network::NetworkInterface & networkInterface()			{ return networkInterface_; }

	COMPONENT_ID componentID() const	{ return componentID_; }
	COMPONENT_TYPE componentType() const	{ return componentType_; }

	virtual void onSignalled(int sigNum);
	virtual void onChannelTimeOut(Network::Channel * pChannel);
	virtual void onChannelDeregister(Network::Channel * pChannel);
	virtual void onAddComponent(const Components::ComponentInfos* pInfos);
	virtual void onRemoveComponent(const Components::ComponentInfos* pInfos);
	virtual void onIdentityillegal(COMPONENT_TYPE componentType, COMPONENT_ID componentID, uint32 pid, const char* pAddr);

	virtual void onShutdownBegin();
	virtual void onShutdown(bool first);
	virtual void onShutdownEnd();

	/** Network Interface
		Request to see watcher
	*/
	void queryWatcher(Network::Channel* pChannel, MemoryStream& s);

	void shutDown(float shutdowntime = -FLT_MAX);

	COMPONENT_ORDER globalOrder() const { return startGlobalOrder_; }
	COMPONENT_ORDER groupOrder() const { return startGroupOrder_; }

	/** Network Interface
		Register a newly activated baseapp or cellapp or dbmgr
		Usually a new app is started and it needs to register itself with certain components.
	*/
	virtual void onRegisterNewApp(Network::Channel* pChannel,
							int32 uid,
							std::string& username,
							COMPONENT_TYPE componentType, COMPONENT_ID componentID, COMPONENT_ORDER globalorderID, COMPONENT_ORDER grouporderID,
							uint32 intaddr, uint16 intport, uint32 extaddr, uint16 extport, std::string& extaddrEx);

	/** Network Interface
		An app informs the app that it is active.
	*/
	void onAppActiveTick(Network::Channel* pChannel, COMPONENT_TYPE componentType, COMPONENT_ID componentID);

	/** Network Interface
		Request to disconnect the server
	*/
	virtual void reqClose(Network::Channel* pChannel);

	/** Network Interface
		An app requests to view the app
	*/
	virtual void lookApp(Network::Channel* pChannel);

	/** Network Interface
		Request to shut down the server
	*/
	virtual void reqCloseServer(Network::Channel* pChannel, MemoryStream& s);

	/** Network Interface
		An app request to view the app load status, usually the console request view
	*/
	virtual void queryLoad(Network::Channel* pChannel);

	/** Network Interface
		Request to shut down the server
	*/
	void reqKillServer(Network::Channel* pChannel, MemoryStream& s);

	/** Network Interface
		The first time the client and server establish an interaction, The client sends its own version number, communication key, and other information
		To the server, the server returns whether the handshake succeeds
	*/
	virtual void hello(Network::Channel* pChannel, MemoryStream& s);
	virtual void onHello(Network::Channel* pChannel,
		const std::string& verInfo,
		const std::string& scriptVerInfo,
		const std::string& encryptedKey);

	// Engine version does not match
	virtual void onVersionNotMatch(Network::Channel* pChannel);

	// Engine script layer version does not match
	virtual void onScriptVersionNotMatch(Network::Channel* pChannel);

	/** Network Interface
		Console request start profile
	*/
	void startProfile(Network::Channel* pChannel, Ouroboros::MemoryStream& s);
	virtual void startProfile_(Network::Channel* pChannel, std::string profileName, int8 profileType, uint32 timelen);

protected:
	COMPONENT_TYPE											componentType_;
	COMPONENT_ID											componentID_;									// The ID of this component

	Network::EventDispatcher& 								dispatcher_;
	Network::NetworkInterface&								networkInterface_;

	Timers													timers_;

	// App startup sequence, global is the global (such as dbmgr, cellapp order) startup sequence,
	// Group is the group startup order (for example, all baseapps are a group)
	COMPONENT_ORDER											startGlobalOrder_;
	COMPONENT_ORDER											startGroupOrder_;

	Shutdowner*												pShutdowner_;
	ComponentActiveReportHandler*							pActiveTimerHandle_;

	// Thread Pool
	thread::ThreadPool										threadPool_;
};

}

#endif // OURO_SERVER_APP_H
