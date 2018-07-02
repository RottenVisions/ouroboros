// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com


#include "tcp_packet_sender.h"
#ifndef CODE_INLINE
#include "tcp_packet_sender.inl"
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
static ObjectPool<TCPPacketSender> _g_objPool("TCPPacketSender");
ObjectPool<TCPPacketSender>& TCPPacketSender::ObjPool()
{
	return _g_objPool;
}

//-------------------------------------------------------------------------------------
TCPPacketSender* TCPPacketSender::createPoolObject()
{
	return _g_objPool.createObject();
}

//-------------------------------------------------------------------------------------
void TCPPacketSender::reclaimPoolObject(TCPPacketSender* obj)
{
	_g_objPool.reclaimObject(obj);
}

//-------------------------------------------------------------------------------------
void TCPPacketSender::onReclaimObject()
{
	sendfailCount_ = 0;
}

//-------------------------------------------------------------------------------------
void TCPPacketSender::destroyObjPool()
{
	DEBUG_MSG(fmt::format("TCPPacketSender::destroyObjPool(): size {}.\n",
		_g_objPool.size()));

	_g_objPool.destroy();
}

//-------------------------------------------------------------------------------------
TCPPacketSender::SmartPoolObjectPtr TCPPacketSender::createSmartPoolObj()
{
	return SmartPoolObjectPtr(new SmartPoolObject<TCPPacketSender>(ObjPool().createObject(), _g_objPool));
}

//-------------------------------------------------------------------------------------
TCPPacketSender::TCPPacketSender(EndPoint & endpoint,
	   NetworkInterface & networkInterface	) :
	PacketSender(endpoint, networkInterface),
	sendfailCount_(0)
{
}

//-------------------------------------------------------------------------------------
TCPPacketSender::~TCPPacketSender()
{
	//DEBUG_MSG("TCPPacketSender::~TCPPacketSender()\n");
}

//-------------------------------------------------------------------------------------
void TCPPacketSender::onGetError(Channel* pChannel)
{
	pChannel->condemn();

	// This does not need to be destroyed immediately, it may cause bufferedReceives_ to be destroyed by internal traversal iterators
	// Handle to TCPPacketReceiver
	//pChannel->networkInterface().deregisterChannel(pChannel);
	//pChannel->destroy();
}

//-------------------------------------------------------------------------------------
bool TCPPacketSender::processSend(Channel* pChannel, int userarg)
{
	bool noticed = pChannel == NULL;

	// If it is notified by poller, we need to find the channel by address
	if(noticed)
		pChannel = getChannel();

	OURO_ASSERT(pChannel != NULL);

	if(pChannel->isCondemn())
	{
		return false;
	}

	Channel::Bundles& bundles = pChannel->bundles();
	Reason reason = REASON_SUCCESS;

	Channel::Bundles::iterator iter = bundles.begin();
	for(; iter != bundles.end(); ++iter)
	{
		Bundle::Packets& pakcets = (*iter)->packets();
		Bundle::Packets::iterator iter1 = pakcets.begin();
		for (; iter1 != pakcets.end(); ++iter1)
		{
			reason = processPacket(pChannel, (*iter1), userarg);
			if(reason != REASON_SUCCESS)
				break;
			else
				RECLAIM_PACKET((*iter)->isTCPPacket(), (*iter1));
		}

		if(reason == REASON_SUCCESS)
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
				/* Output here may cause deadlock on debugHelper
					WARNING_MSG(fmt::format("TCPPacketSender::processSend: "
						"Transmit queue full, waiting for space(ouroboros.xml->channelCommon->writeBufferSize->{})...\n",
						(pChannel->isInternal() ? "internal" : "external")));
				*/

				// Errors occur more than 10 times in a row
				if (++sendfailCount_ >= 10 && pChannel->isExternal())
				{
					onGetError(pChannel);

					this->dispatcher().errorReporter().reportException(reason, pEndpoint_->addr(),
						fmt::format("TCPPacketSender::processSend(sendfailCount({}) >= 10)", (int)sendfailCount_).c_str());
				}
				else
				{
					this->dispatcher().errorReporter().reportException(reason, pEndpoint_->addr(),
						fmt::format("TCPPacketSender::processSend({})", (int)sendfailCount_).c_str());
				}
			}
			else
			{
#ifdef unix
				this->dispatcher().errorReporter().reportException(reason, pEndpoint_->addr(), "TCPPacketSender::processSend()",
					fmt::format(", errno: {}", errno).c_str());
#else
				this->dispatcher().errorReporter().reportException(reason, pEndpoint_->addr(), "TCPPacketSender::processSend()",
					fmt::format(", errno: {}", WSAGetLastError()).c_str());
#endif
				onGetError(pChannel);
			}

			return false;
		}
	}

	bundles.clear();

	if(noticed)
		pChannel->onSendCompleted();

	return true;
}

//-------------------------------------------------------------------------------------
Reason TCPPacketSender::processFilterPacket(Channel* pChannel, Packet * pPacket, int userarg)
{
	if(pChannel->isCondemn())
	{
		return REASON_CHANNEL_CONDEMN;
	}

	EndPoint* pEndpoint = pChannel->pEndPoint();
	int len = pEndpoint->send(pPacket->data() + pPacket->sentSize, pPacket->length() - pPacket->sentSize);

	if(len > 0)
	{
		pPacket->sentSize += len;
		// DEBUG_MSG(fmt::format("TCPPacketSender::processFilterPacket: sent={}, sentTotalSize={}.\n", len, pPacket->sentSize));
	}

	bool sentCompleted = pPacket->sentSize == pPacket->length();
	pChannel->onPacketSent(len, sentCompleted);

	if (sentCompleted)
	{
		return REASON_SUCCESS;
	}
	else
	{
		// If only a part of the data is sent, it is considered REASON_RESOURCE_UNAVAILABLE
		if (len > 0)
			return REASON_RESOURCE_UNAVAILABLE;
	}

	return checkSocketErrors(pEndpoint);
}

//-------------------------------------------------------------------------------------
}
}
