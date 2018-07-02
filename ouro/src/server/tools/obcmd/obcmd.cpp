// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#include "profile.h"
#include "obcmd.h"
#include "obcmd_interface.h"
#include "client_sdk_unity.h"
#include "client_sdk_ue4.h"
#include "network/common.h"
#include "network/tcp_packet.h"
#include "network/udp_packet.h"
#include "network/message_handler.h"
#include "thread/threadpool.h"
#include "server/components.h"
#include "server/telnet_server.h"
#include "entitydef/entitydef.h"

#include "baseapp/baseapp_interface.h"
#include "cellapp/cellapp_interface.h"
#include "baseappmgr/baseappmgr_interface.h"
#include "cellappmgr/cellappmgr_interface.h"
#include "loginapp/loginapp_interface.h"
#include "dbmgr/dbmgr_interface.h"

namespace Ouroboros{

ServerConfig g_serverConfig;
OURO_SINGLETON_INIT(KBCMD);

//-------------------------------------------------------------------------------------
KBCMD::KBCMD(Network::EventDispatcher& dispatcher,
	Network::NetworkInterface& ninterface,
	COMPONENT_TYPE componentType,
	COMPONENT_ID componentID) :
	PythonApp(dispatcher, ninterface, componentType, componentID),
	mainProcessTimer_()
{
}

//-------------------------------------------------------------------------------------
KBCMD::~KBCMD()
{
	mainProcessTimer_.cancel();
}

//-------------------------------------------------------------------------------------
void KBCMD::onShutdownBegin()
{
	PythonApp::onShutdownBegin();
}

//-------------------------------------------------------------------------------------
void KBCMD::onShutdownEnd()
{
	PythonApp::onShutdownEnd();
}

//-------------------------------------------------------------------------------------
bool KBCMD::run()
{
	return PythonApp::run();
}

//-------------------------------------------------------------------------------------
void KBCMD::handleTimeout(TimerHandle handle, void * arg)
{
	PythonApp::handleTimeout(handle, arg);

	switch (reinterpret_cast<uintptr>(arg))
	{
	case TIMEOUT_TICK:
		this->handleMainTick();
		break;
	default:
		break;
	}
}

//-------------------------------------------------------------------------------------
void KBCMD::handleMainTick()
{
	//time_t t = ::time(NULL);
	//DEBUG_MSG("KBCMD::handleGameTick[%"PRTime"]:%u\n", t, time_);

	threadPool_.onMainThreadTick();
}

//-------------------------------------------------------------------------------------
bool KBCMD::initializeBegin()
{
	EntityDef::entityAliasID(ServerConfig::getSingleton().getCellApp().aliasEntityID);
	EntityDef::entitydefAliasID(ServerConfig::getSingleton().getCellApp().entitydefAliasID);
	return true;
}

//-------------------------------------------------------------------------------------
bool KBCMD::inInitialize()
{
	PythonApp::inInitialize();
	// Broadcast your own address to all ouromachines on the web
	Components::getSingleton().pHandler(this);
	return true;
}

//-------------------------------------------------------------------------------------
bool KBCMD::initializeEnd()
{
	PythonApp::initializeEnd();

	mainProcessTimer_ = this->dispatcher().addTimer(1000000 / g_ouroSrvConfig.gameUpdateHertz(), this,
		reinterpret_cast<void *>(TIMEOUT_TICK));


	return true;
}

//-------------------------------------------------------------------------------------
bool KBCMD::installPyModules()
{
	onInstallPyModules();
	return true;
}

//-------------------------------------------------------------------------------------
void KBCMD::onInstallPyModules()
{
}

//-------------------------------------------------------------------------------------
bool KBCMD::initDB()
{
	return true;
}

//-------------------------------------------------------------------------------------
void KBCMD::finalise()
{
	PythonApp::finalise();
}

//-------------------------------------------------------------------------------------
}
