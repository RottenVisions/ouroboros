// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com


#ifndef OURO_NETWORKKCPPACKET_RECEIVER_EX_H
#define OURO_NETWORKKCPPACKET_RECEIVER_EX_H

#include "network/kcp_packet_receiver.h"

namespace Ouroboros { 

class ClientObject;

namespace Network
{

class KCPPacketReceiverEx : public KCPPacketReceiver
{
public:
	KCPPacketReceiverEx(EndPoint & endpoint, NetworkInterface & networkInterface, ClientObject* pClientObject);
	~KCPPacketReceiverEx();

	virtual Channel* getChannel();
	virtual Channel* findChannel(const Address& addr);

protected:
	virtual void onGetError(Channel* pChannel, const std::string& err);

	ClientObject* pClientObject_;
};
}
}

#endif // OURO_NETWORKKCPPACKET_RECEIVER_EX_H
