// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com


#if defined(DEFINE_IN_INTERFACE)
	#undef OURO_LOGGER_INTERFACE_H
#endif


#ifndef OURO_LOGGER_INTERFACE_H
#define OURO_LOGGER_INTERFACE_H

// common include	
#if defined(LOGGER)
#include "logger.h"
#endif
#include "logger_interface_macros.h"
#include "network/interface_defs.h"
//#define NDEBUG
// windows include	
#if OURO_PLATFORM == PLATFORM_WIN32
#else
// linux include
#endif
	
namespace Ouroboros{

/**
	Logger all message interfaces are defined here
*/
NETWORK_INTERFACE_DECLARE_BEGIN(LoggerInterface)
	// An app registers its own interface address to this app
	LOGGER_MESSAGE_DECLARE_ARGS11(onRegisterNewApp,							NETWORK_VARIABLE_MESSAGE,
									int32,									uid, 
									std::string,							username,
									COMPONENT_TYPE,							componentType, 
									COMPONENT_ID,							componentID, 
									COMPONENT_ORDER,						globalorderID,
									COMPONENT_ORDER,						grouporderID,
									uint32,									intaddr, 
									uint16,									intport,
									uint32,									extaddr, 
									uint16,									extport,
									std::string,							extAddrEx)

	// An app actively requests a look.
	LOGGER_MESSAGE_DECLARE_ARGS0(lookApp,									NETWORK_FIXED_MESSAGE)

	// An app requests to view the app's load status.
	LOGGER_MESSAGE_DECLARE_ARGS0(queryLoad,									NETWORK_FIXED_MESSAGE)

	// An app tells the app that it is active.
	LOGGER_MESSAGE_DECLARE_ARGS2(onAppActiveTick,							NETWORK_FIXED_MESSAGE,
									COMPONENT_TYPE,							componentType, 
									COMPONENT_ID,							componentID)

	// remotely write logs
	LOGGER_MESSAGE_DECLARE_STREAM(writeLog,									NETWORK_VARIABLE_MESSAGE)

	// Register the log listener
	LOGGER_MESSAGE_DECLARE_STREAM(registerLogWatcher,						NETWORK_VARIABLE_MESSAGE)

	// Logout log listener
	LOGGER_MESSAGE_DECLARE_STREAM(deregisterLogWatcher,						NETWORK_VARIABLE_MESSAGE)

	// log listener to update their own settings
	LOGGER_MESSAGE_DECLARE_STREAM(updateLogWatcherSetting,					NETWORK_VARIABLE_MESSAGE)

	// request to shut down the server
	LOGGER_MESSAGE_DECLARE_STREAM(reqCloseServer,							NETWORK_VARIABLE_MESSAGE)

	// Request to query watcher data
	LOGGER_MESSAGE_DECLARE_STREAM(queryWatcher,								NETWORK_VARIABLE_MESSAGE)

	// start profile
	LOGGER_MESSAGE_DECLARE_STREAM(startProfile,								NETWORK_VARIABLE_MESSAGE)

	// request to force kill the current app
	LOGGER_MESSAGE_DECLARE_STREAM(reqKillServer,							NETWORK_VARIABLE_MESSAGE)

NETWORK_INTERFACE_DECLARE_END()

#ifdef DEFINE_IN_INTERFACE
	#undef DEFINE_IN_INTERFACE
#endif

}
#endif
