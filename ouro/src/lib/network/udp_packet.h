// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_SOCKETUDPPACKET_H
#define OURO_SOCKETUDPPACKET_H
	
#include "network/packet.h"
#include "common/objectpool.h"
	
namespace Ouroboros{
namespace Network
{
class UDPPacket : public Packet
{
public:
	typedef OUROShared_ptr< SmartPoolObject< UDPPacket > > SmartPoolObjectPtr;
	static SmartPoolObjectPtr createSmartPoolObj(const std::string& logPoint);
	static ObjectPool<UDPPacket>& ObjPool();
	static UDPPacket* createPoolObject(const std::string& logPoint);
	static void reclaimPoolObject(UDPPacket* obj);
	static void destroyObjPool();
	static size_t maxBufferSize();

    UDPPacket(MessageID msgID = 0, size_t res = 0);
	virtual ~UDPPacket(void);
	
	int recvFromEndPoint(EndPoint & ep, Address* pAddr = NULL);

	virtual void onReclaimObject();
};

typedef SmartPointer<UDPPacket> UDPPacketPtr;
}
}

#endif // OURO_SOCKETUDPPACKET_H
