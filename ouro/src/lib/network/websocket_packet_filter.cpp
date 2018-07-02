// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com


#include "websocket_packet_filter.h"
#include "websocket_protocol.h"

#include "network/bundle.h"
#include "network/channel.h"
#include "network/tcp_packet.h"
#include "network/network_interface.h"
#include "network/packet_receiver.h"

namespace Ouroboros {
namespace Network
{

//-------------------------------------------------------------------------------------
WebSocketPacketFilter::WebSocketPacketFilter(Channel* pChannel):
	pFragmentDatasRemain_(0),
	fragmentDatasFlag_(FRAGMENT_MESSAGE_HREAD),
	msg_opcode_(0),
	msg_fin_(0),
	msg_masked_(0),
	msg_mask_(0),
	msg_length_field_(0),
	msg_payload_length_(0),
	msg_frameType_(websocket::WebSocketProtocol::ERROR_FRAME),
	pChannel_(pChannel),
	pTCPPacket_(NULL)
{
}

//-------------------------------------------------------------------------------------
WebSocketPacketFilter::~WebSocketPacketFilter()
{
	reset();
}

//-------------------------------------------------------------------------------------
void WebSocketPacketFilter::reset()
{
	msg_opcode_ = 0;
	msg_fin_ = 0;
	msg_masked_ = 0;
	msg_mask_ = 0;
	msg_length_field_ = 0;
	msg_payload_length_ = 0;
	pFragmentDatasRemain_ = 0;
	fragmentDatasFlag_ = FRAGMENT_MESSAGE_HREAD;

	TCPPacket::reclaimPoolObject(pTCPPacket_);
	pTCPPacket_ = NULL;
}

//-------------------------------------------------------------------------------------
Reason WebSocketPacketFilter::send(Channel * pChannel, PacketSender& sender, Packet * pPacket, int userarg)
{
	if(pPacket->encrypted())
		return PacketFilter::send(pChannel, sender, pPacket, userarg);

	Bundle* pBundle = pPacket->pBundle();
	TCPPacket* pRetTCPPacket = TCPPacket::createPoolObject();
	websocket::WebSocketProtocol::FrameType frameType = websocket::WebSocketProtocol::BINARY_FRAME;

	if (pBundle)
	{
		Bundle::Packets& packs = pBundle->packets();

		if (packs.size() > 1)
		{
			bool isEnd = packs.back() == pPacket;
			bool isBegin = packs.front() == pPacket;

			if (!isEnd && !isBegin)
			{
				frameType = websocket::WebSocketProtocol::NEXT_FRAME;
			}
			else
			{
				if (!isEnd)
				{
					frameType = websocket::WebSocketProtocol::INCOMPLETE_BINARY_FRAME;
				}
				else
				{
					frameType = websocket::WebSocketProtocol::END_FRAME;
				}
			}
		}
	}

	websocket::WebSocketProtocol::makeFrame(frameType, pPacket, pRetTCPPacket);

	int space = pPacket->length() - pRetTCPPacket->space();
	if(space > 0)
	{
		WARNING_MSG(fmt::format("WebSocketPacketFilter::send: no free space, buffer added:{}, total={}.\n",
			space, pRetTCPPacket->size()));

		pRetTCPPacket->data_resize(pRetTCPPacket->size() + space);
	}

	(*pRetTCPPacket).append(pPacket->data() + pPacket->rpos(), pPacket->length());
	pRetTCPPacket->swap(*(static_cast<Ouroboros::MemoryStream*>(pPacket)));
	TCPPacket::reclaimPoolObject(pRetTCPPacket);

	pPacket->encrypted(true);
	return PacketFilter::send(pChannel, sender, pPacket, userarg);
}

//-------------------------------------------------------------------------------------
Reason WebSocketPacketFilter::recv(Channel * pChannel, PacketReceiver & receiver, Packet * pPacket)
{
	while(pPacket->length() > 0)
	{
		if(fragmentDatasFlag_ == FRAGMENT_MESSAGE_HREAD)
		{
			if(pFragmentDatasRemain_ == 0)
			{
				OURO_ASSERT(pTCPPacket_ == NULL);

				size_t rpos = pPacket->rpos();

				reset();

				// If no cache has been created, try to parse the header directly. If the information is parsed successfully, continue to the next step.
				pFragmentDatasRemain_ = websocket::WebSocketProtocol::getFrame(pPacket, msg_opcode_, msg_fin_, msg_masked_,
					msg_mask_, msg_length_field_, msg_payload_length_, msg_frameType_);

				if(pFragmentDatasRemain_ > 0)
				{
					pPacket->rpos(rpos);
					pTCPPacket_ = TCPPacket::createPoolObject();
					pTCPPacket_->append(*(static_cast<MemoryStream*>(pPacket)));
					pPacket->done();
				}
				else
				{
					fragmentDatasFlag_ = FRAGMENT_MESSAGE_DATAS;
					pFragmentDatasRemain_ = (int32)msg_payload_length_;
				}
			}
			else
			{
				OURO_ASSERT(pTCPPacket_ != NULL);

				// If the length is greater than the remaining read length, you can start parsing
				// Otherwise, continue to cache the package memory
				if((int32)pPacket->length() >= pFragmentDatasRemain_)
				{
					size_t wpos = pPacket->wpos();
					size_t rpos = pPacket->rpos();

					pPacket->wpos(rpos + pFragmentDatasRemain_);

					// First add the required data to pTCPPacket_
					pTCPPacket_->append(*(static_cast<MemoryStream*>(pPacket)));

					// Restore the write location back
					pPacket->wpos(wpos);

					// Discard data that has already been read
					pPacket->read_skip(pFragmentDatasRemain_);

					size_t buffer_rpos = pTCPPacket_->rpos();
					pFragmentDatasRemain_ = websocket::WebSocketProtocol::getFrame(pTCPPacket_, msg_opcode_, msg_fin_, msg_masked_,
						msg_mask_, msg_length_field_, msg_payload_length_, msg_frameType_);

					// If it is still greater than 0, it means the need to continue receiving packets
					if(pFragmentDatasRemain_ > 0)
					{
						// Since we did not complete the analysis at a time, we retrace the data and try to resolve again.
						pTCPPacket_->rpos(buffer_rpos);

						// If the current package has data and is greater than or equal to the data we need, continue to the next cycle and immediately parse
						if ((int32)pPacket->length() >= pFragmentDatasRemain_)
							continue;
					}
					else
					{
						// The frame is parsed and the object is recycled
						TCPPacket::reclaimPoolObject(pTCPPacket_);
						pTCPPacket_ = NULL;

						// Is there data to carry? If not, do not enter data resolution
						if(msg_payload_length_ > 0)
						{
							fragmentDatasFlag_ = FRAGMENT_MESSAGE_DATAS;
							pFragmentDatasRemain_ = (int32)msg_payload_length_;
						}
					}
				}
				else
				{
					pTCPPacket_->append(*(static_cast<MemoryStream*>(pPacket)));
					pFragmentDatasRemain_ -= pPacket->length();

					pPacket->done();
				}
			}

			if(websocket::WebSocketProtocol::ERROR_FRAME == msg_frameType_)
			{
				ERROR_MSG(fmt::format("WebSocketPacketReader::recv: frame is error! addr={}!\n",
					pChannel_->c_str()));

				this->pChannel_->condemn();
				reset();

				TCPPacket::reclaimPoolObject(static_cast<TCPPacket*>(pPacket));
				return REASON_WEBSOCKET_ERROR;
			}
			else if(msg_frameType_ == websocket::WebSocketProtocol::TEXT_FRAME ||
					msg_frameType_ == websocket::WebSocketProtocol::INCOMPLETE_TEXT_FRAME ||
					msg_frameType_ == websocket::WebSocketProtocol::PING_FRAME ||
					msg_frameType_ == websocket::WebSocketProtocol::PONG_FRAME)
			{
				ERROR_MSG(fmt::format("WebSocketPacketReader::recv: Does not support FRAME_TYPE({})! addr={}!\n",
					(int)msg_frameType_, pChannel_->c_str()));

				this->pChannel_->condemn();
				reset();

				TCPPacket::reclaimPoolObject(static_cast<TCPPacket*>(pPacket));
				return REASON_WEBSOCKET_ERROR;
			}
			else if(msg_frameType_ == websocket::WebSocketProtocol::CLOSE_FRAME)
			{
				this->pChannel_->condemn();
				reset();

				TCPPacket::reclaimPoolObject(static_cast<TCPPacket*>(pPacket));
				return REASON_SUCCESS;
			}
			else if(msg_frameType_ == websocket::WebSocketProtocol::INCOMPLETE_FRAME)
			{
				// Continue to wait for subsequent content to arrive
			}
		}
		else
		{
			if (pFragmentDatasRemain_ <= 0)
			{
				ERROR_MSG(fmt::format("WebSocketPacketReader::recv: pFragmentDatasRemain_ <= 0! addr={}!\n",
					pChannel_->c_str()));

				this->pChannel_->condemn();
				reset();

				TCPPacket::reclaimPoolObject(static_cast<TCPPacket*>(pPacket));
				return REASON_WEBSOCKET_ERROR;
			}

			if(pTCPPacket_ == NULL)
				pTCPPacket_ = TCPPacket::createPoolObject();

			if(pFragmentDatasRemain_ <= (int32)pPacket->length())
			{
				pTCPPacket_->append(pPacket->data() + pPacket->rpos(), pFragmentDatasRemain_);
				pPacket->read_skip((size_t)pFragmentDatasRemain_);
				pFragmentDatasRemain_ = 0;
			}
			else
			{
				pTCPPacket_->append(*(static_cast<MemoryStream*>(pPacket)));
				pFragmentDatasRemain_ -= pPacket->length();
				pPacket->done();
			}

			if(!websocket::WebSocketProtocol::decodingDatas(pTCPPacket_, msg_masked_, msg_mask_))
			{
				ERROR_MSG(fmt::format("WebSocketPacketReader::recv: decoding-frame is error! addr={}!\n",
					pChannel_->c_str()));

				this->pChannel_->condemn();
				reset();

				TCPPacket::reclaimPoolObject(static_cast<TCPPacket*>(pPacket));
				return REASON_WEBSOCKET_ERROR;
			}

			Reason reason = PacketFilter::recv(pChannel, receiver, pTCPPacket_);

			// pTCPPacket_ does not need to be recycled here
			pTCPPacket_ = NULL;

			if(pFragmentDatasRemain_ == 0)
				reset();

			if(reason != REASON_SUCCESS)
				return reason;
		}
	}

	TCPPacket::reclaimPoolObject(static_cast<TCPPacket*>(pPacket));
	return REASON_SUCCESS;
}

//-------------------------------------------------------------------------------------
}
}
