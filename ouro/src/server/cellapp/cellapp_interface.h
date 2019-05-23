// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#if defined(DEFINE_IN_INTERFACE)
	#undef OURO_CELLAPP_INTERFACE_H
#endif


#ifndef OURO_CELLAPP_INTERFACE_H
#define OURO_CELLAPP_INTERFACE_H

// common include	
#if defined(CELLAPP)
#include "entity.h"
#include "cellapp.h"
#endif
#include "cellapp_interface_macros.h"
#include "entity_interface_macros.h"
#include "network/interface_defs.h"
//#define NDEBUG
// windows include	
#if OURO_PLATFORM == PLATFORM_WIN32
#else
// linux include
#endif
	
namespace Ouroboros{

/**
	Cellapp all message interfaces are defined here
*/
NETWORK_INTERFACE_DECLARE_BEGIN(CellappInterface)
	// An app registers its own interface address to this app
	CELLAPP_MESSAGE_DECLARE_ARGS11(onRegisterNewApp,								NETWORK_VARIABLE_MESSAGE,
									int32,											uid, 
									std::string,									username,
									COMPONENT_TYPE,									componentType, 
									COMPONENT_ID,									componentID, 
									COMPONENT_ORDER,								globalorderID,
									COMPONENT_ORDER,								grouporderID,
									uint32,											intaddr, 
									uint16,											intport,
									uint32,											extaddr, 
									uint16,											extport,
									std::string,									extaddrEx)

	// An app actively requests a look.
	CELLAPP_MESSAGE_DECLARE_ARGS0(lookApp,											NETWORK_FIXED_MESSAGE)

	// An app requests to view the app's load status.
	CELLAPP_MESSAGE_DECLARE_ARGS0(queryLoad,										NETWORK_FIXED_MESSAGE)

	// console remotely executes python statements.
	CELLAPP_MESSAGE_DECLARE_STREAM(onExecScriptCommand,								NETWORK_VARIABLE_MESSAGE)

	// dbmgr tells the address of other baseapp or cellapp that has been started
	// The current app needs to actively connect with them.
	CELLAPP_MESSAGE_DECLARE_ARGS11(onGetEntityAppFromDbmgr,							NETWORK_VARIABLE_MESSAGE,
									int32,											uid, 
									std::string,									username,
									COMPONENT_TYPE,									componentType, 
									COMPONENT_ID,									componentID, 
									COMPONENT_ORDER,								globalorderID,
									COMPONENT_ORDER,								grouporderID,
									uint32,											intaddr, 
									uint16,											intport,
									uint32,											extaddr, 
									uint16,											extport,
									std::string,									extaddrEx)

	// An app requests to get a callback for the entityID segment
	CELLAPP_MESSAGE_DECLARE_ARGS2(onReqAllocEntityID,								NETWORK_FIXED_MESSAGE,
									ENTITY_ID,										startID,
									ENTITY_ID,										endID)

	// An app requests to get a callback for the entityID segment
	CELLAPP_MESSAGE_DECLARE_ARGS6(onDbmgrInitCompleted,								NETWORK_VARIABLE_MESSAGE,
									GAME_TIME,										gametime, 
									ENTITY_ID,										startID,
									ENTITY_ID,										endID,
									COMPONENT_ORDER,								startGlobalOrder,
									COMPONENT_ORDER,								startGroupOrder,
									std::string,									digest)

	// Global data changes
	CELLAPP_MESSAGE_DECLARE_STREAM(onBroadcastGlobalDataChanged,					NETWORK_VARIABLE_MESSAGE)
	CELLAPP_MESSAGE_DECLARE_STREAM(onBroadcastCellAppDataChanged,					NETWORK_VARIABLE_MESSAGE)

	// The baseEntity request is created in a new space.
	CELLAPP_MESSAGE_DECLARE_STREAM(onCreateCellEntityInNewSpaceFromBaseapp,			NETWORK_VARIABLE_MESSAGE)

	// baseEntity requests to be restored in a new space.
	CELLAPP_MESSAGE_DECLARE_STREAM(onRestoreSpaceInCellFromBaseapp,					NETWORK_VARIABLE_MESSAGE)

	// Other APP requests are recovering from this disaster.
	CELLAPP_MESSAGE_DECLARE_STREAM(requestRestore,									NETWORK_VARIABLE_MESSAGE)
	
	// baseapp requests to create an entity on this cellapp.
	CELLAPP_MESSAGE_DECLARE_STREAM(onCreateCellEntityFromBaseapp,					NETWORK_VARIABLE_MESSAGE)

	// Destroy a cellEntity.
	CELLAPP_MESSAGE_DECLARE_ARGS1(onDestroyCellEntityFromBaseapp,					NETWORK_FIXED_MESSAGE,
									ENTITY_ID,										eid)

	// An app tells the app that it is active.
	CELLAPP_MESSAGE_DECLARE_ARGS2(onAppActiveTick,									NETWORK_FIXED_MESSAGE,
									COMPONENT_TYPE,									componentType, 
									COMPONENT_ID,									componentID)

	// entity receives a remote call request, initiated by an entitycall on an app
	CELLAPP_MESSAGE_DECLARE_STREAM(onEntityCall,									NETWORK_VARIABLE_MESSAGE)

	// client accesses the cell method of the entity
	CELLAPP_MESSAGE_DECLARE_STREAM(onRemoteCallMethodFromClient,					NETWORK_VARIABLE_MESSAGE)

	// client update data
	CELLAPP_MESSAGE_DECLARE_STREAM(onUpdateDataFromClient,							NETWORK_VARIABLE_MESSAGE)
	CELLAPP_MESSAGE_DECLARE_STREAM(onUpdateDataFromClientForControlledEntity,		NETWORK_VARIABLE_MESSAGE)

	// executeRawDatabaseCommand callback from dbmgr
	CELLAPP_MESSAGE_DECLARE_STREAM(onExecuteRawDatabaseCommandCB,					NETWORK_VARIABLE_MESSAGE)

	// base request to get celldata
	CELLAPP_MESSAGE_DECLARE_STREAM(reqBackupEntityCellData,							NETWORK_VARIABLE_MESSAGE)

	// base request to get WriteToDB
	CELLAPP_MESSAGE_DECLARE_STREAM(reqWriteToDBFromBaseapp,							NETWORK_VARIABLE_MESSAGE)

	// The client sends a message directly to the cell entity
	CELLAPP_MESSAGE_DECLARE_STREAM(forwardEntityMessageToCellappFromClient,			NETWORK_VARIABLE_MESSAGE)

	// request to shut down the server
	CELLAPP_MESSAGE_DECLARE_STREAM(reqCloseServer,									NETWORK_VARIABLE_MESSAGE)

	// Request to query watcher data
	CELLAPP_MESSAGE_DECLARE_STREAM(queryWatcher,									NETWORK_VARIABLE_MESSAGE)

	// start profile
	CELLAPP_MESSAGE_DECLARE_STREAM(startProfile,									NETWORK_VARIABLE_MESSAGE)

	// request teleport to the current cellapp
	CELLAPP_MESSAGE_DECLARE_STREAM(reqTeleportToCellApp,							NETWORK_VARIABLE_MESSAGE)

	// After the entity is transferred to the space on the destination cellapp, it is returned to the callback of the previous cellapp
	CELLAPP_MESSAGE_DECLARE_STREAM(reqTeleportToCellAppCB,							NETWORK_VARIABLE_MESSAGE)

	// When the cross-cellapp is transferred, the baseapp setting completion status is required to clear the flag of the cellapp record, after which the cellapp can continue the teleport.
	CELLAPP_MESSAGE_DECLARE_STREAM(reqTeleportToCellAppOver,						NETWORK_VARIABLE_MESSAGE)
		
	// real request to update the property to ghost
	CELLAPP_MESSAGE_DECLARE_STREAM(onUpdateGhostPropertys,							NETWORK_VARIABLE_MESSAGE)
	
	// ghost request to call def method real
	CELLAPP_MESSAGE_DECLARE_STREAM(onRemoteRealMethodCall,							NETWORK_VARIABLE_MESSAGE)

	// Real request to update volatile data to ghost
	CELLAPP_MESSAGE_DECLARE_STREAM(onUpdateGhostVolatileData,						NETWORK_VARIABLE_MESSAGE)

	// request to force kill the current app
	CELLAPP_MESSAGE_DECLARE_STREAM(reqKillServer,									NETWORK_VARIABLE_MESSAGE)

	// The tool requests to change the space viewer (with add and delete functions)
	CELLAPP_MESSAGE_DECLARE_STREAM(setSpaceViewer,									NETWORK_VARIABLE_MESSAGE)

	//--------------------------------------------Entity----------------------------------------------------------
	// Remote call entity method
	ENTITY_MESSAGE_DECLARE_STREAM(onRemoteMethodCall,								NETWORK_VARIABLE_MESSAGE)

	// Client set new location
	ENTITY_MESSAGE_DECLARE_ARGS2(setPosition_XZ_int,								NETWORK_FIXED_MESSAGE,
									int32,											x, 
									int32,											z)
	
	// Client set new location
	ENTITY_MESSAGE_DECLARE_ARGS3(setPosition_XYZ_int,								NETWORK_FIXED_MESSAGE,
									int32,											x, 
									int32,											y, 
									int32,											z)

	// Client set new location
	ENTITY_MESSAGE_DECLARE_ARGS2(setPosition_XZ_float,								NETWORK_FIXED_MESSAGE,
									float,											x, 
									float,											z)

	// Client set new location
	ENTITY_MESSAGE_DECLARE_ARGS3(setPosition_XYZ_float,								NETWORK_FIXED_MESSAGE,
									float,											x, 
									float,											y, 
									float,											z)

	//entity is bound to an observer (client)
	ENTITY_MESSAGE_DECLARE_ARGS0(onGetWitnessFromBase,								NETWORK_FIXED_MESSAGE)

	//entity lost an observer (client)
	ENTITY_MESSAGE_DECLARE_ARGS0(onLoseWitness,										NETWORK_FIXED_MESSAGE)
NETWORK_INTERFACE_DECLARE_END()

#ifdef DEFINE_IN_INTERFACE
	#undef DEFINE_IN_INTERFACE
#endif

}
#endif
