// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com


#ifndef OURO_DEBUG_OPTION_H
#define OURO_DEBUG_OPTION_H
#include "common/common.h"

namespace Ouroboros{

namespace Network
{

/**
	This switch sets whether the packet always carries length information, which provides some convenience when coupling some front ends.
	 If false, some fixed-length packets do not carry length information and are resolved by the peer.
*/
extern bool g_packetAlwaysContainLength;

/**
Do you need to output any received and sent packets as text to the log for debugging
		g_trace_packet:
			0: Not output
			1: Hex output
			2: Character stream output
			3: Decimal output
		use_logfile:
			Whether to log a separate log file to record the contents of the package, the file name is usually
			appname_packetlogs.log
		g_trace_packet_disables:
			Turn off the output of some packages
*/
extern uint8 g_trace_packet;
extern bool g_trace_encrypted_packet;
extern std::vector<std::string> g_trace_packet_disables;
extern bool g_trace_packet_use_logfile;

}

/**
	Whether to output the creation of the entity, the script to obtain attributes, initialization attributes, and other debugging information.
*/
extern bool g_debugEntity;

/**
	Apps publication status, this value can be obtained in the script
		0 : debug
		1 : release
*/
extern int8 g_appPublish;

}

#endif // OURO_DEBUG_OPTION_H
