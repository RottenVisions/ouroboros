// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com


#ifndef OURO_NETWORKTCPPACKET_SENDER_EX_H
#define OURO_NETWORKTCPPACKET_SENDER_EX_H

#include "network/tcp_packet_sender.h"

namespace Ouroboros { 

class ClientObject;

namespace Network
{

class TCPPacketSenderEx : public TCPPacketSender
{
public:
	TCPPacketSenderEx(EndPoint & endpoint, NetworkInterface & networkInterface, ClientObject* pClientObject);
	~TCPPacketSenderEx();

	virtual Channel* getChannel();

protected:
	virtual void onGetError(Channel* pChannel);

	ClientObject* pClientObject_;
};
}
}


#endif // OURO_NETWORKTCPPACKET_SENDER_EX_H
