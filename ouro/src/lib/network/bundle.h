// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_NETWORK_BUNDLE_H
#define OURO_NETWORK_BUNDLE_H

#include "common/common.h"
#include "common/timer.h"
#include "common/objectpool.h"
#include "helper/debug_helper.h"
#include "network/address.h"
#include "network/event_dispatcher.h"
#include "network/endpoint.h"
#include "network/common.h"
#include "network/tcp_packet.h"
#include "network/udp_packet.h"
#include "network/interface_defs.h"

namespace Ouroboros { 
namespace Network
{
class NetworkInterface;
class Channel;

#define PACKET_OUT_VALUE(v, expectSize)																		\
	OURO_ASSERT(packetsLength() >= (int32)expectSize);														\
																											\
	size_t currSize = 0;																					\
	size_t reclaimCount = 0;																				\
																											\
	Packets::iterator iter = packets_.begin();																\
	for (; iter != packets_.end(); ++iter)																	\
	{																										\
		Packet* pPacket = (*iter);																			\
		size_t remainSize = (size_t)expectSize - currSize;													\
																											\
		if(pPacket->length() >= remainSize)																	\
		{																									\
			memcpy(((uint8*)&v) + currSize, pPacket->data() + pPacket->rpos(), remainSize);					\
			pPacket->rpos((int)(pPacket->rpos() + remainSize));												\
																											\
			if(pPacket->length() == 0)																		\
			{																								\
				RECLAIM_PACKET(pPacket->isTCPPacket(), pPacket);											\
				++reclaimCount;																				\
			}																								\
																											\
			break;																							\
		}																									\
		else																								\
		{																									\
			memcpy(((uint8*)&v) + currSize, pPacket->data() + pPacket->rpos(), pPacket->length());			\
			currSize += pPacket->length();																	\
			pPacket->done();																				\
			RECLAIM_PACKET(pPacket->isTCPPacket(), pPacket);												\
			++reclaimCount;																					\
		}																									\
	}																										\
																											\
	if(reclaimCount > 0)																					\
		packets_.erase(packets_.begin(), packets_.begin() + reclaimCount);									\
																											\
	return *this;																							\


// Create and recycle from the object pool
#define MALLOC_BUNDLE() Network::Bundle::createPoolObject()
#define DELETE_BUNDLE(obj) { Network::Bundle::reclaimPoolObject(obj); obj = NULL; }
#define RECLAIM_BUNDLE(obj) { Network::Bundle::reclaimPoolObject(obj);}

class Bundle : public PoolObject
{
public:
	typedef OUROShared_ptr< SmartPoolObject< Bundle > > SmartPoolObjectPtr;
	static SmartPoolObjectPtr createSmartPoolObj(const std::string& logPoint);
	static ObjectPool<Bundle>& ObjPool();
	static Bundle* createPoolObject(const std::string& logPoint);
	static void reclaimPoolObject(Bundle* obj);
	static void destroyObjPool();
	virtual void onReclaimObject();
	virtual size_t getPoolObjectBytes();

	typedef std::vector<Packet*> Packets;
	
	Bundle(Channel * pChannel = NULL, ProtocolType pt = PROTOCOL_TCP);
	Bundle(const Bundle& bundle);
	virtual ~Bundle();
	
	void newMessage(const MessageHandler& msgHandler);
	void finiMessage(bool isSend = true);

	void clearPackets();
	
	INLINE void pCurrMsgHandler(const Network::MessageHandler* pMsgHandler);
	INLINE const Network::MessageHandler* pCurrMsgHandler() const;

	/**
		Calculate the total length of all packages including the ones that have not been written yet
	*/
	int32 packetsLength(bool calccurr = true);

	INLINE bool isTCPPacket() const{ return isTCPPacket_; }
	INLINE void isTCPPacket(bool v){ isTCPPacket_ = v; }

	void clear(bool isRecl);
	bool empty() const;
	
	void copy(const Bundle& bundle);

	INLINE int32 packetMaxSize() const;
	int packetsSize() const;

	/**
		Revoke some message bytes
	*/
	bool revokeMessage(int32 size);
		
	/**
		Calculate the free space remaining after packetMaxSize - the length of the last packet
	*/
	INLINE int32 lastPacketSpace();
	INLINE bool packetHaveSpace();
	
	INLINE Packets& packets();
	INLINE Packet* pCurrPacket() const;
	INLINE void pCurrPacket(Packet* p);
	
	INLINE void finiCurrPacket();

	Packet* newPacket();
	
	INLINE void pChannel(Channel* p);
	INLINE Channel* pChannel();
	
	INLINE MessageID messageID() const;
	INLINE void messageID(MessageID id);
	
	INLINE int32 numMessages() const;

	INLINE void currMsgPacketCount(uint32 v);
	INLINE uint32 currMsgPacketCount() const;

	INLINE void currMsgLength(MessageLength1 v);
	INLINE MessageLength1 currMsgLength() const;

	INLINE void currMsgLengthPos(size_t v);
	INLINE size_t currMsgLengthPos() const;

	static void debugCurrentMessages(MessageID currMsgID, const Network::MessageHandler* pCurrMsgHandler, 
		Network::Packet* pCurrPacket, Network::Bundle::Packets& packets, Network::MessageLength1 currMsgLength,
		Network::Channel* pChannel);
	
protected:
	void _calcPacketMaxSize();
	int32 onPacketAppend(int32 addsize, bool inseparable = true);

public:
    Bundle &operator<<(uint8 value)
    {
		onPacketAppend(sizeof(uint8));
        (*pCurrPacket_) << value;
        return *this;
    }

    Bundle &operator<<(uint16 value)
    {
		onPacketAppend(sizeof(uint16));
        (*pCurrPacket_) << value;
        return *this;
    }

    Bundle &operator<<(uint32 value)
    {
		onPacketAppend(sizeof(uint32));
        (*pCurrPacket_) << value;
        return *this;
    }

    Bundle &operator<<(uint64 value)
    {
		onPacketAppend(sizeof(uint64));
        (*pCurrPacket_) << value;
        return *this;
    }

    Bundle &operator<<(int8 value)
    {
		onPacketAppend(sizeof(int8));
        (*pCurrPacket_) << value;
        return *this;
    }

    Bundle &operator<<(int16 value)
    {
		onPacketAppend(sizeof(int16));
        (*pCurrPacket_) << value;
        return *this;
    }

    Bundle &operator<<(int32 value)
    {
		onPacketAppend(sizeof(int32));
        (*pCurrPacket_) << value;
        return *this;
    }

    Bundle &operator<<(int64 value)
    {
		onPacketAppend(sizeof(int64));
        (*pCurrPacket_) << value;
        return *this;
    }

    Bundle &operator<<(float value)
    {
		onPacketAppend(sizeof(float));
        (*pCurrPacket_) << value;
        return *this;
    }

    Bundle &operator<<(double value)
    {
		onPacketAppend(sizeof(double));
        (*pCurrPacket_) << value;
        return *this;
    }

    Bundle &operator<<(COMPONENT_TYPE value)
    {
		onPacketAppend(sizeof(int32));
        (*pCurrPacket_) << value;
        return *this;
    }

    Bundle &operator<<(ENTITYCALL_TYPE value)
    {
		onPacketAppend(sizeof(int32));
        (*pCurrPacket_) << value;
        return *this;
    }

    Bundle &operator<<(bool value)
    {
		onPacketAppend(sizeof(int8));
        (*pCurrPacket_) << value;
        return *this;
    }

    Bundle &operator<<(const std::string &value)
    {
		int32 len = (int32)value.size() + 1; // +1 is the 0 position of the end of the string
		int32 addtotalsize = 0;

		while(len > 0)
		{
			int32 ilen = onPacketAppend(len, false);
			pCurrPacket_->append(value.c_str() + addtotalsize, ilen);
			addtotalsize += ilen;
			len -= ilen;
		}

        return *this;
    }
	
    Bundle &operator<<(const char *str)
    {
		int32 len = (int32)strlen(str) + 1; // +1 is the 0 position of the end of the string
		int32 addtotalsize = 0;

		while(len > 0)
		{
			int32 ilen = onPacketAppend(len, false);
			pCurrPacket_->append(str + addtotalsize, ilen);
			addtotalsize += ilen;
			len -= ilen;
		}

        return *this;
    }
    
	Bundle &append(Bundle* pBundle)
	{
		OURO_ASSERT(pBundle != NULL);
		return append(*pBundle);
	}

	Bundle &append(Bundle& bundle)
	{
		Packets::iterator iter = bundle.packets_.begin();
		for(; iter!=bundle.packets_.end(); ++iter)
		{
			append((*iter)->data() + (*iter)->rpos(), (int)(*iter)->length());
		}
		
		if(bundle.pCurrPacket_ == NULL)
			return *this;

		return append(bundle.pCurrPacket_->data() + bundle.pCurrPacket_->rpos(), (int)bundle.pCurrPacket_->length());
	}

	Bundle &append(MemoryStream* s)
	{
		OURO_ASSERT(s != NULL);
		return append(*s);
	}

	Bundle &append(MemoryStream& s)
	{
		if(s.length() > 0)
			return append(s.data() + s.rpos(), (int)s.length());

		return *this;
	}

	Bundle &appendBlob(const std::string& str)
	{
		return appendBlob((const uint8 *)str.data(), (ArraySize)str.size());
	}

	Bundle &appendBlob(const char* str, ArraySize n)
	{
		return appendBlob((const uint8 *)str, n);
	}

	Bundle &appendBlob(const uint8 *str, ArraySize n)
	{
		(*this) << n;
		return assign((char*)str, n);
	}

	Bundle &append(const uint8 *str, int n)
	{
		return assign((char*)str, n);
	}

	Bundle &append(const char *str, int n)
	{
		return assign(str, n);
	}

	Bundle &appendPackAnyXYZ(float x, float y, float z, const float epsilon = 0.5f)
	{
		if(epsilon > 0.f)
		{
			x = floorf(x + epsilon);
			y = floorf(y + epsilon);
			z = floorf(z + epsilon);
		}
		
		*this << x << y << z;
		return (*this);
	}

	Bundle &appendPackAnyXZ(float x, float z, const float epsilon = 0.5f)
    {
		if(epsilon > 0.f)
		{
			x = floorf(x + epsilon);
			z = floorf(z + epsilon);
		}

        *this << x << z;
		return (*this);
	}

    Bundle &appendPackXYZ(float x, float y, float z, float minf = -256.f)
    {
		x -= minf;
		y -= minf / 2.f;
		z -= minf;

		// The maximum value should not exceed -256~256
		// y don't exceed -128~128
        uint32 packed = 0;
        packed |= ((int)(x / 0.25f) & 0x7FF);
        packed |= ((int)(z / 0.25f) & 0x7FF) << 11;
        packed |= ((int)(y / 0.25f) & 0x3FF) << 22;
        *this << packed;
        return (*this);
    }

    Bundle &appendPackXZ(float x, float z)
    {
		MemoryStream::PackFloatXType xPackData; 
		xPackData.fv = x;

		MemoryStream::PackFloatXType zPackData; 
		zPackData.fv = z;
		
		// 0-7 digits store the mantissa, 8-10 digits store the index, 11 digits store the logo
		// Since 24 bits are used to store 2 floats, and it is required to be able to reach a number between -512 and 512
		// The 8-bit mantissa can only have a maximum of 256, and the exponent has only 3 digits (the maximum value of the floating-point number is 2^(2^3)=256).
		// We give up the first place to make the range reach (-512~-2), (2~512)
		// So here we guarantee that the minimum number is -2.f or 2.f
		xPackData.fv += xPackData.iv < 0 ? -2.f : 2.f;
		zPackData.fv += zPackData.iv < 0 ? -2.f : 2.f;

		uint32 data = 0;

		// 0x7ff000 = 11111111111000000000000
		// 0x0007ff = 00000000000011111111111
		const uint32 xCeilingValues[] = { 0, 0x7ff000 };
		const uint32 zCeilingValues[] = { 0, 0x0007ff };

		// Here, if this floating point number overflows, set the floating point number to the maximum number.
		// This checks the upper 4 bits of the exponent and the flag bit. If the upper four bits are not 0, it will overflow. If the lower 4 bits and the 8-bit mantissa are not 0, then overflow.
		// 0x7c000000 = 1111100000000000000000000000000
		// 0x40000000 = 1000000000000000000000000000000
		// 0x3ffc000  = 0000011111111111100000000000000
		data |= xCeilingValues[((xPackData.uv & 0x7c000000) != 0x40000000) || ((xPackData.uv & 0x3ffc000) == 0x3ffc000)];
		data |= zCeilingValues[((zPackData.uv & 0x7c000000) != 0x40000000) || ((zPackData.uv & 0x3ffc000) == 0x3ffc000)];
		
		// Copy the 8-bit mantissa and the 3-bit exponent, if the floating-point number of the remaining mantissa is 1 then +1 is rounded up and stored in data
		// 0x7ff000 = 11111111111000000000000
		// 0x0007ff = 00000000000011111111111
		// 0x4000	= 00000000100000000000000
		data |= ((xPackData.uv >>  3) & 0x7ff000) + ((xPackData.uv & 0x4000) >> 2);
		data |= ((zPackData.uv >> 15) & 0x0007ff) + ((zPackData.uv & 0x4000) >> 14);
		
		// Make sure the value is in range
		// 0x7ff7ff = 11111111111011111111111
		data &= 0x7ff7ff;

		// copy the flag bit
		// 0x800000 = 100000000000000000000000
		// 0x000800 = 000000000000100000000000
		data |=  (xPackData.uv >>  8) & 0x800000;
		data |=  (zPackData.uv >> 20) & 0x000800;

		uint8 packs[3];
		packs[0] = (uint8)(data >> 16);
		packs[1] = (uint8)(data >> 8);
		packs[2] = (uint8)data;
		(*this).append(packs, 3);
		return (*this);
    }

	Bundle &appendPackY(float y)
	{
		MemoryStream::PackFloatXType yPackData; 
		yPackData.fv = y;

		yPackData.fv += yPackData.iv < 0 ? -2.f : 2.f;
		uint16 data = 0;
		data = (yPackData.uv >> 12) & 0x7fff;
 		data |= ((yPackData.uv >> 16) & 0x8000);

		(*this) << data;
		return (*this);
	}
	
	Bundle &assign(const char *str, int n)
	{
		int32 len = (int32)n;
		int32 addtotalsize = 0;

		while(len > 0)
		{
			int32 ilen = onPacketAppend(len, false);
			pCurrPacket_->append((uint8*)(str + addtotalsize), ilen);
			addtotalsize += ilen;
			len -= ilen;
		}

		return *this;
	}

    Bundle &operator>>(bool &value)
    {
        PACKET_OUT_VALUE(value, sizeof(bool));
    }

    Bundle &operator>>(uint8 &value)
    {
        PACKET_OUT_VALUE(value, sizeof(uint8));
    }

    Bundle &operator>>(uint16 &value)
    {
        PACKET_OUT_VALUE(value, sizeof(uint16));
    }

    Bundle &operator>>(uint32 &value)
    {
        PACKET_OUT_VALUE(value, sizeof(uint32));
    }

    Bundle &operator>>(uint64 &value)
    {
        PACKET_OUT_VALUE(value, sizeof(uint64));
    }

    Bundle &operator>>(int8 &value)
    {
        PACKET_OUT_VALUE(value, sizeof(int8));
    }

    Bundle &operator>>(int16 &value)
    {
        PACKET_OUT_VALUE(value, sizeof(int16));
    }

    Bundle &operator>>(int32 &value)
    {
        PACKET_OUT_VALUE(value, sizeof(int32));
    }

    Bundle &operator>>(int64 &value)
    {
        PACKET_OUT_VALUE(value, sizeof(int64));
    }

    Bundle &operator>>(float &value)
    {
        PACKET_OUT_VALUE(value, sizeof(float));
    }

    Bundle &operator>>(double &value)
    {
        PACKET_OUT_VALUE(value, sizeof(double));
    }

    Bundle &operator>>(COMPONENT_TYPE &value)
    {
        PACKET_OUT_VALUE(value, sizeof(int32/*PACKET_OUT_VALUE(value, sizeof(int32/*²Î¿¼MemoryStream*/));
    }

    Bundle &operator>>(ENTITYCALL_TYPE &value)
    {
        PACKET_OUT_VALUE(value, sizeof(int32/*PACKET_OUT_VALUE(value, sizeof(int32/*²Î¿¼MemoryStream*/));
    }

    Bundle &operator>>(std::string& value)
    {
		OURO_ASSERT(packetsLength() > 0);
		size_t reclaimCount = 0;
		value.clear();

		Packets::iterator iter = packets_.begin();
		for (; iter != packets_.end(); ++iter)
		{
			Packet* pPacket = (*iter);

			while (pPacket->length() > 0)
			{
				char c = pPacket->read<char>();
				if (c == 0)
					break;

				value += c;
			}

			if(pPacket->data()[pPacket->rpos() - 1] == 0)
			{
				if(pPacket->length() == 0)
				{
					RECLAIM_PACKET(pPacket->isTCPPacket(), pPacket);
					++reclaimCount;
				}

				break;
			}
			else
			{
				OURO_ASSERT(pPacket->length() == 0);
				++reclaimCount;
				RECLAIM_PACKET(pPacket->isTCPPacket(), pPacket);
			}
		}

		if(reclaimCount > 0)
			packets_.erase(packets_.begin(), packets_.begin() + reclaimCount);

		return *this;
    }

	ArraySize readBlob(std::string& datas)
	{
		datas.clear();

		ArraySize rsize = 0;
		(*this) >> rsize;

		if((int32)rsize > packetsLength())
			return 0;

		size_t reclaimCount = 0;
		datas.reserve(rsize);

		Packets::iterator iter = packets_.begin();
		for (; iter != packets_.end(); ++iter)
		{
			Packet* pPacket = (*iter);

			if(pPacket->length() > rsize - datas.size())
			{
				datas.append((char*)pPacket->data() + pPacket->rpos(), rsize - datas.size());
				pPacket->rpos((int)(pPacket->rpos() + rsize - datas.size()));
				if(pPacket->length() == 0)
				{
					RECLAIM_PACKET(pPacket->isTCPPacket(), pPacket);
					++reclaimCount;
				}

				break;
			}
			else
			{
				datas.append((char*)pPacket->data() + pPacket->rpos(), (int)pPacket->length());
				pPacket->done();
				RECLAIM_PACKET(pPacket->isTCPPacket(), pPacket);
				++reclaimCount;
			}
		}

		if(reclaimCount > 0)
			packets_.erase(packets_.begin(), packets_.begin() + reclaimCount);

		return rsize;
	}

private:
	Channel* pChannel_;
	int32 numMessages_;

	Packet* pCurrPacket_;
	MessageID currMsgID_;
	uint32 currMsgPacketCount_;
	MessageLength1 currMsgLength_;	
	int32 currMsgHandlerLength_;
	size_t currMsgLengthPos_;

	Packets packets_;

	bool isTCPPacket_;
	int32 packetMaxSize_;

	const Network::MessageHandler* pCurrMsgHandler_;
};

}
}

#ifdef CODE_INLINE
#include "bundle.inl"
#endif
#endif // OURO_NETWORK_BUNDLE_H
