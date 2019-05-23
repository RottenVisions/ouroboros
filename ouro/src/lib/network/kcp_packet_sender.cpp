// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com


#include "kcp_packet_sender.h"
#ifndef CODE_INLINE
#include "kcp_packet_sender.inl"
#endif

#include "network/address.h"
#include "network/bundle.h"
#include "network/channel.h"
#include "network/endpoint.h"
#include "network/event_dispatcher.h"
#include "network/network_interface.h"
#include "network/event_poller.h"
#include "network/error_reporter.h"
#include "network/tcp_packet.h"
#include "network/udp_packet.h"

namespace Ouroboros { 
namespace Network
{

//-------------------------------------------------------------------------------------
static ObjectPool<KCPPacketSender> _g_objPool("KCPPacketSender");
ObjectPool<KCPPacketSender>& KCPPacketSender::ObjPool()
{
	return _g_objPool;
}

//-------------------------------------------------------------------------------------
KCPPacketSender* KCPPacketSender::createPoolObject(const std::string& logPoint)
{
	return _g_objPool.createObject(logPoint);
}

//-------------------------------------------------------------------------------------
void KCPPacketSender::reclaimPoolObject(KCPPacketSender* obj)
{
	_g_objPool.reclaimObject(obj);
}

//-------------------------------------------------------------------------------------
void KCPPacketSender::onReclaimObject()
{
}

//-------------------------------------------------------------------------------------
void KCPPacketSender::destroyObjPool()
{
	DEBUG_MSG(fmt::format("KCPPacketSender::destroyObjPool(): size {}.\n", 
		_g_objPool.size()));

	_g_objPool.destroy();
}

//-------------------------------------------------------------------------------------
KCPPacketSender::SmartPoolObjectPtr KCPPacketSender::createSmartPoolObj(const std::string& logPoint)
{
	return SmartPoolObjectPtr(new SmartPoolObject<KCPPacketSender>(ObjPool().createObject(logPoint), _g_objPool));
}

//-------------------------------------------------------------------------------------
KCPPacketSender::KCPPacketSender(EndPoint & endpoint,
	   NetworkInterface & networkInterface	) :
	UDPPacketSender(endpoint, networkInterface)
{
}

//-------------------------------------------------------------------------------------
KCPPacketSender::~KCPPacketSender()
{
	//DEBUG_MSG("KCPPacketSender::~KCPPacketSender()\n");
}

//-------------------------------------------------------------------------------------
Reason KCPPacketSender::processFilterPacket(Channel* pChannel, Packet * pPacket, int userarg)
{
	if (pChannel->condemn() == Channel::FLAG_CONDEMN_AND_DESTROY)
	{
		return REASON_CHANNEL_CONDEMN;
	}

	if (userarg > 0)
	{
		//DEBUG_MSG(fmt::format("KCPPacketSender::processFilterPacket: kcp_sent={}, kcp={:p}, channel={:p}, this={:p}\n", 
		//	pPacket->length(), (void*)pChannel->pKCP(), (void*)pChannel, (void*)this));

		pChannel->addKcpUpdate();


				if (ikcp_waitsnd(pChannel->pKCP()) > (int)(pChannel->pKCP()->snd_wnd * 2)/* if the send queue exceeds the send window by 2 times, the resource is insufficient.*/ || 
			ikcp_send(pChannel->pKCP(), (const char*)pPacket->data(), pPacket->length()) < 0)
		{
			ERROR_MSG(fmt::format("KCPPacketSender::ikcp_send: send error! currPacketSize={}, ikcp_waitsnd={}, snd_wndsize={}\n", 
				pPacket->length(), ikcp_waitsnd(pChannel->pKCP()), pChannel->pKCP()->snd_wnd));

			return REASON_RESOURCE_UNAVAILABLE;
		}

		pPacket->sentSize += pPacket->length();
	}
	else
	{
		EndPoint* pEndpoint = pChannel->pEndPoint();
		int retlen = pEndpoint->sendto((void*)(pPacket->data()), pPacket->length());
		bool sentCompleted = (retlen == (int)pPacket->length());

		if (retlen > 0)
		{
			pPacket->sentSize += retlen;
			//DEBUG_MSG(fmt::format("KCPPacketSender::processFilterPacket: sent={}, sentTotalSize={}.\n", retlen, pPacket->sentSize));
		}

		pChannel->onPacketSent(retlen, sentCompleted);

		if (!sentCompleted)
		{
			// If you only sent a portion of the data, the error is sent.
			return checkSocketErrors(pEndpoint);
		}
	}

	return REASON_SUCCESS;
}

//-------------------------------------------------------------------------------------
void KCPPacketSender::onSent(Packet* pPacket)
{
	RECLAIM_PACKET(pPacket->isTCPPacket(), pPacket);
}

//-------------------------------------------------------------------------------------
int KCPPacketSender::kcp_output(const char *buf, int len, ikcpcb *kcp, Channel* pChannel)
{
	//OURO_ASSERT(kcp == pChannel->pKCP());

	EndPoint* pEndpoint = pChannel->pEndPoint();
	int retlen = pEndpoint->sendto((void*)buf, len);

	bool sentCompleted = retlen == len;
	pChannel->onPacketSent(retlen, sentCompleted);

	//DEBUG_MSG(fmt::format("KCPPacketSender::kcp_output: kcp={:p}, pChannel={:p} sent={}\n", (void*)kcp, (void*)pChannel, len));
	return sentCompleted ? 0 : -1;
}

//-------------------------------------------------------------------------------------
}
}

