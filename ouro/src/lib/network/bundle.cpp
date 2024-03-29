// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com


#include "bundle.h"
#include "network/network_stats.h"
#include "network/network_interface.h"
#include "network/channel.h"
#include "helper/profile.h"
#include "network/packet_sender.h"

#ifndef CODE_INLINE
#include "bundle.inl"
#endif

#include "common/blowfish.h"


namespace Ouroboros { 
namespace Network
{

//-------------------------------------------------------------------------------------
static ObjectPool<Bundle> _g_objPool("Bundle");
ObjectPool<Bundle>& Bundle::ObjPool()
{
	return _g_objPool;
}

//-------------------------------------------------------------------------------------
Bundle* Bundle::createPoolObject(const std::string& logPoint)
{
	return _g_objPool.createObject(logPoint);
}

//-------------------------------------------------------------------------------------
void Bundle::reclaimPoolObject(Bundle* obj)
{
	_g_objPool.reclaimObject(obj);
}

//-------------------------------------------------------------------------------------
void Bundle::destroyObjPool()
{
	DEBUG_MSG(fmt::format("Bundle::destroyObjPool(): size {}.\n", 
		_g_objPool.size()));

	_g_objPool.destroy();
}

//-------------------------------------------------------------------------------------
size_t Bundle::getPoolObjectBytes()
{
	size_t bytes = sizeof(pCurrMsgHandler_) + sizeof(isTCPPacket_) + sizeof(pCurrPacket_) + sizeof(packetMaxSize_) +
		sizeof(currMsgLengthPos_) + sizeof(currMsgHandlerLength_) + sizeof(currMsgLength_) + 
		sizeof(currMsgPacketCount_) + sizeof(currMsgID_) + sizeof(numMessages_) + sizeof(pChannel_)
		+ (packets_.size() * sizeof(Packet*));

	return bytes;
}

//-------------------------------------------------------------------------------------
Bundle::SmartPoolObjectPtr Bundle::createSmartPoolObj(const std::string& logPoint)
{
	return SmartPoolObjectPtr(new SmartPoolObject<Bundle>(ObjPool().createObject(logPoint), _g_objPool));
}

//-------------------------------------------------------------------------------------
Bundle::Bundle(Channel * pChannel, ProtocolType pt):
	pChannel_(pChannel),
	numMessages_(0),
	pCurrPacket_(NULL),
	currMsgID_(0),
	currMsgPacketCount_(0),
	currMsgLength_(0),
	currMsgHandlerLength_(0),
	currMsgLengthPos_(0),
	packets_(),
	isTCPPacket_(pt == PROTOCOL_TCP),
	packetMaxSize_(0),
	pCurrMsgHandler_(NULL)
{
	_calcPacketMaxSize();
	 newPacket();
}

//-------------------------------------------------------------------------------------
Bundle::Bundle(const Bundle& bundle)
{
	copy(bundle);
}

//-------------------------------------------------------------------------------------
Bundle::~Bundle()
{
	clear(false);
}

//-------------------------------------------------------------------------------------
void Bundle::onReclaimObject()
{
	clear(true);
}

//-------------------------------------------------------------------------------------
void Bundle::copy(const Bundle& bundle)
{
	// these must be set in front
	// Otherwise creating a packet in the middle may be wrong
	isTCPPacket_ = bundle.isTCPPacket_;
	pChannel_ = bundle.pChannel_;
	pCurrMsgHandler_ = bundle.pCurrMsgHandler_;
	currMsgID_ = bundle.currMsgID_;

	Packets::const_iterator iter = bundle.packets_.begin();
	for (; iter != bundle.packets_.end(); ++iter)
	{
		newPacket();
		pCurrPacket_->append(*static_cast<MemoryStream*>((*iter)));
		packets_.push_back(pCurrPacket_);
	}

	pCurrPacket_ = NULL;
	if (bundle.pCurrPacket_)
	{
		newPacket();
		pCurrPacket_->append(*static_cast<MemoryStream*>(bundle.pCurrPacket_));
	}

	numMessages_ = bundle.numMessages_;
	currMsgPacketCount_ = bundle.currMsgPacketCount_;
	currMsgLength_ = bundle.currMsgLength_;
	currMsgHandlerLength_ = bundle.currMsgHandlerLength_;
	currMsgLengthPos_ = bundle.currMsgLengthPos_;
	_calcPacketMaxSize();
}

//-------------------------------------------------------------------------------------
void Bundle::_calcPacketMaxSize()
{
	// If you use openssl to encrypt communication, we guarantee that a package can be divided by Blowfish::BLOCK_SIZE.
	// So we don't need extra padding when encrypting a fully loaded package
	if(g_channelExternalEncryptType == 1)
	{
		packetMaxSize_ = isTCPPacket_ ? (int)(TCPPacket::maxBufferSize() - ENCRYPTTION_WASTAGE_SIZE) :
			(PACKET_MAX_SIZE_UDP - ENCRYPTTION_WASTAGE_SIZE);

		packetMaxSize_ -= packetMaxSize_ % Ouroboros::OUROBlowfish::BLOCK_SIZE;
	}
	else
	{
		packetMaxSize_ = isTCPPacket_ ? (int)TCPPacket::maxBufferSize() : PACKET_MAX_SIZE_UDP;
	}
}

//-------------------------------------------------------------------------------------
int32 Bundle::packetsLength(bool calccurr)
{
	int32 len = 0;

	Packets::iterator iter = packets_.begin();
	for (; iter != packets_.end(); ++iter)
	{
		len += (int)(*iter)->length();
	}

	if(calccurr && pCurrPacket_)
		len += (int)pCurrPacket_->length();

	return len;
}

//-------------------------------------------------------------------------------------
int32 Bundle::onPacketAppend(int32 addsize, bool inseparable)
{
	if(pCurrPacket_ == NULL)
	{
		newPacket();
	}

	int32 totalsize = (int32)pCurrPacket_->length();
	int32 fwpos = (int32)pCurrPacket_->wpos();

	if(inseparable)
		fwpos += addsize;

	// If the current package does not contain the data of this append, fill it into the new package.
	if(fwpos >= packetMaxSize_)
	{
		packets_.push_back(pCurrPacket_);
		currMsgPacketCount_++;
		newPacket();
		totalsize = 0;
	}

	int32 remainsize = packetMaxSize_ - totalsize;
	int32 taddsize = addsize;

	// If the current package free space is less than the bytes to be added, this time fill this package
	if(remainsize < addsize)
		taddsize = remainsize;
	
	currMsgLength_ += taddsize;
	return taddsize;
}

//-------------------------------------------------------------------------------------
Packet* Bundle::newPacket()
{
	MALLOC_PACKET(pCurrPacket_, isTCPPacket_);
	pCurrPacket_->pBundle(this);
	return pCurrPacket_;
}

//-------------------------------------------------------------------------------------
void Bundle::clear(bool isRecl)
{
	if(pCurrPacket_ != NULL)
	{
		packets_.push_back(pCurrPacket_);
		pCurrPacket_ = NULL;
	}

	Packets::iterator iter = packets_.begin();
	for (; iter != packets_.end(); ++iter)
	{
		if(!isRecl)
		{
			delete (*iter);
		}
		else
		{
			RECLAIM_PACKET(isTCPPacket_, (*iter));
		}
	}
	
	packets_.clear();

	pChannel_ = NULL;
	numMessages_ = 0;

	currMsgID_ = 0;
	currMsgPacketCount_ = 0;
	currMsgLength_ = 0;
	currMsgLengthPos_ = 0;
	currMsgHandlerLength_ = 0;
	pCurrMsgHandler_ = NULL;
	_calcPacketMaxSize();
}

//-------------------------------------------------------------------------------------
void Bundle::clearPackets()
{
	if(pCurrPacket_ != NULL)
	{
		packets_.push_back(pCurrPacket_);
		pCurrPacket_ = NULL;
	}

	Packets::iterator iter = packets_.begin();
	for (; iter != packets_.end(); ++iter)
	{
		RECLAIM_PACKET(isTCPPacket_, (*iter));
	}

	packets_.clear();
}

//-------------------------------------------------------------------------------------
void Bundle::newMessage(const MessageHandler& msgHandler)
{
	pCurrMsgHandler_ = &msgHandler;
	
	if(pCurrPacket_ == NULL)
	{
		if(packets_.size() > 0)
		{
			if (packetHaveSpace())
			{
				pCurrPacket_ = packets_.back();
				packets_.pop_back();
			}
			else
			{
				this->newPacket();
			}
		}
		else
		{
			this->newPacket();
		}
	}

	finiMessage(false);
	OURO_ASSERT(pCurrPacket_ != NULL);
	
	(*this) << msgHandler.msgID;
	pCurrPacket_->messageID(msgHandler.msgID);

	// Here for a non-fixed length message, you need to set its message length bit to 0, and finally need to fill the length.
	if(msgHandler.msgLen == NETWORK_VARIABLE_MESSAGE)
	{
		MessageLength msglen = 0;
		currMsgLengthPos_ = pCurrPacket_->wpos();
		(*this) << msglen;
	}

	++numMessages_;
	currMsgID_ = msgHandler.msgID;
	currMsgPacketCount_ = 0;
	currMsgHandlerLength_ = msgHandler.msgLen;
}

//-------------------------------------------------------------------------------------
void Bundle::finiMessage(bool isSend)
{
	OURO_ASSERT(pCurrPacket_ != NULL);
	
	pCurrPacket_->pBundle(this);

	if(isSend)
	{
		++currMsgPacketCount_;
		packets_.push_back(pCurrPacket_);
	}

	// Track the message
	if(pCurrMsgHandler_){
		if(isSend || numMessages_ > 1)
		{
			NetworkStats::getSingleton().trackMessage(NetworkStats::SEND, 
									*pCurrMsgHandler_, currMsgLength_);
		}
	}

	// Here you need to set its final length information for non-fixed length messages.
	if(currMsgID_ > 0 && (currMsgHandlerLength_ < 0))
	{
		Packet* pPacket = pCurrPacket_;
		if(currMsgPacketCount_ > 0)
			pPacket = packets_[packets_.size() - currMsgPacketCount_];

		currMsgLength_ -= NETWORK_MESSAGE_ID_SIZE;
		currMsgLength_ -= NETWORK_MESSAGE_LENGTH_SIZE;

		// It is not possible to exceed NETWORK_MESSAGE_MAX_SIZE by designing a package.
		if(g_componentType == BOTS_TYPE || g_componentType == CLIENT_TYPE)
		{
			OURO_ASSERT(currMsgLength_ <= NETWORK_MESSAGE_MAX_SIZE);
		}

		// If the message length is greater than or equal to NETWORK_MESSAGE_MAX_SIZE
		// Use the extended message length mechanism to fill the message length with 4 bytes
		// used to describe a larger length
		if(currMsgLength_ >= NETWORK_MESSAGE_MAX_SIZE)
		{
			MessageLength1 ex_msg_length = currMsgLength_;
			Ouroboros::EndianConvert(ex_msg_length);

			MessageLength msgLen = NETWORK_MESSAGE_MAX_SIZE;
			Ouroboros::EndianConvert(msgLen);

			memcpy(&pPacket->data()[currMsgLengthPos_], 
				(uint8*)&msgLen, NETWORK_MESSAGE_LENGTH_SIZE);

			pPacket->insert(currMsgLengthPos_ + NETWORK_MESSAGE_LENGTH_SIZE, (uint8*)&ex_msg_length, 
														NETWORK_MESSAGE_LENGTH1_SIZE);
		}
		else
		{
			MessageLength msgLen = (MessageLength)currMsgLength_;
			Ouroboros::EndianConvert(msgLen);

			memcpy(&pPacket->data()[currMsgLengthPos_], 
				(uint8*)&msgLen, NETWORK_MESSAGE_LENGTH_SIZE);
		}
	}

	if (Network::g_trace_packet > 0)
		debugCurrentMessages(currMsgID_, pCurrMsgHandler_, pCurrPacket_, packets_, currMsgLength_, pChannel_);

	if(isSend)
	{
		currMsgHandlerLength_ = 0;
		pCurrPacket_ = NULL;
	}
		
	currMsgID_ = 0;
	currMsgPacketCount_ = 0;
	currMsgLength_ = 0;
	currMsgLengthPos_ = 0;
}

//-------------------------------------------------------------------------------------
void Bundle::debugCurrentMessages(MessageID currMsgID, const Network::MessageHandler* pCurrMsgHandler,
	Network::Packet* pCurrPacket, Network::Bundle::Packets& packets, Network::MessageLength1 currMsgLength,
	Network::Channel* pChannel)
{
	if (currMsgID == 0)
		return;

	if (!pCurrMsgHandler || currMsgID != pCurrMsgHandler->msgID || !pCurrMsgHandler->pMessageHandlers)
		return;

	if (pCurrMsgHandler->msgLen == NETWORK_VARIABLE_MESSAGE)
	{
		// Because the Bundle::finiMessage and other places encounter the variable parameter message, the length of the message header is removed, and the message to be restored is added back here.
		currMsgLength += NETWORK_MESSAGE_ID_SIZE;
		currMsgLength += NETWORK_MESSAGE_LENGTH_SIZE;
		if (currMsgLength - NETWORK_MESSAGE_ID_SIZE - NETWORK_MESSAGE_LENGTH_SIZE >= NETWORK_MESSAGE_MAX_SIZE)
			currMsgLength += NETWORK_MESSAGE_LENGTH1_SIZE;
	}
	
	if (currMsgLength >= MemoryStream::MAX_SIZE)
	{
		DebugHelper::getSingleton().set_errorcolor();
		printf("%s", fmt::format("ERROR: Bundle::debugCurrentMessages(): is too big, size=%d\n", currMsgLength).c_str());
		DebugHelper::getSingleton().set_normalcolor();
		
		return;
	}

	MemoryStream* pMemoryStream = MemoryStream::createPoolObject(OBJECTPOOL_POINT);
	
	// Find the message header by message length and then output the message content
	int msglen = currMsgLength;
	if(pCurrPacket)
	{
		// If all the contents of the current message are in the current package, you can directly output the content.
		msglen -= pCurrPacket->length();
		if(msglen <= 0)
		{
			pMemoryStream->append(pCurrPacket->data() + pCurrPacket->wpos() - currMsgLength, currMsgLength);
		}
		else
		{
			int idx = 0;

			Network::Bundle::Packets::reverse_iterator packiter = packets.rbegin();
			for (; packiter != packets.rend(); ++packiter)
			{
				++idx;

				Network::Packet* pPacket = (*packiter);

				// The current package may have been calculated
				if (pCurrPacket == pPacket)
					continue;

				// If everything is in the package
				if((int)pPacket->length() >= msglen)
				{
					int wpos = pPacket->length() - msglen;
					pMemoryStream->append(pPacket->data() + wpos, msglen);
					
					for(size_t i = packets.size() - idx; i < packets.size(); ++i)
					{
						Network::Packet* pPacket1 = packets[i];
						
						// This package has been processed above.
						if (pPacket1 == pPacket || pCurrPacket == pPacket1)
							continue;
						
						// All the contents of the package are added during the period
						pMemoryStream->append(pPacket1->data() + pPacket1->rpos(), pPacket1->length());
					}
					
					// Add all the contents of the current package
					pMemoryStream->append(pCurrPacket->data() + pCurrPacket->rpos(), pCurrPacket->length());
					break;
				}
				else
				{
					msglen -= pPacket->length();
				}
			}
		}
	}
	
	// Some packages of sendto operations cause such packages to not need to be tracked.
	if(pMemoryStream->length() < NETWORK_MESSAGE_ID_SIZE)
	{
		MemoryStream::reclaimPoolObject(pMemoryStream);
		return;
	}

	OURO_ASSERT(currMsgLength == pMemoryStream->length());
	
	TRACE_MESSAGE_PACKET(false, pMemoryStream, pCurrMsgHandler, pMemoryStream->length(),
		(pChannel != NULL ? pChannel->c_str() : "None"), false);
					
	MemoryStream::reclaimPoolObject(pMemoryStream);
}

//-------------------------------------------------------------------------------------
bool Bundle::revokeMessage(int32 size)
{
	if(pCurrPacket_)
	{
		if(size >= (int32)pCurrPacket_->wpos())
		{
			size -= pCurrPacket_->wpos();
			RECLAIM_PACKET(isTCPPacket_, pCurrPacket_);
			pCurrPacket_ = NULL;
		}
		else
		{
			pCurrPacket_->wpos(pCurrPacket_->wpos() - size);
			size = 0;
		}
	}
	
	while(size > 0 && packets_.size() > 0)
	{
		Network::Packet* pPacket = packets_.back();
		if(pPacket->wpos() > (size_t)size)
		{
			pPacket->wpos(pPacket->wpos() - size);
			size = 0;
			break;
		}
		else
		{
			size -= pPacket->wpos();
			RECLAIM_PACKET(isTCPPacket_, pPacket);
			packets_.pop_back();
		}
	}
	
	if (pCurrPacket_)
	{
		packets_.push_back(pCurrPacket_);
		pCurrPacket_ = NULL;
	}

	--numMessages_;
	currMsgHandlerLength_ = 0;
		
	currMsgID_ = 0;
	currMsgPacketCount_ = 0;
	currMsgLength_ = 0;
	currMsgLengthPos_ = 0;
	pCurrMsgHandler_ = NULL;

	return size == 0;
}

//-------------------------------------------------------------------------------------
}
}
