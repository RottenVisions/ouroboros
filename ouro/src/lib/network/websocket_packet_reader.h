// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_WEBSOCKET_PACKET_READER_H
#define OURO_WEBSOCKET_PACKET_READER_H

#include "network/packet_reader.h"

namespace Ouroboros{
namespace Network
{

class WebSocketPacketReader : public PacketReader
{
public:
	WebSocketPacketReader(Channel* pChannel);
	virtual ~WebSocketPacketReader();

	virtual void reset();
	virtual void processMessages(Ouroboros::Network::MessageHandlers* pMsgHandlers, Packet* pPacket);

	virtual PacketReader::PACKET_READER_TYPE type() const { return PACKET_READER_TYPE_WEBSOCKET; }

protected:
};


}
}
#endif 
