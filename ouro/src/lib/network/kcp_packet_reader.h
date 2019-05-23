// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_KCP_PACKET_READER_H
#define OURO_KCP_PACKET_READER_H

#include "network/packet_reader.h"

namespace Ouroboros{
namespace Network
{

class KCPPacketReader : public PacketReader
{
public:
	KCPPacketReader(Channel* pChannel);
	virtual ~KCPPacketReader();

	virtual void reset();
	virtual void processMessages(Ouroboros::Network::MessageHandlers* pMsgHandlers, Packet* pPacket);

	virtual PacketReader::PACKET_READER_TYPE type() const { return PACKET_READER_TYPE_KCP; }

protected:
};


}
}
#endif 
