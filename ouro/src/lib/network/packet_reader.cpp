// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#include "packet_reader.h"
#include "network/channel.h"
#include "network/message_handler.h"
#include "network/network_stats.h"

namespace Ouroboros {
namespace Network
{


//-------------------------------------------------------------------------------------
PacketReader::PacketReader(Channel* pChannel):
	pFragmentDatas_(NULL),
	pFragmentDatasWpos_(0),
	pFragmentDatasRemain_(0),
	fragmentDatasFlag_(FRAGMENT_DATA_UNKNOW),
	pFragmentStream_(NULL),
	currMsgID_(0),
	currMsgLen_(0),
	pChannel_(pChannel)
{
}

//-------------------------------------------------------------------------------------
PacketReader::~PacketReader()
{
	reset();
	pChannel_ = NULL;
}

//-------------------------------------------------------------------------------------
void PacketReader::reset()
{
	fragmentDatasFlag_ = FRAGMENT_DATA_UNKNOW;
	pFragmentDatasWpos_ = 0;
	pFragmentDatasRemain_ = 0;
	currMsgID_ = 0;
	currMsgLen_ = 0;

	SAFE_RELEASE_ARRAY(pFragmentDatas_);
	MemoryStream::reclaimPoolObject(pFragmentStream_);
	pFragmentStream_ = NULL;
}

//-------------------------------------------------------------------------------------
void PacketReader::processMessages(Ouroboros::Network::MessageHandlers* pMsgHandlers, Packet* pPacket)
{
	while(pPacket->length() > 0 || pFragmentStream_ != NULL)
	{
		if(fragmentDatasFlag_ == FRAGMENT_DATA_UNKNOW)
		{
			// If there is no ID information, get the ID first
			if(currMsgID_ == 0)
			{
				if(NETWORK_MESSAGE_ID_SIZE > 1 && pPacket->length() < NETWORK_MESSAGE_ID_SIZE)
				{
					writeFragmentMessage(FRAGMENT_DATA_MESSAGE_ID, pPacket, NETWORK_MESSAGE_ID_SIZE);
					break;
				}

				(*pPacket) >> currMsgID_;
				pPacket->messageID(currMsgID_);
			}

			Network::MessageHandler* pMsgHandler = pMsgHandlers->find(currMsgID_);

			if(pMsgHandler == NULL)
			{
				MemoryStream* pPacket1 = pFragmentStream_ != NULL ? pFragmentStream_ : pPacket;
				TRACE_MESSAGE_PACKET(true, pPacket1, pMsgHandler, pPacket1->length(), pChannel_->c_str(), false);

				// When used as a debugger
				uint32 rpos = pPacket1->rpos();
				pPacket1->rpos(0);
				TRACE_MESSAGE_PACKET(true, pPacket1, pMsgHandler, pPacket1->length(), pChannel_->c_str(), false);
				pPacket1->rpos(rpos);

				ERROR_MSG(fmt::format("PacketReader::processMessages: not found msgID={}, msglen={}, from {}.\n",
					currMsgID_, pPacket1->length(), pChannel_->c_str()));

				currMsgID_ = 0;
				currMsgLen_ = 0;
				pChannel_->condemn();
				break;
			}

			// If there is no data available then exit to wait for the next packet.
			// May be a parameterless packet
			//if(pPacket->opsize() == 0)
			//	break;

			// If length information is not obtained, wait for length information
			if(currMsgLen_ == 0)
			{
				// Length data is analyzed from the stream if the length information is variable or if the Always include length information option is configured
				if(pMsgHandler->msgLen == NETWORK_VARIABLE_MESSAGE)
				{
					// If the length information is incomplete, wait for the next packet processing
					if(pPacket->length() < NETWORK_MESSAGE_LENGTH_SIZE)
					{
						writeFragmentMessage(FRAGMENT_DATA_MESSAGE_LENGTH, pPacket, NETWORK_MESSAGE_LENGTH_SIZE);
						break;
					}
					else
					{
						// Here is the length information
						Network::MessageLength currlen;
						(*pPacket) >> currlen;
						currMsgLen_ = currlen;

						NetworkStats::getSingleton().trackMessage(NetworkStats::RECV, *pMsgHandler,
							currMsgLen_ + NETWORK_MESSAGE_ID_SIZE + NETWORK_MESSAGE_LENGTH_SIZE);

						// If the length is full and the extension length is used, we also need to wait for the extension length information
						if(currMsgLen_ == NETWORK_MESSAGE_MAX_SIZE)
						{
							if(pPacket->length() < NETWORK_MESSAGE_LENGTH1_SIZE)
							{
								// If the length information is incomplete, wait for the next packet processing
								writeFragmentMessage(FRAGMENT_DATA_MESSAGE_LENGTH1, pPacket, NETWORK_MESSAGE_LENGTH1_SIZE);
								break;
							}
							else
							{
								// Here is the extended length information
								(*pPacket) >> currMsgLen_;

								NetworkStats::getSingleton().trackMessage(NetworkStats::RECV, *pMsgHandler,
									currMsgLen_ + NETWORK_MESSAGE_ID_SIZE + NETWORK_MESSAGE_LENGTH1_SIZE);
							}
						}
					}
				}
				else
				{
					currMsgLen_ = pMsgHandler->msgLen;

					NetworkStats::getSingleton().trackMessage(NetworkStats::RECV, *pMsgHandler,
						currMsgLen_ + NETWORK_MESSAGE_LENGTH_SIZE);
				}
			}

			if(this->pChannel_->isExternal() &&
				g_componentType != BOTS_TYPE &&
				g_componentType != CLIENT_TYPE &&
				currMsgLen_ > NETWORK_MESSAGE_MAX_SIZE)
			{
				MemoryStream* pPacket1 = pFragmentStream_ != NULL ? pFragmentStream_ : pPacket;
				TRACE_MESSAGE_PACKET(true, pPacket1, pMsgHandler, pPacket1->length(), pChannel_->c_str(), false);

				// When used as a debugger
				uint32 rpos = pPacket1->rpos();
				pPacket1->rpos(0);
				TRACE_MESSAGE_PACKET(true, pPacket1, pMsgHandler, pPacket1->length(), pChannel_->c_str(), false);
				pPacket1->rpos(rpos);

				WARNING_MSG(fmt::format("PacketReader::processMessages({0}): msglen exceeds the limit! msgID={1}, msglen=({2}:{3}), maxlen={5}, from {4}.\n",
					pMsgHandler->name.c_str(), currMsgID_, currMsgLen_, pPacket1->length(), pChannel_->c_str(), NETWORK_MESSAGE_MAX_SIZE));

				currMsgLen_ = 0;
				pChannel_->condemn();
				break;
			}

			if(pFragmentStream_ != NULL)
			{
				TRACE_MESSAGE_PACKET(true, pFragmentStream_, pMsgHandler, currMsgLen_, pChannel_->c_str(), false);
				pMsgHandler->handle(pChannel_, *pFragmentStream_);
				MemoryStream::reclaimPoolObject(pFragmentStream_);
				pFragmentStream_ = NULL;
			}
			else
			{
				if(pPacket->length() < currMsgLen_)
				{
					writeFragmentMessage(FRAGMENT_DATA_MESSAGE_BODY, pPacket, currMsgLen_);
					break;
				}

				// Set the valid read bit temporarily to prevent overflow in the interface
				size_t wpos = pPacket->wpos();
				// size_t rpos = pPacket->rpos();
				size_t frpos = pPacket->rpos() + currMsgLen_;
				pPacket->wpos(frpos);

				TRACE_MESSAGE_PACKET(true, pPacket, pMsgHandler, currMsgLen_, pChannel_->c_str(), true);
				pMsgHandler->handle(pChannel_, *pPacket);

				// If the handler does not process the data output a warning
				if(currMsgLen_ > 0)
				{
					if(frpos != pPacket->rpos())
					{
						WARNING_MSG(fmt::format("PacketReader::processMessages({}): rpos({}) invalid, expect={}. msgID={}, msglen={}.\n",
							pMsgHandler->name.c_str(), pPacket->rpos(), frpos, currMsgID_, currMsgLen_));

						pPacket->rpos(frpos);
					}
				}

				pPacket->wpos(wpos);
			}

			currMsgID_ = 0;
			currMsgLen_ = 0;
		}
		else
		{
			mergeFragmentMessage(pPacket);
		}
	}
}

//-------------------------------------------------------------------------------------
void PacketReader::writeFragmentMessage(FragmentDataTypes fragmentDatasFlag, Packet* pPacket, uint32 datasize)
{
	OURO_ASSERT(pFragmentDatas_ == NULL);

	size_t opsize = pPacket->length();
	pFragmentDatasRemain_ = datasize - opsize;
	pFragmentDatas_ = new uint8[opsize + pFragmentDatasRemain_ + 1];

	fragmentDatasFlag_ = fragmentDatasFlag;
	pFragmentDatasWpos_ = opsize;

	if(pPacket->length() > 0)
	{
		memcpy(pFragmentDatas_, pPacket->data() + pPacket->rpos(), opsize);
		pPacket->done();
	}

	//DEBUG_MSG(fmt::format("PacketReader::writeFragmentMessage({}): channel[{:p}], fragmentDatasFlag={}, remainsize={}, currMsgID={}, currMsgLen={}.\n",
	//	pChannel_->c_str(), (void*)pChannel_, fragmentDatasFlag, pFragmentDatasRemain_, currMsgID_, currMsgLen_));
}

//-------------------------------------------------------------------------------------
void PacketReader::mergeFragmentMessage(Packet* pPacket)
{
	size_t opsize = pPacket->length();
	if(opsize == 0)
		return;

	if(pPacket->length() >= pFragmentDatasRemain_)
	{
		memcpy(pFragmentDatas_ + pFragmentDatasWpos_, pPacket->data() + pPacket->rpos(), pFragmentDatasRemain_);
		pPacket->rpos(pPacket->rpos() + pFragmentDatasRemain_);

		OURO_ASSERT(pFragmentStream_ == NULL);

		switch(fragmentDatasFlag_)
		{
		case FRAGMENT_DATA_MESSAGE_ID:			// Incomplete message ID information
			memcpy(&currMsgID_, pFragmentDatas_, NETWORK_MESSAGE_ID_SIZE);
			break;

		case FRAGMENT_DATA_MESSAGE_LENGTH:		// Incomplete message length information
			memcpy(&currMsgLen_, pFragmentDatas_, NETWORK_MESSAGE_LENGTH_SIZE);
			break;

		case FRAGMENT_DATA_MESSAGE_LENGTH1:		// Incomplete message length information
			memcpy(&currMsgLen_, pFragmentDatas_, NETWORK_MESSAGE_LENGTH1_SIZE);
			break;

		case FRAGMENT_DATA_MESSAGE_BODY:		// Incomplete message content
			pFragmentStream_ = MemoryStream::createPoolObject();
			pFragmentStream_->append(pFragmentDatas_, currMsgLen_);
			break;

		default:
			break;
		};

		//DEBUG_MSG(fmt::format("PacketReader::mergeFragmentMessage({}): channel[{:p}], fragmentDatasFlag={}, currMsgID={}, currMsgLen={}, completed!\n",
		//	pChannel_->c_str(), (void*)pChannel_, fragmentDatasFlag_, currMsgID_, currMsgLen_));

		fragmentDatasFlag_ = FRAGMENT_DATA_UNKNOW;
		pFragmentDatasRemain_ = 0;
		SAFE_RELEASE_ARRAY(pFragmentDatas_);
	}
	else
	{
		memcpy(pFragmentDatas_ + pFragmentDatasWpos_, pPacket->data(), opsize);
		pFragmentDatasRemain_ -= opsize;
		pFragmentDatasWpos_ += opsize;
		pPacket->rpos(pPacket->rpos() + opsize);

		//DEBUG_MSG(fmt::format("PacketReader::mergeFragmentMessage({}): channel[{:p}], fragmentDatasFlag={}, remainsize={}, currMsgID={}, currMsgLen={}.\n",
		//	pChannel_->c_str(), (void*)pChannel_, fragmentDatasFlag_, pFragmentDatasRemain_, currMsgID_, currMsgLen_));
	}
}

//-------------------------------------------------------------------------------------
}
}
