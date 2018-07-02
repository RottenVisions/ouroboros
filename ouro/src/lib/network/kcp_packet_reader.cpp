// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#include "kcp_packet_reader.h"
#include "network/channel.h"
#include "network/message_handler.h"
#include "network/network_stats.h"

namespace Ouroboros { 
namespace Network
{


//-------------------------------------------------------------------------------------
KCPPacketReader::KCPPacketReader(Channel* pChannel):
	PacketReader(pChannel)
{
}

//-------------------------------------------------------------------------------------
KCPPacketReader::~KCPPacketReader()
{
}

//-------------------------------------------------------------------------------------
void KCPPacketReader::reset()
{
	PacketReader::reset();
}

//-------------------------------------------------------------------------------------
void KCPPacketReader::processMessages(Ouroboros::Network::MessageHandlers* pMsgHandlers, Packet* pPacket)
{
	PacketReader::processMessages(pMsgHandlers, pPacket);
}

//-------------------------------------------------------------------------------------
} 
}
