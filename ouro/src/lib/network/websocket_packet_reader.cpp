// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#include "websocket_packet_reader.h"
#include "network/channel.h"
#include "network/message_handler.h"
#include "network/network_stats.h"

namespace Ouroboros { 
namespace Network
{


//-------------------------------------------------------------------------------------
WebSocketPacketReader::WebSocketPacketReader(Channel* pChannel):
	PacketReader(pChannel)
{
}

//-------------------------------------------------------------------------------------
WebSocketPacketReader::~WebSocketPacketReader()
{
}

//-------------------------------------------------------------------------------------
void WebSocketPacketReader::reset()
{
	PacketReader::reset();
}

//-------------------------------------------------------------------------------------
void WebSocketPacketReader::processMessages(Ouroboros::Network::MessageHandlers* pMsgHandlers, Packet* pPacket)
{
	PacketReader::processMessages(pMsgHandlers, pPacket);
}

//-------------------------------------------------------------------------------------
} 
}
