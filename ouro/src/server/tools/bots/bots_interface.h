// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#if defined(DEFINE_IN_INTERFACE)
	#undef OURO_BOTS_INTERFACE_H
#endif


#ifndef OURO_BOTS_INTERFACE_H
#define OURO_BOTS_INTERFACE_H

// common include	
#if defined(BOTS)
#include "bots.h"
#endif
#include "bots_interface_macros.h"
#include "network/interface_defs.h"
#include "client_lib/common.h"

//#define NDEBUG
// windows include	
#if OURO_PLATFORM == PLATFORM_WIN32
#else
// linux include
#endif
	
namespace Ouroboros{

/**
	Bots all message interfaces are defined here
*/
NETWORK_INTERFACE_DECLARE_BEGIN(BotsInterface)

	// An app actively requests a look.
	BOTS_MESSAGE_DECLARE_ARGS0(lookApp,									NETWORK_FIXED_MESSAGE)

	// request to shut down the server
	BOTS_MESSAGE_DECLARE_STREAM(reqCloseServer,							NETWORK_VARIABLE_MESSAGE)

	// console remotely executes python statements.
	BOTS_MESSAGE_DECLARE_STREAM(onExecScriptCommand,					NETWORK_VARIABLE_MESSAGE)

	// An app tells the app that it is active.
	BOTS_MESSAGE_DECLARE_ARGS2(onAppActiveTick,							NETWORK_FIXED_MESSAGE,
								COMPONENT_TYPE,							componentType, 
								COMPONENT_ID,							componentID)

	// Add bots.
	BOTS_MESSAGE_DECLARE_STREAM(addBots,								NETWORK_VARIABLE_MESSAGE)

	// Request to query watcher data
	BOTS_MESSAGE_DECLARE_STREAM(queryWatcher,							NETWORK_VARIABLE_MESSAGE)

	// start profile
	BOTS_MESSAGE_DECLARE_STREAM(startProfile,							NETWORK_VARIABLE_MESSAGE)

	// request to force kill the current app
	BOTS_MESSAGE_DECLARE_STREAM(reqKillServer,							NETWORK_VARIABLE_MESSAGE)

NETWORK_INTERFACE_DECLARE_END()

#ifdef DEFINE_IN_INTERFACE
	#undef DEFINE_IN_INTERFACE
#endif

}
#endif
