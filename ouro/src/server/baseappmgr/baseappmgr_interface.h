// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com


#if defined(DEFINE_IN_INTERFACE)
	#undef OURO_BASEAPPMGR_INTERFACE_H
#endif


#ifndef OURO_BASEAPPMGR_INTERFACE_H
#define OURO_BASEAPPMGR_INTERFACE_H

// common include
#if defined(BASEAPPMGR)
#include "baseappmgr.h"
#endif
#include "baseappmgr_interface_macros.h"
#include "network/interface_defs.h"
#include "server/server_errors.h"
//#define NDEBUG
// windows include
#if OURO_PLATFORM == PLATFORM_WIN32
#else
// linux include
#endif

namespace Ouroboros{

/**
	BASEAPPMGR all message interfaces are defined here
*/
NETWORK_INTERFACE_DECLARE_BEGIN(BaseappmgrInterface)
	// An app registers its own interface address to this app
	BASEAPPMGR_MESSAGE_DECLARE_ARGS11(onRegisterNewApp,									NETWORK_VARIABLE_MESSAGE,
									int32,												uid,
									std::string,										username,
									COMPONENT_TYPE,										componentType,
									COMPONENT_ID,										componentID,
									COMPONENT_ORDER,									globalorderID,
									COMPONENT_ORDER,									grouporderID,
									uint32,												intaddr,
									uint16,												intport,
									uint32,												extaddr,
									uint16,												extport,
									std::string,										extaddrEx)

	// An app actively requests a look.
	BASEAPPMGR_MESSAGE_DECLARE_ARGS0(lookApp,											NETWORK_FIXED_MESSAGE)

	// An app requests to see the app load status.
	BASEAPPMGR_MESSAGE_DECLARE_ARGS0(queryLoad,											NETWORK_FIXED_MESSAGE)

	// An app informs the app that it is active.
	BASEAPPMGR_MESSAGE_DECLARE_ARGS2(onAppActiveTick,									NETWORK_FIXED_MESSAGE,
									COMPONENT_TYPE,										componentType,
									COMPONENT_ID,										componentID)

	// The baseEntity request is created in a new space.
	BASEAPPMGR_MESSAGE_DECLARE_STREAM(reqCreateEntityAnywhere,							NETWORK_VARIABLE_MESSAGE)

	// The baseEntity request is created in a new space.
	BASEAPPMGR_MESSAGE_DECLARE_STREAM(reqCreateEntityRemotely,							NETWORK_VARIABLE_MESSAGE)

	// The baseEntity request is created in a new space, querying the current best component ID
	BASEAPPMGR_MESSAGE_DECLARE_STREAM(reqCreateEntityAnywhereFromDBIDQueryBestBaseappID,NETWORK_VARIABLE_MESSAGE)

	// The baseEntity request is created in a new space.
	BASEAPPMGR_MESSAGE_DECLARE_STREAM(reqCreateEntityAnywhereFromDBID,					NETWORK_VARIABLE_MESSAGE)

	// The baseEntity request is created in a new space.
	BASEAPPMGR_MESSAGE_DECLARE_STREAM(reqCreateEntityRemotelyFromDBID,					NETWORK_VARIABLE_MESSAGE)

	// The message is forwarded by an app that wants to forward the message to an app through the app.
	BASEAPPMGR_MESSAGE_DECLARE_STREAM(forwardMessage,									NETWORK_VARIABLE_MESSAGE)

	// An app informs the app that it is active.
	BASEAPPMGR_MESSAGE_DECLARE_STREAM(registerPendingAccountToBaseapp,					NETWORK_VARIABLE_MESSAGE)

	// Get the address of the baseapp.
	BASEAPPMGR_MESSAGE_DECLARE_ARGS5(onPendingAccountGetBaseappAddr,					NETWORK_VARIABLE_MESSAGE,
									std::string,										loginName,
									std::string,										accountName,
									std::string,										addr,
									uint16,												tcp_port,
									uint16,												udp_port)

	// A newly logged in account has the right to legally login to baseapp. Now it is necessary to register the account to the specified baseapp
	// Make it allow login on this baseapp.
	BASEAPPMGR_MESSAGE_DECLARE_STREAM(registerPendingAccountToBaseappAddr,				NETWORK_VARIABLE_MESSAGE)

	// Request to shut down the server
	BASEAPPMGR_MESSAGE_DECLARE_STREAM(reqCloseServer,									NETWORK_VARIABLE_MESSAGE)

	// Update baseapp information.
	BASEAPPMGR_MESSAGE_DECLARE_ARGS5(updateBaseapp,										NETWORK_FIXED_MESSAGE,
									COMPONENT_ID,										componentID,
									ENTITY_ID,											numBases,
									ENTITY_ID,											numProxices,
									float,												load,
									uint32,												flags)

	// Request for watcher data
	BASEAPPMGR_MESSAGE_DECLARE_STREAM(queryWatcher,										NETWORK_VARIABLE_MESSAGE)

	// Baseapp synchronizes its own initialization information
	BASEAPPMGR_MESSAGE_DECLARE_ARGS2(onBaseappInitProgress,								NETWORK_FIXED_MESSAGE,
									COMPONENT_ID,										cid,
									float,												progress)

	// Start profile
	BASEAPPMGR_MESSAGE_DECLARE_STREAM(startProfile,										NETWORK_VARIABLE_MESSAGE)

	// Request to force kill the current app
	BASEAPPMGR_MESSAGE_DECLARE_STREAM(reqKillServer,									NETWORK_VARIABLE_MESSAGE)

	// Query all relevant process load information
	BASEAPPMGR_MESSAGE_DECLARE_STREAM(queryAppsLoads,									NETWORK_VARIABLE_MESSAGE)

	// Baseapp request binding e-mail (return to need to find loginapp address)
	BASEAPPMGR_MESSAGE_DECLARE_ARGS6(reqAccountBindEmailAllocCallbackLoginapp,			NETWORK_VARIABLE_MESSAGE,
									COMPONENT_ID,										reqBaseappID,
									ENTITY_ID,											entityID,
									std::string,										accountName,
									std::string,										email,
									SERVER_ERROR_CODE,									failedcode,
									std::string,										code)

	// Baseapp request binding e-mail (return to need to find loginapp address)
	BASEAPPMGR_MESSAGE_DECLARE_ARGS8(onReqAccountBindEmailCBFromLoginapp,				NETWORK_VARIABLE_MESSAGE,
									COMPONENT_ID,										reqBaseappID,
									ENTITY_ID,											entityID,
									std::string,										accountName,
									std::string,										email,
									SERVER_ERROR_CODE,									failedcode,
									std::string,										code,
									std::string,										loginappCBHost,
									uint16,												loginappCBPort)

	NETWORK_INTERFACE_DECLARE_END()

#ifdef DEFINE_IN_INTERFACE
	#undef DEFINE_IN_INTERFACE
#endif

}
#endif
