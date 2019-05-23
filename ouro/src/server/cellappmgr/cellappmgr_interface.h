// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#if defined(DEFINE_IN_INTERFACE)
	#undef OURO_CELLAPPMGR_INTERFACE_H
#endif


#ifndef OURO_CELLAPPMGR_INTERFACE_H
#define OURO_CELLAPPMGR_INTERFACE_H

// common include	
#if defined(CELLAPPMGR)
#include "cellappmgr.h"
#endif
#include "cellappmgr_interface_macros.h"
#include "network/interface_defs.h"
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
NETWORK_INTERFACE_DECLARE_BEGIN(CellappmgrInterface)
	// An app registers its own interface address to this app
	CELLAPPMGR_MESSAGE_DECLARE_ARGS11(onRegisterNewApp,						NETWORK_VARIABLE_MESSAGE,
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

	// An app actively requests a look.
	CELLAPPMGR_MESSAGE_DECLARE_ARGS0(lookApp,								NETWORK_FIXED_MESSAGE)

	// An app requests to view the app's load status.
	CELLAPPMGR_MESSAGE_DECLARE_ARGS0(queryLoad,								NETWORK_FIXED_MESSAGE)

	// An app tells the app that it is active.
	CELLAPPMGR_MESSAGE_DECLARE_ARGS2(onAppActiveTick,						NETWORK_FIXED_MESSAGE,
									COMPONENT_TYPE,							componentType, 
									COMPONENT_ID,							componentID)

	// The baseEntity request is created in a new space.
	CELLAPPMGR_MESSAGE_DECLARE_STREAM(reqCreateCellEntityInNewSpace,		NETWORK_VARIABLE_MESSAGE)

	// baseEntity requests to be restored in a new space.
	CELLAPPMGR_MESSAGE_DECLARE_STREAM(reqRestoreSpaceInCell,				NETWORK_VARIABLE_MESSAGE)

	// Message forwarding, an app wants to forward the message to an app via this app.
	CELLAPPMGR_MESSAGE_DECLARE_STREAM(forwardMessage,						NETWORK_VARIABLE_MESSAGE)

	// request to shut down the server
	CELLAPPMGR_MESSAGE_DECLARE_STREAM(reqCloseServer,						NETWORK_VARIABLE_MESSAGE)

	// Request to query watcher data
	CELLAPPMGR_MESSAGE_DECLARE_STREAM(queryWatcher,							NETWORK_VARIABLE_MESSAGE)

	// Update the cellapp information.
	CELLAPPMGR_MESSAGE_DECLARE_ARGS4(updateCellapp,							NETWORK_FIXED_MESSAGE,
									COMPONENT_ID,							componentID,
									ENTITY_ID,								numEntities,
									float,									load,
									uint32,									flags)

	// start profile
	CELLAPPMGR_MESSAGE_DECLARE_STREAM(startProfile,							NETWORK_VARIABLE_MESSAGE)

	// request to force kill the current app
	CELLAPPMGR_MESSAGE_DECLARE_STREAM(reqKillServer,						NETWORK_VARIABLE_MESSAGE)

	// cellapp synchronizes its own initialization information
	CELLAPPMGR_MESSAGE_DECLARE_ARGS4(onCellappInitProgress,					NETWORK_FIXED_MESSAGE,
									COMPONENT_ID,							cid,
									float,									progress,
									COMPONENT_ORDER,						componentGlobalOrder,
									COMPONENT_ORDER,						componentGroupOrder)

	// Query all relevant process load information
	CELLAPPMGR_MESSAGE_DECLARE_STREAM(queryAppsLoads,						NETWORK_VARIABLE_MESSAGE)

	// Query all relevant process space information
	CELLAPPMGR_MESSAGE_DECLARE_STREAM(querySpaces,							NETWORK_VARIABLE_MESSAGE)

	// Update the relevant process space information, note: this spaceData is not the spaceData described in the API documentation
	// refers to some information about space
	CELLAPPMGR_MESSAGE_DECLARE_STREAM(updateSpaceData,						NETWORK_VARIABLE_MESSAGE)

	// The tool requests to change the space viewer (with add and delete functions)
	CELLAPPMGR_MESSAGE_DECLARE_STREAM(setSpaceViewer,						NETWORK_VARIABLE_MESSAGE)

NETWORK_INTERFACE_DECLARE_END()

#ifdef DEFINE_IN_INTERFACE
	#undef DEFINE_IN_INTERFACE
#endif

}
#endif
