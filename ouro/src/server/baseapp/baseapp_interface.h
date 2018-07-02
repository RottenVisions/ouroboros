// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com


#if defined(DEFINE_IN_INTERFACE)
	#undef OURO_BASEAPP_INTERFACE_H
#endif


#ifndef OURO_BASEAPP_INTERFACE_H
#define OURO_BASEAPP_INTERFACE_H

// common include
#if defined(BASEAPP)
#include "baseapp.h"
#endif
#include "baseapp_interface_macros.h"
#include "entity_interface_macros.h"
#include "proxy_interface_macros.h"
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
	The BASEAPP all message interface is defined here
*/
NETWORK_INTERFACE_DECLARE_BEGIN(BaseappInterface)
	// The client requests the import protocol.
	BASEAPP_MESSAGE_EXPOSED(importClientMessages)
	BASEAPP_MESSAGE_DECLARE_ARGS0(importClientMessages,								NETWORK_FIXED_MESSAGE)

	// Client entitydef export.
	BASEAPP_MESSAGE_EXPOSED(importClientEntityDef)
	BASEAPP_MESSAGE_DECLARE_ARGS0(importClientEntityDef,							NETWORK_FIXED_MESSAGE)

	// An app actively requests a disconnection.
	BASEAPP_MESSAGE_DECLARE_ARGS0(reqClose,											NETWORK_FIXED_MESSAGE)

	// An app actively requests a look.
	BASEAPP_MESSAGE_DECLARE_ARGS0(lookApp,											NETWORK_FIXED_MESSAGE)

	// An app requests to see the app load status.
	BASEAPP_MESSAGE_DECLARE_ARGS0(queryLoad,										NETWORK_FIXED_MESSAGE)

	// Console executes python statements remotely.
	BASEAPP_MESSAGE_DECLARE_STREAM(onExecScriptCommand,								NETWORK_VARIABLE_MESSAGE)

	// An app registers its own interface address to this app
	BASEAPP_MESSAGE_DECLARE_ARGS11(onRegisterNewApp,								NETWORK_VARIABLE_MESSAGE,
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

	// Dbmgr tells the address of other baseapp or cellapp that has been started
	// Current app needs to actively establish a connection with them
	BASEAPP_MESSAGE_DECLARE_ARGS11(onGetEntityAppFromDbmgr,							NETWORK_VARIABLE_MESSAGE,
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

	// An app requesting a callback for an entityID segment
	BASEAPP_MESSAGE_DECLARE_ARGS2(onReqAllocEntityID,								NETWORK_FIXED_MESSAGE,
									ENTITY_ID,										startID,
									ENTITY_ID,										endID)


	// An app requesting a callback for an entityID segment
	BASEAPP_MESSAGE_DECLARE_ARGS6(onDbmgrInitCompleted,								NETWORK_VARIABLE_MESSAGE,
									GAME_TIME,										gametime,
									ENTITY_ID,										startID,
									ENTITY_ID,										endID,
									COMPONENT_ORDER,								startGlobalOrder,
									COMPONENT_ORDER,								startGroupOrder,
									std::string,									digest)

	// Hello handshaking.
	BASEAPP_MESSAGE_EXPOSED(hello)
	BASEAPP_MESSAGE_DECLARE_STREAM(hello,											NETWORK_VARIABLE_MESSAGE)

	// Global data changes
	BASEAPP_MESSAGE_DECLARE_STREAM(onBroadcastGlobalDataChanged,					NETWORK_VARIABLE_MESSAGE)
	BASEAPP_MESSAGE_DECLARE_STREAM(onBroadcastBaseAppDataChanged,					NETWORK_VARIABLE_MESSAGE)

	// An app informs the app that it is active.
	BASEAPP_MESSAGE_DECLARE_ARGS2(onAppActiveTick,									NETWORK_FIXED_MESSAGE,
									COMPONENT_TYPE,									componentType,
									COMPONENT_ID,									componentID)

	// An app informs the app that it is active.
	BASEAPP_MESSAGE_EXPOSED(onClientActiveTick)
	BASEAPP_MESSAGE_DECLARE_ARGS0(onClientActiveTick,								NETWORK_FIXED_MESSAGE)

	// Received baseappmgr decided to request a baseapp request createEntityAnywhere executed on this baseapp
	BASEAPP_MESSAGE_DECLARE_STREAM(onCreateEntityAnywhere,							NETWORK_VARIABLE_MESSAGE)

	// createEntityAnywhere succeeds callback messages to the entity of the baseapp that initiated the createEntityAnywhere layer.
	BASEAPP_MESSAGE_DECLARE_STREAM(onCreateEntityAnywhereCallback,					NETWORK_FIXED_MESSAGE)

	// After createEntityRemotely succeeds callback message to the entity of the baseapp that initiated the layer createEntityRemotely.
	BASEAPP_MESSAGE_DECLARE_STREAM(onCreateEntityRemotely,							NETWORK_FIXED_MESSAGE)

	// After createEntityRemotely succeeds callback message to the entity of the baseapp that initiated the layer createEntityRemotely.
	BASEAPP_MESSAGE_DECLARE_STREAM(onCreateEntityRemotelyCallback,					 NETWORK_FIXED_MESSAGE)

	// The createCellEntity cell entity creates a successful callback.
	BASEAPP_MESSAGE_DECLARE_ARGS3(onEntityGetCell,									NETWORK_FIXED_MESSAGE,
									ENTITY_ID,										id,
									COMPONENT_ID,									componentID,
									SPACE_ID,										spaceID)

	// The createCellEntity cell entity creates a successful callback.
	BASEAPP_MESSAGE_DECLARE_ARGS1(onCreateCellFailure,								NETWORK_FIXED_MESSAGE,
									ENTITY_ID,										entityID)

	// Loginapp registers itself with an account to be logged in, forwarded by baseappmgr.
	BASEAPP_MESSAGE_DECLARE_STREAM(registerPendingLogin,							NETWORK_VARIABLE_MESSAGE)

	// Automatic entity load information retrieved from the database returns
	BASEAPP_MESSAGE_DECLARE_STREAM(onEntityAutoLoadCBFromDBMgr,						NETWORK_VARIABLE_MESSAGE)

	// The front-end requests to log in to the gateway.
	BASEAPP_MESSAGE_EXPOSED(loginBaseapp)
	BASEAPP_MESSAGE_DECLARE_ARGS2(loginBaseapp,										NETWORK_VARIABLE_MESSAGE,
									std::string,									accountName,
									std::string,									password)

	// The front-end request re-login to the gateway.
	BASEAPP_MESSAGE_EXPOSED(reloginBaseapp)
	BASEAPP_MESSAGE_DECLARE_ARGS4(reloginBaseapp,									NETWORK_VARIABLE_MESSAGE,
									std::string,									accountName,
									std::string,									password,
									uint64,											key,
									ENTITY_ID,										entityID)

	// Get account Entity information from dbmgr
	BASEAPP_MESSAGE_DECLARE_STREAM(onQueryAccountCBFromDbmgr,						NETWORK_VARIABLE_MESSAGE)

	// The entity receives a remote call request initiated by an entitycall on an app
	BASEAPP_MESSAGE_DECLARE_STREAM(onEntityCall,									NETWORK_VARIABLE_MESSAGE)

	// Client access entity cell method
	BASEAPP_MESSAGE_EXPOSED(onRemoteCallCellMethodFromClient)
	BASEAPP_MESSAGE_DECLARE_STREAM(onRemoteCallCellMethodFromClient,				NETWORK_VARIABLE_MESSAGE)

	// Client update data
	BASEAPP_MESSAGE_EXPOSED(onUpdateDataFromClient)
	BASEAPP_MESSAGE_DECLARE_STREAM(onUpdateDataFromClient,							NETWORK_VARIABLE_MESSAGE)
	BASEAPP_MESSAGE_EXPOSED(onUpdateDataFromClientForControlledEntity)
	BASEAPP_MESSAGE_DECLARE_STREAM(onUpdateDataFromClientForControlledEntity,		NETWORK_VARIABLE_MESSAGE)

	// executeRawDatabaseCommand callback from dbmgr
	BASEAPP_MESSAGE_DECLARE_STREAM(onExecuteRawDatabaseCommandCB,					NETWORK_VARIABLE_MESSAGE)

	// Cellapp backup cell data of entity
	BASEAPP_MESSAGE_DECLARE_STREAM(onBackupEntityCellData,							NETWORK_VARIABLE_MESSAGE)

	// cellapp writeToDB complete
	BASEAPP_MESSAGE_DECLARE_STREAM(onCellWriteToDBCompleted,						NETWORK_VARIABLE_MESSAGE)

	// Cellapp forwards entity message to client
	BASEAPP_MESSAGE_DECLARE_STREAM(forwardMessageToClientFromCellapp,				NETWORK_VARIABLE_MESSAGE)

	// Cellapp forwards the entity message to the cellEntity of a baseEntity
	BASEAPP_MESSAGE_DECLARE_STREAM(forwardMessageToCellappFromCellapp,				NETWORK_VARIABLE_MESSAGE)

	// Request to shut down the server
	BASEAPP_MESSAGE_DECLARE_STREAM(reqCloseServer,									NETWORK_VARIABLE_MESSAGE)

	// Write entity to db callback.
	BASEAPP_MESSAGE_DECLARE_ARGS5(onWriteToDBCallback,								NETWORK_FIXED_MESSAGE,
									ENTITY_ID,										eid,
									DBID,											entityDBID,
									uint16,											dbInterfaceIndex,
									CALLBACK_ID,									callbackID,
									bool,											success)

	// createEntityFromDBID callback
	BASEAPP_MESSAGE_DECLARE_STREAM(onCreateEntityFromDBIDCallback,					NETWORK_FIXED_MESSAGE)

	// Callback for createEntityAnywhereFromDBID
	BASEAPP_MESSAGE_DECLARE_STREAM(onGetCreateEntityAnywhereFromDBIDBestBaseappID,	NETWORK_FIXED_MESSAGE)

	// Callback for createEntityAnywhereFromDBID
	BASEAPP_MESSAGE_DECLARE_STREAM(onCreateEntityAnywhereFromDBIDCallback,			NETWORK_FIXED_MESSAGE)

	// Callback for createEntityAnywhereFromDBID
	BASEAPP_MESSAGE_DECLARE_STREAM(createEntityAnywhereFromDBIDOtherBaseapp,		NETWORK_FIXED_MESSAGE)

	// Callback for createEntityAnywhereFromDBID
	BASEAPP_MESSAGE_DECLARE_ARGS5(onCreateEntityAnywhereFromDBIDOtherBaseappCallback,NETWORK_VARIABLE_MESSAGE,
									COMPONENT_ID,									createByBaseappID,
									std::string,									entityType,
									ENTITY_ID,										createdEntityID,
									CALLBACK_ID,									callbackID,
									DBID,											dbid)

	// Callback for createEntityRemotelyFromDBID
	BASEAPP_MESSAGE_DECLARE_STREAM(onCreateEntityRemotelyFromDBIDCallback,			NETWORK_FIXED_MESSAGE)

	// Callback for createEntityRemotelyFromDBID
	BASEAPP_MESSAGE_DECLARE_STREAM(createEntityRemotelyFromDBIDOtherBaseapp,		NETWORK_FIXED_MESSAGE)

	// Callback for createEntityRemotelyFromDBID
	BASEAPP_MESSAGE_DECLARE_ARGS5(onCreateEntityRemotelyFromDBIDOtherBaseappCallback,NETWORK_VARIABLE_MESSAGE,
									COMPONENT_ID,									createByBaseappID,
									std::string,									entityType,
									ENTITY_ID,										createdEntityID,
									CALLBACK_ID,									callbackID,
									DBID,											dbid)

	// Request for watcher data
	BASEAPP_MESSAGE_DECLARE_STREAM(queryWatcher,									NETWORK_VARIABLE_MESSAGE)

	// Recharge callback
	BASEAPP_MESSAGE_DECLARE_STREAM(onChargeCB,										NETWORK_VARIABLE_MESSAGE)

	// Start profile
	BASEAPP_MESSAGE_DECLARE_STREAM(startProfile,									NETWORK_VARIABLE_MESSAGE)

	// Request to delete entity from database
	BASEAPP_MESSAGE_DECLARE_STREAM(deleteEntityByDBIDCB,							NETWORK_VARIABLE_MESSAGE)

	// lookUpEntityByDBID callback
	BASEAPP_MESSAGE_DECLARE_STREAM(lookUpEntityByDBIDCB,							NETWORK_VARIABLE_MESSAGE)

	// The space on a baseapp restores the cell. Determines whether the current baseapp has related entities and needs to restore the cell.
	BASEAPP_MESSAGE_DECLARE_STREAM(onRestoreSpaceCellFromOtherBaseapp,				NETWORK_VARIABLE_MESSAGE)

	// Request to return results on other APP disaster recovery.
	BASEAPP_MESSAGE_DECLARE_STREAM(onRequestRestoreCB,								NETWORK_VARIABLE_MESSAGE)

	// Request binding email
	BASEAPP_MESSAGE_EXPOSED(reqAccountBindEmail)
	BASEAPP_MESSAGE_DECLARE_ARGS3(reqAccountBindEmail,								NETWORK_VARIABLE_MESSAGE,
									ENTITY_ID,										entityID,
									std::string,									password,
									std::string,									email)

	// Request callback for binding email request
	BASEAPP_MESSAGE_DECLARE_ARGS5(onReqAccountBindEmailCBFromDBMgr,					NETWORK_VARIABLE_MESSAGE,
									ENTITY_ID,										entityID,
									std::string,									accountName,
									std::string,									email,
									SERVER_ERROR_CODE,								failedcode,
									std::string,									code)

	// Baseapp request binding e-mail (return to need to find loginapp address)
	BASEAPP_MESSAGE_DECLARE_ARGS7(onReqAccountBindEmailCBFromBaseappmgr,			NETWORK_VARIABLE_MESSAGE,
									ENTITY_ID,										entityID,
									std::string,									accountName,
									std::string,									email,
									SERVER_ERROR_CODE,								failedcode,
									std::string,									code,
									std::string,									loginappCBHost,
									uint16,											loginappCBPort)

	// Request to change password
	BASEAPP_MESSAGE_EXPOSED(reqAccountNewPassword)
	BASEAPP_MESSAGE_DECLARE_ARGS3(reqAccountNewPassword,							NETWORK_VARIABLE_MESSAGE,
									ENTITY_ID,										entityID,
									std::string,									oldpassword,
									std::string,									newpassword)

	// Request a change callback
	BASEAPP_MESSAGE_DECLARE_ARGS3(onReqAccountNewPasswordCB,						NETWORK_VARIABLE_MESSAGE,
									ENTITY_ID,										entityID,
									std::string,									accountName,
									SERVER_ERROR_CODE,								failedcode)

	// Request to force kill the current app
	BASEAPP_MESSAGE_DECLARE_STREAM(reqKillServer,									NETWORK_VARIABLE_MESSAGE)

	//--------------------------------------------Entity----------------------------------------------------------
	// Remote call entity method
	ENTITY_MESSAGE_EXPOSED(onRemoteMethodCall)
	ENTITY_MESSAGE_DECLARE_STREAM(onRemoteMethodCall,								NETWORK_VARIABLE_MESSAGE)

	// Cellapp informs that the cell part of the entity is destroyed or lost
	ENTITY_MESSAGE_DECLARE_STREAM(onLoseCell,										NETWORK_VARIABLE_MESSAGE)

	// The client directly sends messages to the cell entity
	ENTITY_MESSAGE_EXPOSED(forwardEntityMessageToCellappFromClient)
	ENTITY_MESSAGE_DECLARE_STREAM(forwardEntityMessageToCellappFromClient,			NETWORK_VARIABLE_MESSAGE)

	// An entity request teleport to the space of the entity
	ENTITY_MESSAGE_DECLARE_ARGS3(reqTeleportOther,									NETWORK_FIXED_MESSAGE,
								ENTITY_ID,											reqTeleportEntityID,
								COMPONENT_ID,										reqTeleportEntityAppID,
								COMPONENT_ID,										reqTeleportEntityBaseAppID)

	// Callback result of an entity request teleport
	ENTITY_MESSAGE_DECLARE_ARGS2(onTeleportCB,										NETWORK_FIXED_MESSAGE,
								SPACE_ID,											spaceID,
								bool,												fromCellTeleport)

	// Callback result of an entity request teleport
	ENTITY_MESSAGE_DECLARE_ARGS1(onGetDBID,											NETWORK_FIXED_MESSAGE,
								DBID,												dbid)

	// Entity request to migrate to space procedure on another cellapp begins
	ENTITY_MESSAGE_DECLARE_ARGS2(onMigrationCellappStart,							NETWORK_FIXED_MESSAGE,
								COMPONENT_ID,										sourceCellAppID,
								COMPONENT_ID,										targetCellAppID)

	// Entity request migration to another cellapp on the end of the space process
	ENTITY_MESSAGE_DECLARE_ARGS2(onMigrationCellappEnd,								NETWORK_FIXED_MESSAGE,
								COMPONENT_ID,										sourceCellAppID,
								COMPONENT_ID,										targetCellAppID)

	//--------------------------------------------Proxy---------------------------------------------------------
	/**
		Remote call entity method
	*/
	//PROXY_MESSAGE_EXPOSED(onClientGetCell)
	//PROXY_MESSAGE_DECLARE_ARGS0(onClientGetCell,									NETWORK_FIXED_MESSAGE)

NETWORK_INTERFACE_DECLARE_END()

#ifdef DEFINE_IN_INTERFACE
	#undef DEFINE_IN_INTERFACE
#endif

}
#endif
