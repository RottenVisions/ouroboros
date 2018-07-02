// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com


#ifndef OURO_NETWORKKCPPACKET_SENDER_EX_H
#define OURO_NETWORKKCPPACKET_SENDER_EX_H

#include "network/kcp_packet_sender.h"

namespace Ouroboros { 

class ClientObject;

namespace Network
{

class KCPPacketSenderEx : public KCPPacketSender
{
public:
	KCPPacketSenderEx(EndPoint & endpoint, NetworkInterface & networkInterface, ClientObject* pClientObject);
	~KCPPacketSenderEx();

	virtual Channel* getChannel();

protected:
	virtual void onGetError(Channel* pChannel);
	virtual void onSent(Packet* pPacket);

	ClientObject* pClientObject_;
};
}
}


#endif // OURO_NETWORKKCPPACKET_SENDER_EX_H
