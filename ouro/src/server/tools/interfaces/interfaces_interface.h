// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com


#if defined(DEFINE_IN_INTERFACE)
	#undef OURO_INTERFACES_TOOL_INTERFACE_H
#endif


#ifndef OURO_INTERFACES_TOOL_INTERFACE_H
#define OURO_INTERFACES_TOOL_INTERFACE_H

// common include	
#if defined(INTERFACES)
#include "interfaces.h"
#endif
#include "interfaces_interface_macros.h"
#include "network/interface_defs.h"
//#define NDEBUG
// windows include	
#if OURO_PLATFORM == PLATFORM_WIN32
#else
// linux include
#endif
	
namespace Ouroboros{

/**
	interfaces message macro, the parameter is stream, you need to unlock it yourself.
*/

/**
	interfaces all message interfaces are defined here
*/
NETWORK_INTERFACE_DECLARE_BEGIN(InterfacesInterface)
	// An app registers its own interface address to this app
	INTERFACES_MESSAGE_DECLARE_ARGS11(onRegisterNewApp,						NETWORK_VARIABLE_MESSAGE,
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
									std::string,							extaddrEx)

	// Request to create an account.
	INTERFACES_MESSAGE_DECLARE_STREAM(reqCreateAccount,						NETWORK_VARIABLE_MESSAGE)

		// Login account.
	INTERFACES_MESSAGE_DECLARE_STREAM(onAccountLogin,						NETWORK_VARIABLE_MESSAGE)

	// Recharge request
	INTERFACES_MESSAGE_DECLARE_STREAM(charge,								NETWORK_VARIABLE_MESSAGE)

	// An app actively requests a look.
	INTERFACES_MESSAGE_DECLARE_ARGS0(lookApp,								NETWORK_FIXED_MESSAGE)

	// An app tells the app that it is active.
	INTERFACES_MESSAGE_DECLARE_ARGS2(onAppActiveTick,						NETWORK_FIXED_MESSAGE,
										COMPONENT_TYPE,						componentType, 
										COMPONENT_ID,						componentID)

	// request to shut down the server
	INTERFACES_MESSAGE_DECLARE_STREAM(reqCloseServer,						NETWORK_VARIABLE_MESSAGE)

	// Request to query watcher data
	INTERFACES_MESSAGE_DECLARE_STREAM(queryWatcher,							NETWORK_VARIABLE_MESSAGE)

	// Request to wipe the client request task.
	INTERFACES_MESSAGE_DECLARE_ARGS1(eraseClientReq,						NETWORK_VARIABLE_MESSAGE,
										std::string,						logkey)

	// start profile
	INTERFACES_MESSAGE_DECLARE_STREAM(startProfile,							NETWORK_VARIABLE_MESSAGE)

	// request to force kill the current app
	INTERFACES_MESSAGE_DECLARE_STREAM(reqKillServer,						NETWORK_VARIABLE_MESSAGE)

	// executeRawDatabaseCommand callback from dbmgr
	INTERFACES_MESSAGE_DECLARE_STREAM(onExecuteRawDatabaseCommandCB,		NETWORK_VARIABLE_MESSAGE)

NETWORK_INTERFACE_DECLARE_END()

#ifdef DEFINE_IN_INTERFACE
	#undef DEFINE_IN_INTERFACE
#endif

}

#endif // OURO_INTERFACES_TOOL_INTERFACE_H
