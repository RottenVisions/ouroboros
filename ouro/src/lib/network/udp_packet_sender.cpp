// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com


#include "udp_packet_sender.h"
#ifndef CODE_INLINE
#include "udp_packet_sender.inl"
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
static ObjectPool<UDPPacketSender> _g_objPool("UDPPacketSender");
ObjectPool<UDPPacketSender>& UDPPacketSender::ObjPool()
{
	return _g_objPool;
}

//-------------------------------------------------------------------------------------
UDPPacketSender* UDPPacketSender::createPoolObject(const std::string& logPoint)
{
	return _g_objPool.createObject(logPoint);
}

//-------------------------------------------------------------------------------------
void UDPPacketSender::reclaimPoolObject(UDPPacketSender* obj)
{
	_g_objPool.reclaimObject(obj);
}

//-------------------------------------------------------------------------------------
void UDPPacketSender::onReclaimObject()
{
	sendfailCount_ = 0;
}

//-------------------------------------------------------------------------------------
void UDPPacketSender::destroyObjPool()
{
	DEBUG_MSG(fmt::format("UDPPacketSender::destroyObjPool(): size {}.\n", 
		_g_objPool.size()));

	_g_objPool.destroy();
}

//-------------------------------------------------------------------------------------
UDPPacketSender::SmartPoolObjectPtr UDPPacketSender::createSmartPoolObj(const std::string& logPoint)
{
	return SmartPoolObjectPtr(new SmartPoolObject<UDPPacketSender>(ObjPool().createObject(logPoint), _g_objPool));
}

//-------------------------------------------------------------------------------------
UDPPacketSender::UDPPacketSender(EndPoint & endpoint,
	   NetworkInterface & networkInterface	) :
	PacketSender(endpoint, networkInterface),
	sendfailCount_(0)
{
}

//-------------------------------------------------------------------------------------
UDPPacketSender::~UDPPacketSender()
{
	//DEBUG_MSG("UDPPacketSender::~UDPPacketSender()\n");
}

//-------------------------------------------------------------------------------------
void UDPPacketSender::onGetError(Channel* pChannel, const std::string& err)
{
	pChannel->condemn(err);
	
	// There is no need to destroy it immediately, which may cause the bufferedReceives_ internal traversal iterator to break.
	// Hand it to TCPPacketReceiver
	//pChannel->networkInterface().deregisterChannel(pChannel);
	//pChannel->destroy();
}

//-------------------------------------------------------------------------------------
void UDPPacketSender::onSent(Packet* pPacket)
{
	RECLAIM_PACKET(pPacket->isTCPPacket(), pPacket);
}

//-------------------------------------------------------------------------------------
bool UDPPacketSender::processSend(Channel* pChannel, int userarg)
{
	OURO_ASSERT(pChannel != NULL);

	if (pChannel->condemn() == Channel::FLAG_CONDEMN_AND_DESTROY)
	{
		return false;
	}

	Channel::Bundles& bundles = pChannel->bundles();
	Reason reason = REASON_SUCCESS;

	Channel::Bundles::iterator iter = bundles.begin();
	for (; iter != bundles.end(); ++iter)
	{
		Bundle::Packets& pakcets = (*iter)->packets();
		Bundle::Packets::iterator iter1 = pakcets.begin();
		for (; iter1 != pakcets.end(); ++iter1)
		{
			Packet* pPacket = (*iter1);
			reason = processPacket(pChannel, pPacket, userarg);
			if (reason != REASON_SUCCESS)
				break;
			else
				onSent(pPacket);
		}

		if (reason == REASON_SUCCESS)
		{
			pakcets.clear();
			Network::Bundle::reclaimPoolObject((*iter));
			sendfailCount_ = 0;
		}
		else
		{
			pakcets.erase(pakcets.begin(), iter1);
			bundles.erase(bundles.begin(), iter);

			if (reason == REASON_RESOURCE_UNAVAILABLE)
			{
				/* The output here may cause debugHelper to kill the lock
					WARNING_MSG(fmt::format("UDPPacketSender::processSend: "
						"Transmit queue full, waiting for space(ouroboros.xml->channelCommon->writeBufferSize->{})...\n",
						(pChannel->isInternal() ? "internal" : "external")));
				*/

				// Notice more than 10 consecutive times
				if (++sendfailCount_ >= 10 && pChannel->isExternal())
				{
					onGetError(pChannel, "UDPPacketSender::processSend: sendfailCount >= 10");

					this->dispatcher().errorReporter().reportException(reason, pEndpoint_->addr(),
						fmt::format("UDPPacketSender::processSend(external, sendfailCount({}) >= 10)", (int)sendfailCount_).c_str());
				}
				else
				{
					this->dispatcher().errorReporter().reportException(reason, pEndpoint_->addr(),
						fmt::format("UDPPacketSender::processSend(internal, {})", (int)sendfailCount_).c_str());
				}
			}
			else
			{
				if (pChannel->isExternal())
				{
#if OURO_PLATFORM == PLATFORM_UNIX
					this->dispatcher().errorReporter().reportException(reason, pEndpoint_->addr(), "UDPPacketSender::processSend(external)",
						fmt::format(", errno: {}", errno).c_str());
#else
					this->dispatcher().errorReporter().reportException(reason, pEndpoint_->addr(), "UDPPacketSender::processSend(external)",
						fmt::format(", errno: {}", WSAGetLastError()).c_str());
#endif
			}
				else
				{
#if OURO_PLATFORM == PLATFORM_UNIX
					this->dispatcher().errorReporter().reportException(reason, pEndpoint_->addr(), "UDPPacketSender::processSend(internal)",
						fmt::format(", errno: {}, {}", errno, pChannel->c_str()).c_str());
#else
					this->dispatcher().errorReporter().reportException(reason, pEndpoint_->addr(), "UDPPacketSender::processSend(internal)",
						fmt::format(", errno: {}, {}", WSAGetLastError(), pChannel->c_str()).c_str());
#endif
				}

				onGetError(pChannel, fmt::format("UDPPacketSender::processSend: errno={}", ouro_lasterror()));
			}

			return false;
		}
	}

	bundles.clear();

	return true;
}

//-------------------------------------------------------------------------------------
Reason UDPPacketSender::processFilterPacket(Channel* pChannel, Packet * pPacket, int userarg)
{
	if (pChannel->condemn() == Channel::FLAG_CONDEMN_AND_DESTROY)
	{
		return REASON_CHANNEL_CONDEMN;
	}

	// sendto is not implemented
	OURO_ASSERT(false);

	return REASON_SUCCESS;
}

//-------------------------------------------------------------------------------------
}
}

