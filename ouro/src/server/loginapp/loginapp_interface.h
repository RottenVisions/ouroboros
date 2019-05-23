// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#if defined(DEFINE_IN_INTERFACE)
	#undef OURO_LOGINAPP_INTERFACE_H
#endif


#ifndef OURO_LOGINAPP_INTERFACE_H
#define OURO_LOGINAPP_INTERFACE_H

// common include	
#if defined(LOGINAPP)
#include "loginapp.h"
#endif
#include "loginapp_interface_macros.h"
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
	LOGINAPP all message interfaces are defined here
*/
NETWORK_INTERFACE_DECLARE_BEGIN(LoginappInterface)
	// Client protocol export.
	LOGINAPP_MESSAGE_EXPOSED(importClientMessages)
	LOGINAPP_MESSAGE_DECLARE_ARGS0(importClientMessages,							NETWORK_FIXED_MESSAGE)

	// Error code description export.
	LOGINAPP_MESSAGE_EXPOSED(importServerErrorsDescr)
	LOGINAPP_MESSAGE_DECLARE_ARGS0(importServerErrorsDescr,							NETWORK_FIXED_MESSAGE)

	// Client SDK export.
	LOGINAPP_MESSAGE_EXPOSED(importClientSDK)
	LOGINAPP_MESSAGE_DECLARE_STREAM(importClientSDK,								NETWORK_VARIABLE_MESSAGE)

	// An app actively requests a disconnection.
	LOGINAPP_MESSAGE_DECLARE_ARGS0(reqClose,										NETWORK_FIXED_MESSAGE)

	// An app actively requests a look.
	LOGINAPP_MESSAGE_DECLARE_ARGS0(lookApp,											NETWORK_FIXED_MESSAGE)

	// An app requests to view the app's load status.
	LOGINAPP_MESSAGE_DECLARE_ARGS0(queryLoad,										NETWORK_FIXED_MESSAGE)

	// hello handshake.
	NETWORK_MESSAGE_EXPOSED(Loginapp, hello)
	LOGINAPP_MESSAGE_DECLARE_STREAM(hello,											NETWORK_VARIABLE_MESSAGE)

	// An app tells the app that it is active.
	LOGINAPP_MESSAGE_EXPOSED(onClientActiveTick)
	LOGINAPP_MESSAGE_DECLARE_ARGS0(onClientActiveTick,								NETWORK_FIXED_MESSAGE)
	
	// request to create an account
	LOGINAPP_MESSAGE_EXPOSED(reqCreateAccount)
	LOGINAPP_MESSAGE_DECLARE_STREAM(reqCreateAccount,								NETWORK_VARIABLE_MESSAGE)

	LOGINAPP_MESSAGE_EXPOSED(reqCreateMailAccount)
	LOGINAPP_MESSAGE_DECLARE_STREAM(reqCreateMailAccount,							NETWORK_VARIABLE_MESSAGE)

	// Reset account password request
	LOGINAPP_MESSAGE_EXPOSED(reqAccountResetPassword)
	LOGINAPP_MESSAGE_DECLARE_ARGS1(reqAccountResetPassword,							NETWORK_VARIABLE_MESSAGE,
									std::string,									accountName)

	// Reset callback for account password request
	LOGINAPP_MESSAGE_DECLARE_ARGS4(onReqAccountResetPasswordCB,						NETWORK_VARIABLE_MESSAGE,
									std::string,									accountName,
									std::string,									email,
									SERVER_ERROR_CODE,								failedcode,
									std::string,									code)
	// User login to the server
	LOGINAPP_MESSAGE_EXPOSED(login)
	LOGINAPP_MESSAGE_DECLARE_STREAM(login,											NETWORK_VARIABLE_MESSAGE)

	// An app requests to get a callback for the entityID segment
	LOGINAPP_MESSAGE_DECLARE_ARGS3(onDbmgrInitCompleted,							NETWORK_VARIABLE_MESSAGE,
									COMPONENT_ORDER,								startGlobalOrder,
									COMPONENT_ORDER,								startGroupOrder,
									std::string,									digest)

	// An app tells the app that it is active.
	LOGINAPP_MESSAGE_DECLARE_ARGS2(onAppActiveTick,									NETWORK_FIXED_MESSAGE,
									COMPONENT_TYPE,									componentType, 
									COMPONENT_ID,									componentID)

	// Query from dbmgr to user legality results
	LOGINAPP_MESSAGE_DECLARE_STREAM(onLoginAccountQueryResultFromDbmgr,				NETWORK_VARIABLE_MESSAGE)

	// The login gateway address returned by baseappmgr
	LOGINAPP_MESSAGE_DECLARE_ARGS5(onLoginAccountQueryBaseappAddrFromBaseappmgr,	NETWORK_VARIABLE_MESSAGE,
									std::string,									loginName, 
									std::string,									accountName,
									std::string,									addr,
									uint16,											tcp_port,
									uint16,											udp_port)

	// Request to create an account with dbmgr to return results
	LOGINAPP_MESSAGE_DECLARE_STREAM(onReqCreateAccountResult,						NETWORK_VARIABLE_MESSAGE)
	LOGINAPP_MESSAGE_DECLARE_STREAM(onReqCreateMailAccountResult,					NETWORK_VARIABLE_MESSAGE)

	// dbmgr account activation returns
	LOGINAPP_MESSAGE_DECLARE_ARGS2(onAccountActivated,								NETWORK_VARIABLE_MESSAGE,
									std::string,									code, 
									bool,											success)
	
	// dbmgr account binding email return
	LOGINAPP_MESSAGE_DECLARE_ARGS2(onAccountBindedEmail,							NETWORK_VARIABLE_MESSAGE,
									std::string,									code, 
									bool,											success)

	// dbmgr account reset password return
	LOGINAPP_MESSAGE_DECLARE_ARGS2(onAccountResetPassword,							NETWORK_VARIABLE_MESSAGE,
									std::string,									code, 
									bool,											success)

	// baseapp request binding email (return to find the address of the loginapp)
	LOGINAPP_MESSAGE_DECLARE_ARGS6(onReqAccountBindEmailAllocCallbackLoginapp,		NETWORK_VARIABLE_MESSAGE,
									COMPONENT_ID,									reqBaseappID,
									ENTITY_ID,										entityID,
									std::string,									accountName,
									std::string,									email,
									SERVER_ERROR_CODE,								failedcode,
									std::string,									code)

	// request to shut down the server
	LOGINAPP_MESSAGE_DECLARE_STREAM(reqCloseServer,									NETWORK_VARIABLE_MESSAGE)


	// Request to query watcher data
	LOGINAPP_MESSAGE_DECLARE_STREAM(queryWatcher,									NETWORK_VARIABLE_MESSAGE)

	// baseapp synchronizes its own initialization information
	LOGINAPP_MESSAGE_DECLARE_ARGS1(onBaseappInitProgress,							NETWORK_FIXED_MESSAGE,
									float,											progress)

	// start profile
	LOGINAPP_MESSAGE_DECLARE_STREAM(startProfile,									NETWORK_VARIABLE_MESSAGE)

	// request to force kill the current app
	LOGINAPP_MESSAGE_DECLARE_STREAM(reqKillServer,									NETWORK_VARIABLE_MESSAGE)

NETWORK_INTERFACE_DECLARE_END()

#ifdef DEFINE_IN_INTERFACE
	#undef DEFINE_IN_INTERFACE
#endif

}
#endif
