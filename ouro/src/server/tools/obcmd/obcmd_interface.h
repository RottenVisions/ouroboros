// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com


#if defined(DEFINE_IN_INTERFACE)
	#undef OURO_KBCMD_TOOL_INTERFACE_H
#endif


#ifndef OURO_KBCMD_TOOL_INTERFACE_H
#define OURO_KBCMD_TOOL_INTERFACE_H

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
	KBCMD message macro, parameter is stream, need to unlock
*/

/**
	KBCMD all message interfaces are defined here
*/
NETWORK_INTERFACE_DECLARE_BEGIN(KBCMDInterface)

	// An app actively requests a look.
	KBCMD_MESSAGE_DECLARE_ARGS0(lookApp, NETWORK_FIXED_MESSAGE)

	// An app informs the app that it is active.
	KBCMD_MESSAGE_DECLARE_ARGS2(onAppActiveTick, NETWORK_FIXED_MESSAGE,
		COMPONENT_TYPE, componentType,
		COMPONENT_ID, componentID)

NETWORK_INTERFACE_DECLARE_END()

#ifdef DEFINE_IN_INTERFACE
	#undef DEFINE_IN_INTERFACE
#endif

}

#endif // OURO_KBCMD_TOOL_INTERFACE_H
