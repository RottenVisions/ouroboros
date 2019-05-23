// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com


#include "packet_filter.h"

#ifndef CODE_INLINE
#include "packet_filter.inl"
#endif

#include "network/channel.h"
#include "network/network_interface.h"
#include "network/packet_receiver.h"
#include "network/packet_sender.h"

namespace Ouroboros { 
namespace Network
{
//-------------------------------------------------------------------------------------
Reason PacketFilter::send(Channel * pChannel, PacketSender& sender, Packet * pPacket, int userarg)
{
	return sender.processFilterPacket(pChannel, pPacket, userarg);
}

//-------------------------------------------------------------------------------------
Reason PacketFilter::recv(Channel * pChannel, PacketReceiver & receiver, Packet * pPacket)
{
	return receiver.processFilteredPacket(pChannel, pPacket);
}

//-------------------------------------------------------------------------------------
} 
}
