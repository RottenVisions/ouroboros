// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com


#ifndef OURO_NETWORKUDPPACKET_SENDER_H
#define OURO_NETWORKUDPPACKET_SENDER_H

#include "common/common.h"
#include "common/timer.h"
#include "common/objectpool.h"
#include "helper/debug_helper.h"
#include "network/common.h"
#include "network/interfaces.h"
#include "network/tcp_packet.h"
#include "network/packet_sender.h"

namespace Ouroboros { 
namespace Network
{
class EndPoint;
class Channel;
class Address;
class NetworkInterface;
class EventDispatcher;

class UDPPacketSender : public PacketSender
{
public:
	typedef OUROShared_ptr< SmartPoolObject< UDPPacketSender > > SmartPoolObjectPtr;
	static SmartPoolObjectPtr createSmartPoolObj();
	static ObjectPool<UDPPacketSender>& ObjPool();
	static UDPPacketSender* createPoolObject();
	static void reclaimPoolObject(UDPPacketSender* obj);
	virtual void onReclaimObject();
	static void destroyObjPool();
	
	UDPPacketSender():PacketSender(){}
	UDPPacketSender(EndPoint & endpoint, NetworkInterface & networkInterface);
	virtual ~UDPPacketSender();

	virtual bool processSend(Channel* pChannel, int userarg);

	virtual PacketSender::PACKET_SENDER_TYPE type() const
	{
		return UDP_PACKET_SENDER;
	}

protected:
	virtual void onGetError(Channel* pChannel);
	virtual void onSent(Packet* pPacket);
	virtual Reason processFilterPacket(Channel* pChannel, Packet * pPacket, int userarg);

};
}
}

#ifdef CODE_INLINE
#include "udp_packet_sender.inl"
#endif
#endif // OURO_NETWORKUDPPACKET_SENDER_H
