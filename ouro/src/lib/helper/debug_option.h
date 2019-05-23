// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com


#ifndef OURO_DEBUG_OPTION_H
#define OURO_DEBUG_OPTION_H
#include "common/common.h"

namespace Ouroboros{

namespace Network
{

/** 
	This switch sets whether the packet always carries length information, which provides some convenience when coupling some front ends.
	 if it is false, some fixed-length data packets do not carry length information, which is resolved by the peer itself.
*/
extern bool g_packetAlwaysContainLength;

/**
Is it necessary to output any received and sent packages to the log as text for debugging?
		g_trace_packet:
			0: no output
			1: hex output
			2: Character stream output
			3: decimal output
		use_logfile:
			Whether to separate a log file to record the contents of the package, the file name is usually
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
	Whether to output the creation of the entity, the script to get the properties, the initialization properties and other debugging information.
*/
extern bool g_debugEntity;

/**
	Apps release status, which can be obtained in the script
		0 : debug
		1 : release
*/
extern int8 g_appPublish;

}

#endif // OURO_DEBUG_OPTION_H
