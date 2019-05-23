// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_SOCKETTCPPACKET_H
#define OURO_SOCKETTCPPACKET_H
	
// common include
#include "network/packet.h"
#include "common/objectpool.h"
	
namespace Ouroboros{
namespace Network
{
class TCPPacket : public Packet
{
public:
	typedef KBEShared_ptr< SmartPoolObject< TCPPacket > > SmartPoolObjectPtr;
	static SmartPoolObjectPtr createSmartPoolObj(const std::string& logPoint);
	static ObjectPool<TCPPacket>& ObjPool();
	static TCPPacket* createPoolObject(const std::string& logPoint);
	static void reclaimPoolObject(TCPPacket* obj);
	static void destroyObjPool();

	static size_t maxBufferSize();

    TCPPacket(MessageID msgID = 0, size_t res = 0);
	virtual ~TCPPacket(void);
	
	int recvFromEndPoint(EndPoint & ep, Address* pAddr = NULL);

	virtual void onReclaimObject();
};

typedef SmartPointer<TCPPacket> TCPPacketPtr;
}
}

#endif // OURO_SOCKETTCPPACKET_H
