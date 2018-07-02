// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#if defined(DEFINE_IN_INTERFACE)
	#undef OURO_CLIENT_INTERFACE_H
#endif


#ifndef OURO_CLIENT_INTERFACE_H
#define OURO_CLIENT_INTERFACE_H

// common include
#if defined(CLIENT)
#include "clientapp.h"
#endif
#include "client_interface_macros.h"
#include "network/interface_defs.h"
#include "server/server_errors.h"
#include "entitydef/common.h"
#include "common.h"

namespace Ouroboros{

/**
	CLIENT All the message interfaces defined in this
*/
NETWORK_INTERFACE_DECLARE_BEGIN(ClientInterface)
	// Service end hello return
	CLIENT_MESSAGE_DECLARE_STREAM(onHelloCB,								NETWORK_VARIABLE_MESSAGE)

	// And server version do not match
	CLIENT_MESSAGE_DECLARE_STREAM(onVersionNotMatch,						NETWORK_VARIABLE_MESSAGE)

	// And server-side scripting layer version mismatch
	CLIENT_MESSAGE_DECLARE_STREAM(onScriptVersionNotMatch,					NETWORK_VARIABLE_MESSAGE)

	// Create an account to fail.
	CLIENT_MESSAGE_DECLARE_STREAM(onCreateAccountResult,					NETWORK_VARIABLE_MESSAGE)

	// A successful login.
	CLIENT_MESSAGE_DECLARE_STREAM(onLoginSuccessfully,						NETWORK_VARIABLE_MESSAGE)

	// The Login failed.
	CLIENT_MESSAGE_DECLARE_STREAM(onLoginFailed,							NETWORK_VARIABLE_MESSAGE)

	// The server has been created a associated with the client of the proxy Entity || Login to the gateway was successful.
	CLIENT_MESSAGE_DECLARE_ARGS3(onCreatedProxies,							NETWORK_VARIABLE_MESSAGE,
								uint64,										rndUUID,
								ENTITY_ID,									eid,
								std::string,								entityType)

	// Login to Gateway failed.
	CLIENT_MESSAGE_DECLARE_ARGS1(onLoginBaseappFailed,						NETWORK_FIXED_MESSAGE,
								SERVER_ERROR_CODE,							failedcode)

	// Login to Gateway failed.
	CLIENT_MESSAGE_DECLARE_ARGS1(onReloginBaseappFailed,					NETWORK_FIXED_MESSAGE,
								SERVER_ERROR_CODE,							failedcode)

	// On the server entity has entered the game world.
	CLIENT_MESSAGE_DECLARE_STREAM(onEntityEnterWorld,						NETWORK_VARIABLE_MESSAGE)

	// On the server entity has left the game world.
	CLIENT_MESSAGE_DECLARE_ARGS1(onEntityLeaveWorld,						NETWORK_FIXED_MESSAGE,
								ENTITY_ID,									eid)

	// On the server entity has left the game world.
	CLIENT_MESSAGE_DECLARE_STREAM(onEntityLeaveWorldOptimized,				NETWORK_VARIABLE_MESSAGE)

	// Tell the client a entity Destroyed. Such entity is usually also not onEntityEnterWorld.
	CLIENT_MESSAGE_DECLARE_ARGS1(onEntityDestroyed,							NETWORK_FIXED_MESSAGE,
								ENTITY_ID,									eid)

	// On the server entity has entered the space.
	CLIENT_MESSAGE_DECLARE_STREAM(onEntityEnterSpace,						NETWORK_VARIABLE_MESSAGE)

	// On the server entity have left the space.
	CLIENT_MESSAGE_DECLARE_ARGS1(onEntityLeaveSpace,						NETWORK_FIXED_MESSAGE,
								ENTITY_ID,									eid)

	// The remote method call on the entity
	CLIENT_MESSAGE_DECLARE_STREAM(onRemoteMethodCall,						NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onRemoteMethodCallOptimized,				NETWORK_VARIABLE_MESSAGE)

	// Being kicked out of the server
	CLIENT_MESSAGE_DECLARE_ARGS1(onKicked,									NETWORK_FIXED_MESSAGE,
								SERVER_ERROR_CODE,							failedcode)

	// Server to update the properties of the entity
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdatePropertys,						NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdatePropertysOptimized,				NETWORK_VARIABLE_MESSAGE)

	// Server forced setting of the position and orientation of the entity
	CLIENT_MESSAGE_DECLARE_STREAM(onSetEntityPosAndDir,						NETWORK_VARIABLE_MESSAGE)

	// A server update package
	CLIENT_MESSAGE_DECLARE_ARGS3(onUpdateBasePos,							NETWORK_FIXED_MESSAGE,
								float,										x,
								float,										y,
								float,										z)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateBaseDir,							NETWORK_VARIABLE_MESSAGE)

	CLIENT_MESSAGE_DECLARE_ARGS2(onUpdateBasePosXZ,							NETWORK_FIXED_MESSAGE,
								float,										x,
								float,										z)

	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData,								NETWORK_VARIABLE_MESSAGE)

	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_ypr,							NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_yp,							NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_yr,							NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_pr,							NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_y,							NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_p,							NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_r,							NETWORK_VARIABLE_MESSAGE)

	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xz,							NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xz_ypr,						NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xz_yp,						NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xz_yr,						NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xz_pr,						NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xz_y,						NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xz_p,						NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xz_r,						NETWORK_VARIABLE_MESSAGE)

	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xyz,							NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xyz_ypr,						NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xyz_yp,						NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xyz_yr,						NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xyz_pr,						NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xyz_y,						NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xyz_p,						NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xyz_r,						NETWORK_VARIABLE_MESSAGE)

	// download stream Started
	CLIENT_MESSAGE_DECLARE_ARGS3(onStreamDataStarted,						NETWORK_VARIABLE_MESSAGE,
								int16,										id,
								uint32,										datasize,
								std::string,								descr)

	// Received streamData
	CLIENT_MESSAGE_DECLARE_STREAM(onStreamDataRecv,							NETWORK_VARIABLE_MESSAGE)

	// download streamC ompleted
	CLIENT_MESSAGE_DECLARE_ARGS1(onStreamDataCompleted,						NETWORK_FIXED_MESSAGE,
								int16,										id)

	// Import Protocol
	CLIENT_MESSAGE_DECLARE_STREAM(onImportClientMessages,					NETWORK_VARIABLE_MESSAGE)

	// Import entitydef
	CLIENT_MESSAGE_DECLARE_STREAM(onImportClientEntityDef,					NETWORK_VARIABLE_MESSAGE)

	// Error Code Description export
	CLIENT_MESSAGE_DECLARE_STREAM(onImportServerErrorsDescr,				NETWORK_VARIABLE_MESSAGE)

	// Service end initialization spacedata
	CLIENT_MESSAGE_DECLARE_STREAM(initSpaceData,							NETWORK_VARIABLE_MESSAGE)

	// The service end of the set. spacedata
	CLIENT_MESSAGE_DECLARE_ARGS3(setSpaceData,								NETWORK_VARIABLE_MESSAGE,
								SPACE_ID,									spaceID,
								std::string,								key,
								std::string,								val)

	// Service end removed spacedata
	CLIENT_MESSAGE_DECLARE_ARGS2(delSpaceData,								NETWORK_VARIABLE_MESSAGE,
								SPACE_ID,									spaceID,
								std::string,								key)

	// Reset account password to request to return
	CLIENT_MESSAGE_DECLARE_ARGS1(onReqAccountResetPasswordCB,				NETWORK_FIXED_MESSAGE,
								SERVER_ERROR_CODE,							failedcode)

	// Reset account password to request to return
	CLIENT_MESSAGE_DECLARE_ARGS1(onReqAccountBindEmailCB,					NETWORK_FIXED_MESSAGE,
								SERVER_ERROR_CODE,							failedcode)

	// Reset account password to request to return
	CLIENT_MESSAGE_DECLARE_ARGS1(onReqAccountNewPasswordCB,					NETWORK_FIXED_MESSAGE,
								SERVER_ERROR_CODE,							failedcode)

	// Re-login the gateway to success
	CLIENT_MESSAGE_DECLARE_STREAM(onReloginBaseappSuccessfully,				NETWORK_VARIABLE_MESSAGE)

	// Tell the client: you the current charge or to cancel the control who of the displacement of synchronization
	CLIENT_MESSAGE_DECLARE_ARGS2(onControlEntity,							NETWORK_FIXED_MESSAGE,
									ENTITY_ID,								eid,
									int8,									isControlled)

	// Server heartbeat callback
	CLIENT_MESSAGE_DECLARE_ARGS0(onAppActiveTickCB,							NETWORK_FIXED_MESSAGE)

	NETWORK_INTERFACE_DECLARE_END()

#ifdef DEFINE_IN_INTERFACE
	#undef DEFINE_IN_INTERFACE
#endif

}

#endif // OURO_CLIENT_INTERFACE_H
