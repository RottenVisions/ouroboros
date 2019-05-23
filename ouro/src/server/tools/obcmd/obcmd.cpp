// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

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

#ifdef _WIN32  
#include <direct.h>  
#include <io.h>  
#elif _LINUX  
#include <stdarg.h>  
#include <sys/stat.h>  
#endif  

#if OURO_PLATFORM == PLATFORM_WIN32
#define OURO_ACCESS _access  
#define OURO_MKDIR(a) _mkdir((a))  
#else
#define OURO_ACCESS access  
#define OURO_MKDIR(a) OURO_UNIX_MKDIR((a))  

int OURO_UNIX_MKDIR(const char* a)
{
	umask(0);
	return mkdir((a), 0755);
}
#endif  

namespace Ouroboros{
	
ServerConfig g_serverConfig;
OURO_SINGLETON_INIT(OBCMD);

//-------------------------------------------------------------------------------------
OBCMD::OBCMD(Network::EventDispatcher& dispatcher,
	Network::NetworkInterface& ninterface,
	COMPONENT_TYPE componentType,
	COMPONENT_ID componentID) :
	PythonApp(dispatcher, ninterface, componentType, componentID),
	mainProcessTimer_()
{
}

//-------------------------------------------------------------------------------------
OBCMD::~OBCMD()
{
	mainProcessTimer_.cancel();
}

//-------------------------------------------------------------------------------------	
void OBCMD::onShutdownBegin()
{
	PythonApp::onShutdownBegin();
}

//-------------------------------------------------------------------------------------	
void OBCMD::onShutdownEnd()
{
	PythonApp::onShutdownEnd();
}

//-------------------------------------------------------------------------------------
bool OBCMD::run()
{
	return PythonApp::run();
}

//-------------------------------------------------------------------------------------
void OBCMD::handleTimeout(TimerHandle handle, void * arg)
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
void OBCMD::handleMainTick()
{
	threadPool_.onMainThreadTick();
}

//-------------------------------------------------------------------------------------
bool OBCMD::initializeBegin()
{
	EntityDef::entityAliasID(ServerConfig::getSingleton().getCellApp().aliasEntityID);
	EntityDef::entitydefAliasID(ServerConfig::getSingleton().getCellApp().entitydefAliasID);
	return true;
}

//-------------------------------------------------------------------------------------
bool OBCMD::inInitialize()
{
	PythonApp::inInitialize();
	// Broadcast your own address to all ouromachines on the web
	Components::getSingleton().pHandler(this);
	return true;
}

//-------------------------------------------------------------------------------------
bool OBCMD::initializeEnd()
{
	PythonApp::initializeEnd();

	mainProcessTimer_ = this->dispatcher().addTimer(1000000 / g_ouroSrvConfig.gameUpdateHertz(), this,
		reinterpret_cast<void *>(TIMEOUT_TICK));


	return true;
}

//-------------------------------------------------------------------------------------		
bool OBCMD::installPyModules()
{
	onInstallPyModules();
	return true;
}

//-------------------------------------------------------------------------------------		
void OBCMD::onInstallPyModules()
{
}

//-------------------------------------------------------------------------------------		
bool OBCMD::initDB()
{
	return true;
}

//-------------------------------------------------------------------------------------
void OBCMD::finalise()
{
	PythonApp::finalise();
}

//-------------------------------------------------------------------------------------
int OBCMD::creatDir(const char *pDir)
{
	int i = 0;
	int iRet = -1;
	int iLen = 0;
	char* pszDir = NULL;

	if (NULL == pDir)
	{
		return 0;
	}

	pszDir = strdup(pDir);
	iLen = strlen(pszDir);

	// Create an intermediate directory
	for (i = 0; i < iLen; i++)
	{
		if (pszDir[i] == '\\' || pszDir[i] == '/')
		{
			if (i == 0)
				continue;

			pszDir[i] = '\0';

			//If it doesn't exist, create it
			iRet = OURO_ACCESS(pszDir, 0);
			if (iRet != 0)
			{
				iRet = OURO_MKDIR(pszDir);
				if (iRet != 0)
				{
					ERROR_MSG(fmt::format("creatDir(): OURO_MKDIR [{}] error! iRet={}\n",
						pszDir, iRet));

					free(pszDir);
					return -1;
				}
			}

			//Support linux, replace all \ with /
			pszDir[i] = '/';
		}
	}

	if (iLen > 0 && OURO_ACCESS(pszDir, 0) != 0)
	{
		iRet = OURO_MKDIR(pszDir);

		if (iRet != 0)
		{
			ERROR_MSG(fmt::format("creatDir(): OURO_MKDIR [{}] error! iRet={}\n",
				pszDir, iRet));
		}
	}

	free(pszDir);
	return iRet;
}

//-------------------------------------------------------------------------------------
}
