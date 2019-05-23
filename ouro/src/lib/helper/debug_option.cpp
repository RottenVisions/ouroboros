// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com


#include "debug_option.h"

namespace Ouroboros { 
	
namespace Network
{

bool g_packetAlwaysContainLength = false;

uint8 g_trace_packet = 0;
bool g_trace_encrypted_packet = true;
bool g_trace_packet_use_logfile = false;
std::vector<std::string> g_trace_packet_disables;

}

bool g_debugEntity = false;
int8 g_appPublish = 1;

}
