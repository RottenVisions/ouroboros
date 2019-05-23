// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_NETWORKKCPPACKET_RECEIVER_H
#define OURO_NETWORKKCPPACKET_RECEIVER_H

#include "network/udp_packet_receiver.h"

namespace Ouroboros { 
namespace Network
{

class KCPPacketReceiver : public UDPPacketReceiver
{
public:
	typedef KBEShared_ptr< SmartPoolObject< KCPPacketReceiver > > SmartPoolObjectPtr;
	static SmartPoolObjectPtr createSmartPoolObj(const std::string& logPoint);
	static ObjectPool<KCPPacketReceiver>& ObjPool();
	static KCPPacketReceiver* createPoolObject(const std::string& logPoint);
	static void reclaimPoolObject(KCPPacketReceiver* obj);
	static void destroyObjPool();

	KCPPacketReceiver():UDPPacketReceiver(){}
	KCPPacketReceiver(EndPoint & endpoint, NetworkInterface & networkInterface);
	virtual ~KCPPacketReceiver();

	bool processRecv(UDPPacket* pReceiveWindow);
	virtual bool processRecv(bool expectingPacket);

	virtual Reason processPacket(Channel* pChannel, Packet * pPacket);

	virtual ProtocolSubType protocolSubType() const {
		return SUB_PROTOCOL_KCP;
	}

protected:

};

}
}

#ifdef CODE_INLINE
#include "kcp_packet_receiver.inl"
#endif
#endif // OURO_NETWORKKCPPACKET_RECEIVER_H
