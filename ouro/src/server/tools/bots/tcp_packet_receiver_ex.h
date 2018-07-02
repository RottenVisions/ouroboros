// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com


#ifndef OURO_NETWORKTCPPACKET_RECEIVER_EX_H
#define OURO_NETWORKTCPPACKET_RECEIVER_EX_H

#include "network/tcp_packet_receiver.h"

namespace Ouroboros { 

class ClientObject;

namespace Network
{

class TCPPacketReceiverEx : public TCPPacketReceiver
{
public:
	TCPPacketReceiverEx(EndPoint & endpoint, NetworkInterface & networkInterface, ClientObject* pClientObject);
	~TCPPacketReceiverEx();

	virtual Channel* getChannel();

protected:
	virtual void onGetError(Channel* pChannel);

	ClientObject* pClientObject_;
};
}
}

#endif // OURO_NETWORKTCPPACKET_RECEIVER_EX_H
