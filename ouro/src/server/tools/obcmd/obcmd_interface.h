// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com


#if defined(DEFINE_IN_INTERFACE)
	#undef OURO_OBCMD_TOOL_INTERFACE_H
#endif


#ifndef OURO_OBCMD_TOOL_INTERFACE_H
#define OURO_OBCMD_TOOL_INTERFACE_H

// common include	
#if defined(INTERFACES)
#include "obcmd.h"
#endif
#include "obcmd_interface_macros.h"
#include "network/interface_defs.h"
//#define NDEBUG
// windows include	
#if OURO_PLATFORM == PLATFORM_WIN32
#else
// linux include
#endif
	
namespace Ouroboros{

/**
	OBCMD message macro, the parameter is stream, you need to unlock it yourself.
*/

/**
	OBCMD all message interfaces are defined here
*/
NETWORK_INTERFACE_DECLARE_BEGIN(OBCMDInterface)

	// An app actively requests a look.
	OBCMD_MESSAGE_DECLARE_ARGS0(lookApp, NETWORK_FIXED_MESSAGE)

	// An app tells the app that it is active.
	OBCMD_MESSAGE_DECLARE_ARGS2(onAppActiveTick, NETWORK_FIXED_MESSAGE,
		COMPONENT_TYPE, componentType,
		COMPONENT_ID, componentID)

NETWORK_INTERFACE_DECLARE_END()

#ifdef DEFINE_IN_INTERFACE
	#undef DEFINE_IN_INTERFACE
#endif

}

#endif // OURO_OBCMD_TOOL_INTERFACE_H
