// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com


#include "udp_packet.h"
#ifndef CODE_INLINE
#include "udp_packet.inl"
#endif
#include "network/bundle.h"
#include "network/endpoint.h"
#include "network/network_interface.h"
#include "network/message_handler.h"

namespace Ouroboros {
namespace Network
{
//-------------------------------------------------------------------------------------
static ObjectPool<UDPPacket> _g_objPool("UDPPacket");
ObjectPool<UDPPacket>& UDPPacket::ObjPool()
{
	return _g_objPool;
}

//-------------------------------------------------------------------------------------
UDPPacket* UDPPacket::createPoolObject()
{
	return _g_objPool.createObject();
}

//-------------------------------------------------------------------------------------
void UDPPacket::reclaimPoolObject(UDPPacket* obj)
{
	_g_objPool.reclaimObject(obj);
}

//-------------------------------------------------------------------------------------
void UDPPacket::destroyObjPool()
{
	DEBUG_MSG(fmt::format("UDPPacket::destroyObjPool(): size {}.\n",
		_g_objPool.size()));

	_g_objPool.destroy();
}

//-------------------------------------------------------------------------------------
UDPPacket::SmartPoolObjectPtr UDPPacket::createSmartPoolObj()
{
	return SmartPoolObjectPtr(new SmartPoolObject<UDPPacket>(ObjPool().createObject(), _g_objPool));
}

//-------------------------------------------------------------------------------------
UDPPacket::UDPPacket(MessageID msgID, size_t res):
Packet(msgID, false, res)
{
	data_resize(maxBufferSize());
	wpos(0);
}

//-------------------------------------------------------------------------------------
UDPPacket::~UDPPacket(void)
{
}

//-------------------------------------------------------------------------------------
size_t UDPPacket::maxBufferSize()
{
	return PACKET_MAX_SIZE_UDP;
}

//-------------------------------------------------------------------------------------
void UDPPacket::onReclaimObject()
{
	Packet::onReclaimObject();
	data_resize(maxBufferSize());
}

//-------------------------------------------------------------------------------------
int UDPPacket::recvFromEndPoint(EndPoint & ep, Address* pAddr)
{
	OURO_ASSERT(maxBufferSize() > wpos());

	// Recvfrom returns when the received size is greater than the receive buffer-1
	int len = ep.recvfrom(data() + wpos(), size() - wpos(),
		(u_int16_t*)&pAddr->port, (u_int32_t*)&pAddr->ip);

	if(len > 0)
		wpos(wpos() + len);

	return len;
}

//-------------------------------------------------------------------------------------
}
}
