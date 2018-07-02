// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#include "helper/profile.h"
#include "encryption_filter.h"
#include "helper/debug_helper.h"
#include "network/tcp_packet.h"
#include "network/udp_packet.h"
#include "network/channel.h"
#include "network/network_interface.h"
#include "network/packet_receiver.h"
#include "network/packet_sender.h"

namespace Ouroboros {
namespace Network
{

//-------------------------------------------------------------------------------------
BlowfishFilter::BlowfishFilter(const Key & key):
OUROBlowfish(key),
pPacket_(NULL),
packetLen_(0),
padSize_(0)
{
}

//-------------------------------------------------------------------------------------
BlowfishFilter::BlowfishFilter():
OUROBlowfish(),
pPacket_(NULL),
packetLen_(0),
padSize_(0)
{
}

//-------------------------------------------------------------------------------------
BlowfishFilter::~BlowfishFilter()
{
	if(pPacket_)
	{
		RECLAIM_PACKET(pPacket_->isTCPPacket(), pPacket_);
		pPacket_ = NULL;
	}
}

//-------------------------------------------------------------------------------------
Reason BlowfishFilter::send(Channel * pChannel, PacketSender& sender, Packet * pPacket, int userarg)
{
	if(!pPacket->encrypted())
	{
		AUTO_SCOPED_PROFILE("encryptSend")

		if (!isGood_)
		{
			WARNING_MSG(fmt::format("BlowfishFilter::send: "
				"Dropping packet to {} due to invalid filter\n",
				pChannel->addr().c_str()));

			return REASON_GENERAL_NETWORK;
		}

		Packet * pOutPacket = NULL;
		MALLOC_PACKET(pOutPacket, pPacket->isTCPPacket());

		PacketLength oldlen = (PacketLength)pPacket->length();
		pOutPacket->wpos(PACKET_LENGTH_SIZE + 1);
		encrypt(pPacket, pOutPacket);

		PacketLength packetLen = (PacketLength)(pPacket->length() + 1);
		uint8 padSize = (uint8)(pPacket->length() - oldlen);
		size_t oldwpos =  pOutPacket->wpos();
		pOutPacket->wpos(0);

		(*pOutPacket) << packetLen;
		(*pOutPacket) << padSize;

		pOutPacket->wpos((int)oldwpos);
		pPacket->swap(*(static_cast<Ouroboros::MemoryStream*>(pOutPacket)));
		RECLAIM_PACKET(pPacket->isTCPPacket(), pOutPacket);

		if (Network::g_trace_packet > 0 && Network::g_trace_encrypted_packet)
		{
			if (Network::g_trace_packet_use_logfile)
				DebugHelper::getSingleton().changeLogger("packetlogs");

			DEBUG_MSG(fmt::format("<==== BlowfishFilter::send: encryptedLen={}, padSize={}\n",
				packetLen, (int)padSize));

			switch (Network::g_trace_packet)
			{
			case 1:
				pPacket->hexlike();
				break;
			case 2:
				pPacket->textlike();
				break;
			default:
				pPacket->print_storage();
				break;
			};

			if (Network::g_trace_packet_use_logfile)
				DebugHelper::getSingleton().changeLogger(COMPONENT_NAME_EX(g_componentType));
		}
	}

	return sender.processFilterPacket(pChannel, pPacket, userarg);
}

//-------------------------------------------------------------------------------------
Reason BlowfishFilter::recv(Channel * pChannel, PacketReceiver & receiver, Packet * pPacket)
{
	while(pPacket || pPacket_)
	{
		AUTO_SCOPED_PROFILE("encryptRecv")

		if (!isGood_)
		{
			WARNING_MSG(fmt::format("BlowfishFilter::recv: "
				"Dropping packet to {} due to invalid filter\n",
				pChannel->addr().c_str()));

			return REASON_GENERAL_NETWORK;
		}

		if(pPacket_)
		{
			if(pPacket)
			{
				pPacket_->append(pPacket->data() + pPacket->rpos(), pPacket->length());
				RECLAIM_PACKET(pPacket->isTCPPacket(), pPacket);
			}

			pPacket = pPacket_;
		}

		if(packetLen_ <= 0)
		{
			// If a minimum packet is satisfied, it is attempted to unwrap, otherwise the packet is cached and merged with the next packet and then unwrapped
			if(pPacket->length() >= (PACKET_LENGTH_SIZE + 1 + BLOCK_SIZE))
			{
				(*pPacket) >> packetLen_;
				(*pPacket) >> padSize_;

				packetLen_ -= 1;

				// If the package is complete, the process below will decrypt, if there is excess content it needs to be trimmed out and merged with the next package
				if(pPacket->length() > packetLen_)
				{
					MALLOC_PACKET(pPacket_, pPacket->isTCPPacket());
					int currLen = pPacket->rpos() + packetLen_;
					pPacket_->append(pPacket->data() + currLen, pPacket->wpos() - currLen);
					pPacket->wpos(currLen);
				}
				else if(pPacket->length() == packetLen_)
				{
					if(pPacket_ != NULL && pPacket_ == pPacket)
						pPacket_ = NULL;
				}
				else
				{
					if(pPacket_ == NULL)
						pPacket_ = pPacket;

					return receiver.processFilteredPacket(pChannel, NULL);
				}
			}
			else
			{
				if(pPacket_ == NULL)
					pPacket_ = pPacket;

				return receiver.processFilteredPacket(pChannel, NULL);
			}
		}
		else
		{
			// If the last time there was an unpacking but the package is not complete then continue processing
			// If the package is complete, the process below will decrypt, if there is excess content it needs to be trimmed out and merged with the next package
			if(pPacket->length() > packetLen_)
			{
				MALLOC_PACKET(pPacket_, pPacket->isTCPPacket());
				int currLen = pPacket->rpos() + packetLen_;
				pPacket_->append(pPacket->data() + currLen, pPacket->wpos() - currLen);
				pPacket->wpos(currLen);
			}
			else if(pPacket->length() == packetLen_)
			{
				if(pPacket_ != NULL && pPacket_ == pPacket)
					pPacket_ = NULL;
			}
			else
			{
				if(pPacket_ == NULL)
					pPacket_ = pPacket;

				return receiver.processFilteredPacket(pChannel, NULL);
			}
		}

		if(Network::g_trace_packet > 0 && Network::g_trace_encrypted_packet)
		{
			if(Network::g_trace_packet_use_logfile)
				DebugHelper::getSingleton().changeLogger("packetlogs");

			DEBUG_MSG(fmt::format("====> BlowfishFilter::recv: encryptedLen={}, padSize={}\n",
				(packetLen_ + 1), (int)padSize_));

			switch(Network::g_trace_packet)
			{
			case 1:
				pPacket->hexlike();
				break;
			case 2:
				pPacket->textlike();
				break;
			default:
				pPacket->print_storage();
				break;
			};

			if(Network::g_trace_packet_use_logfile)
				DebugHelper::getSingleton().changeLogger(COMPONENT_NAME_EX(g_componentType));
		}

		decrypt(pPacket, pPacket);

		// The above process can guarantee that there will be no extra packages after wpos
		// If there is extra packet data will be put in pPacket_
		pPacket->wpos((int)(pPacket->wpos() - padSize_));

		packetLen_ = 0;
		padSize_ = 0;

		Reason ret = receiver.processFilteredPacket(pChannel, pPacket);
		if(ret != REASON_SUCCESS)
		{
			if(pPacket_)
			{
				RECLAIM_PACKET(pPacket_->isTCPPacket(), pPacket);
				pPacket_ = NULL;
			}

			return ret;
		}

		pPacket = NULL;
	}

	return REASON_SUCCESS;
}

//-------------------------------------------------------------------------------------
void BlowfishFilter::encrypt(Packet * pInPacket, Packet * pOutPacket)
{
	// BlowFish Can only encrypt and decrypt 8-byte data at a time
	// Less than 8 bytes are filled with 0
	uint8 padSize = 0;

	if (pInPacket->length() % BLOCK_SIZE != 0)
	{
		// Get insufficient size
		padSize = BLOCK_SIZE - (pInPacket->length() % BLOCK_SIZE);

		// Fill so many pPacket
		pInPacket->data_resize(pInPacket->size() + padSize);

		// Fill 0
		memset(pInPacket->data() + pInPacket->wpos(), 0, padSize);

		pInPacket->wpos((int)(pInPacket->wpos() + padSize));
	}

	if(pInPacket != pOutPacket)
	{
		pOutPacket->data_resize(pInPacket->size() + pOutPacket->wpos());
		int size = OUROBlowfish::encrypt(pInPacket->data(), pOutPacket->data() + pOutPacket->wpos(), (int)pInPacket->wpos());
		pOutPacket->wpos((int)(size + pOutPacket->wpos()));
	}
	else
	{
		if(pInPacket->isTCPPacket())
			pOutPacket = TCPPacket::createPoolObject();
		else
			pOutPacket = UDPPacket::createPoolObject();

		pOutPacket->data_resize(pInPacket->size() + 1);

		int size = OUROBlowfish::encrypt(pInPacket->data(), pOutPacket->data() + pOutPacket->wpos(), (int)pInPacket->wpos());
		pOutPacket->wpos(size);

		pInPacket->swap(*(static_cast<Ouroboros::MemoryStream*>(pOutPacket)));
		RECLAIM_PACKET(pInPacket->isTCPPacket(), pOutPacket);
	}

	pInPacket->encrypted(true);
}

//-------------------------------------------------------------------------------------
void BlowfishFilter::decrypt(Packet * pInPacket, Packet * pOutPacket)
{
	if(pInPacket != pOutPacket)
	{
		pOutPacket->data_resize(pInPacket->size());

		int size = OUROBlowfish::decrypt(pInPacket->data() + pInPacket->rpos(),
			pOutPacket->data() + pOutPacket->rpos(),
			(int)(pInPacket->wpos() - pInPacket->rpos()));

		pOutPacket->wpos((int)(size + pOutPacket->wpos()));
	}
	else
	{
		OUROBlowfish::decrypt(pInPacket->data() + pInPacket->rpos(), pInPacket->data(),
			(int)(pInPacket->wpos() - pInPacket->rpos()));

		pInPacket->wpos((int)(pInPacket->wpos() - pInPacket->rpos()));
		pInPacket->rpos(0);
	}
}

//-------------------------------------------------------------------------------------

}
}
