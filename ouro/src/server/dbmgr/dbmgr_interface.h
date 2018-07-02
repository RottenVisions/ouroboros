// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com


#if defined(DEFINE_IN_INTERFACE)
	#undef OURO_DBMGR_INTERFACE_H
#endif


#ifndef OURO_DBMGR_INTERFACE_H
#define OURO_DBMGR_INTERFACE_H

// common include
#if defined(DBMGR)
#include "dbmgr.h"
#endif
#include "dbmgr_interface_macros.h"
#include "network/interface_defs.h"

//#define NDEBUG
// windows include
#if OURO_PLATFORM == PLATFORM_WIN32
#else
// linux include
#endif

namespace Ouroboros{

/**
	Dbmgr message macro, parameter is stream, need to unlock
*/

/**
	DBMGR all message interfaces are defined here
*/
NETWORK_INTERFACE_DECLARE_BEGIN(DbmgrInterface)
	// An app registers its own interface address to this app
	DBMGR_MESSAGE_DECLARE_ARGS11(onRegisterNewApp,					NETWORK_VARIABLE_MESSAGE,
									int32,							uid,
									std::string,					username,
									COMPONENT_TYPE,					componentType,
									COMPONENT_ID,					componentID,
									COMPONENT_ORDER,				globalorderID,
									COMPONENT_ORDER,				grouporderID,
									uint32,							intaddr,
									uint16,							intport,
									uint32,							extaddr,
									uint16,							extport,
									std::string,					extaddrEx)

	// An app actively requests a look.
	DBMGR_MESSAGE_DECLARE_ARGS0(lookApp,							NETWORK_FIXED_MESSAGE)

	// An app requests to see the app load status.
	DBMGR_MESSAGE_DECLARE_ARGS0(queryLoad,							NETWORK_FIXED_MESSAGE)

	// An app requesting an entityID segment
	DBMGR_MESSAGE_DECLARE_ARGS2(onReqAllocEntityID,					NETWORK_FIXED_MESSAGE,
								COMPONENT_TYPE,						componentType,
								COMPONENT_ID,						componentID)

	// Global data changes
	DBMGR_MESSAGE_DECLARE_STREAM(onBroadcastGlobalDataChanged,		NETWORK_VARIABLE_MESSAGE)

	// An app informs the app that it is active.
	DBMGR_MESSAGE_DECLARE_ARGS2(onAppActiveTick,					NETWORK_FIXED_MESSAGE,
									COMPONENT_TYPE,					componentType,
									COMPONENT_ID,					componentID)

	// Loginapp requests to create an account.
	DBMGR_MESSAGE_DECLARE_STREAM(reqCreateAccount,					NETWORK_VARIABLE_MESSAGE)
	DBMGR_MESSAGE_DECLARE_STREAM(onCreateAccountCBFromInterfaces,	NETWORK_VARIABLE_MESSAGE)

	// Login account.
	DBMGR_MESSAGE_DECLARE_STREAM(onAccountLogin,					NETWORK_VARIABLE_MESSAGE)
	DBMGR_MESSAGE_DECLARE_STREAM(onLoginAccountCBBFromInterfaces,	NETWORK_VARIABLE_MESSAGE)

	// Baseapp query account information.
	DBMGR_MESSAGE_DECLARE_ARGS8(queryAccount,						NETWORK_VARIABLE_MESSAGE,
									std::string,					accountName,
									std::string,					password,
									bool,							needCheckPassword,
									COMPONENT_ID,					componentID,
									ENTITY_ID,						entityID,
									DBID,							entityDBID,
									uint32,							ip,
									uint16,							port)

	// Baseapp on the account online.
	DBMGR_MESSAGE_DECLARE_ARGS3(onAccountOnline,					NETWORK_VARIABLE_MESSAGE,
									std::string,					accountName,
									COMPONENT_ID,					componentID,
									ENTITY_ID,						entityID)

	// Baseapp on the entity offline.
	DBMGR_MESSAGE_DECLARE_ARGS3(onEntityOffline,					NETWORK_FIXED_MESSAGE,
									DBID,							dbid,
									uint16,							sid,
									uint16,							dbInterfaceIndex)

	// Request to erase the client request task.
	DBMGR_MESSAGE_DECLARE_ARGS1(eraseClientReq,						NETWORK_VARIABLE_MESSAGE,
									std::string,					logkey)

	// Database query
	DBMGR_MESSAGE_DECLARE_STREAM(executeRawDatabaseCommand,			NETWORK_VARIABLE_MESSAGE)

	// An entity archive
	DBMGR_MESSAGE_DECLARE_STREAM(writeEntity,						NETWORK_VARIABLE_MESSAGE)

	// Delete an entity's archive
	DBMGR_MESSAGE_DECLARE_STREAM(removeEntity,						NETWORK_VARIABLE_MESSAGE)

	// Request to delete entity from database
	DBMGR_MESSAGE_DECLARE_STREAM(deleteEntityByDBID,				NETWORK_VARIABLE_MESSAGE)

	// Check if an entity is checked out from the database by dbid
	DBMGR_MESSAGE_DECLARE_STREAM(lookUpEntityByDBID,				NETWORK_VARIABLE_MESSAGE)

	// Request to shut down the server
	DBMGR_MESSAGE_DECLARE_STREAM(reqCloseServer,					NETWORK_VARIABLE_MESSAGE)

	// An app informs the app that it is active.
	DBMGR_MESSAGE_DECLARE_ARGS7(queryEntity,						NETWORK_VARIABLE_MESSAGE,
									uint16,							dbInterfaceIndex,
									COMPONENT_ID,					componentID,
									int8,							queryMode,
									DBID,							dbid,
									std::string,					entityType,
									CALLBACK_ID,					callbackID,
									ENTITY_ID,						entityID)

	// Entity automatic loading function
	DBMGR_MESSAGE_DECLARE_STREAM(entityAutoLoad,					NETWORK_VARIABLE_MESSAGE)

	// Synchronous entity flow template
	DBMGR_MESSAGE_DECLARE_STREAM(syncEntityStreamTemplate,			NETWORK_VARIABLE_MESSAGE)

	// Request for watcher data
	DBMGR_MESSAGE_DECLARE_STREAM(queryWatcher,						NETWORK_VARIABLE_MESSAGE)

	// Recharge request
	DBMGR_MESSAGE_DECLARE_STREAM(charge,							NETWORK_VARIABLE_MESSAGE)

	// Recharge callback
	DBMGR_MESSAGE_DECLARE_STREAM(onChargeCB,						NETWORK_VARIABLE_MESSAGE)

	// Activate the callback.
	DBMGR_MESSAGE_DECLARE_ARGS1(accountActivate,					NETWORK_VARIABLE_MESSAGE,
									std::string,					scode)

	// The account request resets the password.
	DBMGR_MESSAGE_DECLARE_ARGS1(accountReqResetPassword,			NETWORK_VARIABLE_MESSAGE,
									std::string,					accountName)

	// The account has completed resetting the password.
	DBMGR_MESSAGE_DECLARE_ARGS3(accountResetPassword,				NETWORK_VARIABLE_MESSAGE,
									std::string,					accountName,
									std::string,					newpassword,
									std::string,					code)

	// The account request is bound to the mailbox.
	DBMGR_MESSAGE_DECLARE_ARGS4(accountReqBindMail,					NETWORK_VARIABLE_MESSAGE,
									ENTITY_ID,						entityID,
									std::string,					accountName,
									std::string,					password,
									std::string,					email)

	// The account is complete with a binding email.
	DBMGR_MESSAGE_DECLARE_ARGS2(accountBindMail,					NETWORK_VARIABLE_MESSAGE,
									std::string,					username,
									std::string,					code)

	// Account change password.
	DBMGR_MESSAGE_DECLARE_ARGS4(accountNewPassword,					NETWORK_VARIABLE_MESSAGE,
									ENTITY_ID,						entityID,
									std::string,					accountName,
									std::string,					password,
									std::string,					newpassword)

	// Start profile
	DBMGR_MESSAGE_DECLARE_STREAM(startProfile,						NETWORK_VARIABLE_MESSAGE)

	// Request to force kill the current app
	DBMGR_MESSAGE_DECLARE_STREAM(reqKillServer,						NETWORK_VARIABLE_MESSAGE)

NETWORK_INTERFACE_DECLARE_END()

#ifdef DEFINE_IN_INTERFACE
	#undef DEFINE_IN_INTERFACE
#endif

}
#endif
