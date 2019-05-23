// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com


#if defined(DEFINE_IN_INTERFACE)
	#undef OURO_MACHINE_INTERFACE_H
#endif


#ifndef OURO_MACHINE_INTERFACE_H
#define OURO_MACHINE_INTERFACE_H

// common include	
#if defined(MACHINE)
#include "machine.h"
#endif
#include "machine_interface_macros.h"
#include "network/interface_defs.h"
//#define NDEBUG
// windows include	
#if OURO_PLATFORM == PLATFORM_WIN32
#else
// linux include
#endif
	
namespace Ouroboros{

/**
	Machine all message interfaces are defined here
*/
NETWORK_INTERFACE_DECLARE_BEGIN(MachineInterface)
	// other components broadcast their own interface address to the app
	MACHINE_MESSAGE_DECLARE_ARGS25(onBroadcastInterface,			NETWORK_VARIABLE_MESSAGE,
									int32,							uid, 
									std::string,					username,
									COMPONENT_TYPE,					componentType, 
									COMPONENT_ID,					componentID, 
									COMPONENT_ID,					componentIDEx, 
									COMPONENT_ORDER,				globalorderid, 
									COMPONENT_ORDER,				grouporderid, 
									COMPONENT_GUS,					gus,
									uint32,							intaddr, 
									uint16,							intport,
									uint32,							extaddr, 
									uint16,							extport,
									std::string,					extaddrEx,
									uint32,							pid,
									float,							cpu, 
									float,							mem, 
									uint32,							usedmem,
									int8,							state,
									uint32,							machineID, 
									uint64,							extradata,
									uint64,							extradata1,
									uint64,							extradata2,
									uint64,							extradata3,
									uint32,							backRecvAddr,
									uint16,							backRecvPort)
	
	// other components request the app to get the address of a component class
	MACHINE_MESSAGE_DECLARE_ARGS7(onFindInterfaceAddr,				NETWORK_VARIABLE_MESSAGE,
									int32,							uid, 
									std::string,					username,
									COMPONENT_TYPE,					componentType, 
									COMPONENT_ID,					componentID, 
									COMPONENT_TYPE,					findComponentType,
									uint32,							addr, 
									uint16,							finderRecvPort)
						
	// Query all interface information
	MACHINE_MESSAGE_DECLARE_ARGS3(onQueryAllInterfaceInfos,			NETWORK_VARIABLE_MESSAGE,
									int32,							uid, 
									std::string,					username,
									uint16,							finderRecvPort)
		
	// Query all machine processes
	MACHINE_MESSAGE_DECLARE_ARGS3(onQueryMachines,					NETWORK_VARIABLE_MESSAGE,
									int32,							uid, 
									std::string,					username,
									uint16,							finderRecvPort)

	MACHINE_MESSAGE_DECLARE_ARGS6(queryComponentID,					NETWORK_VARIABLE_MESSAGE,
									COMPONENT_TYPE,					componentType,
									COMPONENT_ID,					componentID,
									int32,							uid,
									uint16,							finderRecvPort,
									int,							macMD5,
									int32,							pid)
	// An app actively requests a look.
	MACHINE_MESSAGE_DECLARE_ARGS0(lookApp,							NETWORK_FIXED_MESSAGE)

	// An app requests to view the app's load status.
	MACHINE_MESSAGE_DECLARE_ARGS0(queryLoad,						NETWORK_FIXED_MESSAGE)

	// start the server
	MACHINE_MESSAGE_DECLARE_STREAM(startserver,						NETWORK_VARIABLE_MESSAGE)

	// turn off the server
	MACHINE_MESSAGE_DECLARE_STREAM(stopserver,						NETWORK_VARIABLE_MESSAGE)

	// turn off the server
	MACHINE_MESSAGE_DECLARE_STREAM(killserver,						NETWORK_VARIABLE_MESSAGE)

	// request to force kill the current app
	MACHINE_MESSAGE_DECLARE_STREAM(reqKillServer,					NETWORK_VARIABLE_MESSAGE)

NETWORK_INTERFACE_DECLARE_END()

#ifdef DEFINE_IN_INTERFACE
	#undef DEFINE_IN_INTERFACE
#endif

}
#endif
