// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

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
	CLIENT all message interfaces are defined here
*/
NETWORK_INTERFACE_DECLARE_BEGIN(ClientInterface)
	// The server hello returns.
	CLIENT_MESSAGE_DECLARE_STREAM(onHelloCB,								NETWORK_VARIABLE_MESSAGE)

	// does not match the version of the server
	CLIENT_MESSAGE_DECLARE_STREAM(onVersionNotMatch,						NETWORK_VARIABLE_MESSAGE)

	// does not match the script layer version of the server
	CLIENT_MESSAGE_DECLARE_STREAM(onScriptVersionNotMatch,					NETWORK_VARIABLE_MESSAGE)

	// Failed to create an account.
	CLIENT_MESSAGE_DECLARE_STREAM(onCreateAccountResult,					NETWORK_VARIABLE_MESSAGE)

		// login successful.
	CLIENT_MESSAGE_DECLARE_STREAM(onLoginSuccessfully,						NETWORK_VARIABLE_MESSAGE)

		// Login failed.
	CLIENT_MESSAGE_DECLARE_STREAM(onLoginFailed,							NETWORK_VARIABLE_MESSAGE)

	// The server has created a proxy Entity associated with the client || The login gateway succeeded.
	CLIENT_MESSAGE_DECLARE_ARGS3(onCreatedProxies,							NETWORK_VARIABLE_MESSAGE,
								uint64,										rndUUID,
								ENTITY_ID,									eid,
								std::string,								entityType)

	// Login to the gateway failed.
	CLIENT_MESSAGE_DECLARE_ARGS1(onLoginBaseappFailed,						NETWORK_FIXED_MESSAGE,
								SERVER_ERROR_CODE,							failedcode)

	// Login to the gateway failed.
	CLIENT_MESSAGE_DECLARE_ARGS1(onReloginBaseappFailed,					NETWORK_FIXED_MESSAGE,
								SERVER_ERROR_CODE,							failedcode)

	// The entity on the server has entered the game world.
	CLIENT_MESSAGE_DECLARE_STREAM(onEntityEnterWorld,						NETWORK_VARIABLE_MESSAGE)

	// The entity on the server has left the game world.
	CLIENT_MESSAGE_DECLARE_ARGS1(onEntityLeaveWorld,						NETWORK_FIXED_MESSAGE,
								ENTITY_ID,									eid)

	// The entity on the server has left the game world.
	CLIENT_MESSAGE_DECLARE_STREAM(onEntityLeaveWorldOptimized,				NETWORK_VARIABLE_MESSAGE)

	// Tell the client that an entity has been destroyed. This type of entity is usually not onEntityEnterWorld.
	CLIENT_MESSAGE_DECLARE_ARGS1(onEntityDestroyed,							NETWORK_FIXED_MESSAGE,
								ENTITY_ID,									eid)

	// The entity on the server has entered the space.
	CLIENT_MESSAGE_DECLARE_STREAM(onEntityEnterSpace,						NETWORK_VARIABLE_MESSAGE)

	// The entity on the server has left the space.
	CLIENT_MESSAGE_DECLARE_ARGS1(onEntityLeaveSpace,						NETWORK_FIXED_MESSAGE,
								ENTITY_ID,									eid)

	// remote call entity method
	CLIENT_MESSAGE_DECLARE_STREAM(onRemoteMethodCall,						NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onRemoteMethodCallOptimized,				NETWORK_VARIABLE_MESSAGE)

	// kicked out of the server
	CLIENT_MESSAGE_DECLARE_ARGS1(onKicked,									NETWORK_FIXED_MESSAGE,
								SERVER_ERROR_CODE,							failedcode)

	// Server update entity attribute
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdatePropertys,						NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdatePropertysOptimized,				NETWORK_VARIABLE_MESSAGE)

	// The server forces the location and orientation of the entity
	CLIENT_MESSAGE_DECLARE_STREAM(onSetEntityPosAndDir,						NETWORK_VARIABLE_MESSAGE)

	// server update package
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

	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_ypr_optimized,				NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_yp_optimized,				NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_yr_optimized,				NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_pr_optimized,				NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_y_optimized,					NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_p_optimized,					NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_r_optimized,					NETWORK_VARIABLE_MESSAGE)

	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xz_optimized,				NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xz_ypr_optimized,			NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xz_yp_optimized,				NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xz_yr_optimized,				NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xz_pr_optimized,				NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xz_y_optimized,				NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xz_p_optimized,				NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xz_r_optimized,				NETWORK_VARIABLE_MESSAGE)

	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xyz_optimized,				NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xyz_ypr_optimized,			NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xyz_yp_optimized,			NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xyz_yr_optimized,			NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xyz_pr_optimized,			NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xyz_y_optimized,				NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xyz_p_optimized,				NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xyz_r_optimized,				NETWORK_VARIABLE_MESSAGE)

	// download stream started
	CLIENT_MESSAGE_DECLARE_ARGS3(onStreamDataStarted,						NETWORK_VARIABLE_MESSAGE,
								int16,										id,
								uint32,										datasize,
								std::string,								descr)

	// Received streamData
	CLIENT_MESSAGE_DECLARE_STREAM(onStreamDataRecv,							NETWORK_VARIABLE_MESSAGE)

	// download stream is done
	CLIENT_MESSAGE_DECLARE_ARGS1(onStreamDataCompleted,						NETWORK_FIXED_MESSAGE,
								int16,										id)

	// import protocol
	CLIENT_MESSAGE_DECLARE_STREAM(onImportClientMessages,					NETWORK_VARIABLE_MESSAGE)
	
	// Import entitydef
	CLIENT_MESSAGE_DECLARE_STREAM(onImportClientEntityDef,					NETWORK_VARIABLE_MESSAGE)

	// error code description export
	CLIENT_MESSAGE_DECLARE_STREAM(onImportServerErrorsDescr,				NETWORK_VARIABLE_MESSAGE)

	// Receive import sdk message
	CLIENT_MESSAGE_DECLARE_STREAM(onImportClientSDK,						NETWORK_VARIABLE_MESSAGE)

	// Server initialization spacedata
	CLIENT_MESSAGE_DECLARE_STREAM(initSpaceData,							NETWORK_VARIABLE_MESSAGE)

	// The server sets the spacedata
	CLIENT_MESSAGE_DECLARE_ARGS3(setSpaceData,								NETWORK_VARIABLE_MESSAGE,
								SPACE_ID,									spaceID,
								std::string,								key,
								std::string,								val)

	// The server deleted the spacedata
	CLIENT_MESSAGE_DECLARE_ARGS2(delSpaceData,								NETWORK_VARIABLE_MESSAGE,
								SPACE_ID,									spaceID,
								std::string,								key)

	// Reset account password request return
	CLIENT_MESSAGE_DECLARE_ARGS1(onReqAccountResetPasswordCB,				NETWORK_FIXED_MESSAGE,
								SERVER_ERROR_CODE,							failedcode)

	// Reset account password request return
	CLIENT_MESSAGE_DECLARE_ARGS1(onReqAccountBindEmailCB,					NETWORK_FIXED_MESSAGE,
								SERVER_ERROR_CODE,							failedcode)

	// Reset account password request return
	CLIENT_MESSAGE_DECLARE_ARGS1(onReqAccountNewPasswordCB,					NETWORK_FIXED_MESSAGE,
								SERVER_ERROR_CODE,							failedcode)

	// Re-login to the gateway successfully
	CLIENT_MESSAGE_DECLARE_STREAM(onReloginBaseappSuccessfully,				NETWORK_VARIABLE_MESSAGE)
									
	// Tell the client: who is currently responsible (or cancels) who controls the displacement synchronization
	CLIENT_MESSAGE_DECLARE_ARGS2(onControlEntity,							NETWORK_FIXED_MESSAGE,
									ENTITY_ID,								eid,
									int8,									isControlled)

	// server heartbeat callback
	CLIENT_MESSAGE_DECLARE_ARGS0(onAppActiveTickCB,							NETWORK_FIXED_MESSAGE)

	NETWORK_INTERFACE_DECLARE_END()

#ifdef DEFINE_IN_INTERFACE
	#undef DEFINE_IN_INTERFACE
#endif

}

#endif // OURO_CLIENT_INTERFACE_H
