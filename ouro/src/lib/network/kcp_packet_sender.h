// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com


#ifndef OURO_NETWORKKCPPACKET_SENDER_H
#define OURO_NETWORKKCPPACKET_SENDER_H

#include "common/common.h"
#include "common/timer.h"
#include "common/objectpool.h"
#include "helper/debug_helper.h"
#include "network/common.h"
#include "network/interfaces.h"
#include "network/tcp_packet.h"
#include "network/udp_packet_sender.h"
#include "network/ikcp.h"

namespace Ouroboros { 
namespace Network
{

class KCPPacketSender : public UDPPacketSender
{
public:
	typedef OUROShared_ptr< SmartPoolObject< KCPPacketSender > > SmartPoolObjectPtr;
	static SmartPoolObjectPtr createSmartPoolObj(const std::string& logPoint);
	static ObjectPool<KCPPacketSender>& ObjPool();
	static KCPPacketSender* createPoolObject(const std::string& logPoint);
	static void reclaimPoolObject(KCPPacketSender* obj);
	virtual void onReclaimObject();
	static void destroyObjPool();
	
	KCPPacketSender():UDPPacketSender(){}
	KCPPacketSender(EndPoint & endpoint, NetworkInterface & networkInterface);
	virtual ~KCPPacketSender();

	int kcp_output(const char *buf, int len, ikcpcb *kcp, Channel* pChannel);

protected:
	virtual void onSent(Packet* pPacket);
	virtual Reason processFilterPacket(Channel* pChannel, Packet * pPacket, int userarg);

};
}
}

#ifdef CODE_INLINE
#include "udp_packet_sender.inl"
#endif
#endif // OURO_NETWORKKCPPACKET_SENDER_H
