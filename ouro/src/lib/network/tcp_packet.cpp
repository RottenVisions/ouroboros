// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com


#include "tcp_packet.h"
#ifndef CODE_INLINE
#include "tcp_packet.inl"
#endif
#include "network/bundle.h"
#include "network/endpoint.h"
#include "network/network_interface.h"
#include "network/message_handler.h"

namespace Ouroboros {
namespace Network
{
//-------------------------------------------------------------------------------------
static ObjectPool<TCPPacket> _g_objPool("TCPPacket");
ObjectPool<TCPPacket>& TCPPacket::ObjPool()
{
	return _g_objPool;
}

//-------------------------------------------------------------------------------------
TCPPacket* TCPPacket::createPoolObject()
{
	return _g_objPool.createObject();
}

//-------------------------------------------------------------------------------------
void TCPPacket::reclaimPoolObject(TCPPacket* obj)
{
	_g_objPool.reclaimObject(obj);
}

//-------------------------------------------------------------------------------------
void TCPPacket::destroyObjPool()
{
	DEBUG_MSG(fmt::format("TCPPacket::destroyObjPool(): size {}.\n",
		_g_objPool.size()));

	_g_objPool.destroy();
}

//-------------------------------------------------------------------------------------
TCPPacket::SmartPoolObjectPtr TCPPacket::createSmartPoolObj()
{
	return SmartPoolObjectPtr(new SmartPoolObject<TCPPacket>(ObjPool().createObject(), _g_objPool));
}

//-------------------------------------------------------------------------------------
TCPPacket::TCPPacket(MessageID msgID, size_t res):
Packet(msgID, true, res)
{
	data_resize(maxBufferSize());
	wpos(0);
}

//-------------------------------------------------------------------------------------
TCPPacket::~TCPPacket(void)
{
}

//-------------------------------------------------------------------------------------
size_t TCPPacket::maxBufferSize()
{
	return PACKET_MAX_SIZE_TCP;
}

//-------------------------------------------------------------------------------------
void TCPPacket::onReclaimObject()
{
	Packet::onReclaimObject();
	data_resize(maxBufferSize());
}

//-------------------------------------------------------------------------------------
int TCPPacket::recvFromEndPoint(EndPoint & ep, Address* pAddr)
{
	//OURO_ASSERT(MessageHandlers::pMainMessageHandlers != NULL && "Must set up a MainMessageHandlers!\n");
	OURO_ASSERT(maxBufferSize() > wpos());
	int len = ep.recv(data() + wpos(), (int)(size() - wpos()));

	if(len > 0)
	{
		wpos((int)(wpos() + len));

		// Note: Must be greater than 0 otherwise DEBUG_MSG will cause WSAGetLastError to return 0 and get stuck in an infinite loop
		// DEBUG_MSG(fmt::format("TCPPacket::recvFromEndPoint: datasize={}, wpos={}.\n", len, wpos()));
	}

	return len;
}

//-------------------------------------------------------------------------------------
}
}
