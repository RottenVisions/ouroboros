// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_NETWORK_COMMON_H
#define OURO_NETWORK_COMMON_H

// common include
#include "common/common.h"
#include "helper/debug_option.h"

namespace Ouroboros { 
namespace Network
{
const uint32 BROADCAST = 0xFFFFFFFF;
const uint32 LOCALHOST = 0x0100007F;

// the ID of the message
typedef uint16	MessageID;

// Message length, the current length has 2 kinds, the default message length is the largest MessageLength
// When you exceed this number, you need to extend the length. The bottom layer uses MessageLength1.
typedef uint16	MessageLength;		// 最大65535
typedef uint32 MessageLength1; // max 4294967295

typedef int32	ChannelID;
const ChannelID CHANNEL_ID_NULL = 0;

// channel timeout
extern float g_channelInternalTimeout;
extern float g_channelExternalTimeout;

// channel send timeout retry
extern uint32 g_intReSendInterval;
extern uint32 g_intReSendRetries;
extern uint32 g_extReSendInterval;
extern uint32 g_extReSendRetries;

// External channel encryption category
extern int8 g_channelExternalEncryptType;

// listen listener queue maximum
extern uint32 g_SOMAXCONN;

// udp handshake package
extern const char* UDP_HELLO;
extern const char* UDP_HELLO_ACK;

// UDP parameters
extern uint32 g_rudp_intWritePacketsQueueSize;
extern uint32 g_rudp_intReadPacketsQueueSize;
extern uint32 g_rudp_extWritePacketsQueueSize;
extern uint32 g_rudp_extReadPacketsQueueSize;
extern uint32 g_rudp_tickInterval;
extern uint32 g_rudp_minRTO;
extern uint32 g_rudp_mtu;
extern uint32 g_rudp_missAcksResend;
extern bool g_rudp_congestionControl;
extern bool g_rudp_nodelay;

// Certificate file required for HTTPS/WSS/SSL communication
extern std::string g_sslCertificate;
extern std::string g_sslPrivateKey;

// Do not do channel timeout check
#define CLOSE_CHANNEL_INACTIVITIY_DETECTION()										\
{																					\
	Network::g_channelExternalTimeout = Network::g_channelInternalTimeout = -1.0f;	\
}																					\

	
namespace udp{
}

namespace tcp{
}

// Encrypt additional stored information to occupy bytes (length + padding)
#define ENCRYPTTION_WASTAGE_SIZE			(1 + 7)

#define PACKET_MAX_SIZE						1500
#ifndef PACKET_MAX_SIZE_TCP
#define PACKET_MAX_SIZE_TCP					1460
#endif
#define PACKET_MAX_SIZE_UDP					1472

typedef uint16								PacketLength;				// 最大65535
#define PACKET_LENGTH_SIZE					sizeof(PacketLength)

#define NETWORK_MESSAGE_ID_SIZE				sizeof(Network::MessageID)
#define NETWORK_MESSAGE_LENGTH_SIZE			sizeof(Network::MessageLength)
#define NETWORK_MESSAGE_LENGTH1_SIZE		sizeof(Network::MessageLength1)
#define NETWORK_MESSAGE_MAX_SIZE			65535
#define NETWORK_MESSAGE_MAX_SIZE1			4294967295

// Game content available package size
#define GAME_PACKET_MAX_SIZE_TCP			PACKET_MAX_SIZE_TCP - NETWORK_MESSAGE_ID_SIZE - \
											NETWORK_MESSAGE_LENGTH_SIZE - ENCRYPTTION_WASTAGE_SIZE

/** kbe machine port*/
#define OURO_PORT_START						20000
#define OURO_MACHINE_BROADCAST_SEND_PORT OURO_PORT_START + 86 // The port on which the machine receives the broadcast
#define OURO_PORT_BROADCAST_DISCOVERY		OURO_PORT_START + 87
#define OURO_MACHINE_TCP_PORT				OURO_PORT_START + 88

#define OURO_INTERFACES_TCP_PORT				30099

/*
	Network message type, fixed length or variable length.
	if you need a custom length, fill in the length when you declare it in NETWORK_INTERFACE_DECLARE_BEGIN.
*/
#ifndef NETWORK_FIXED_MESSAGE
#define NETWORK_FIXED_MESSAGE 0
#endif

#ifndef NETWORK_VARIABLE_MESSAGE
#define NETWORK_VARIABLE_MESSAGE -1
#endif

// network message category
enum NETWORK_MESSAGE_TYPE
{
	NETWORK_MESSAGE_TYPE_COMPONENT = 0, // component message
		NETWORK_MESSAGE_TYPE_ENTITY = 1,	// entity消息
};

enum ProtocolType
{
	PROTOCOL_TCP = 0,
	PROTOCOL_UDP = 1,
};

enum ProtocolSubType
{
	SUB_PROTOCOL_DEFAULT = 0,
	SUB_PROTOCOL_UDP = 1,
	SUB_PROTOCOL_KCP = 2,
};

enum Reason
{
	REASON_SUCCESS = 0,				 // No reason.
	REASON_TIMER_EXPIRED = -1,		 // Timer expired.
	REASON_NO_SUCH_PORT = -2,		 // Destination port is not open.
	REASON_GENERAL_NETWORK = -3,	 // The network is stuffed.
	REASON_CORRUPTED_PACKET = -4,	 // Got a bad packet.
	REASON_NONEXISTENT_ENTRY = -5,	 // Wanted to call a null function.
	REASON_WINDOW_OVERFLOW = -6,	 // Channel send window overflowed.
	REASON_INACTIVITY = -7,			 // Channel inactivity timeout.
	REASON_RESOURCE_UNAVAILABLE = -8,// Corresponds to EAGAIN
	REASON_CLIENT_DISCONNECTED = -9, // Client disconnected voluntarily.
	REASON_TRANSMIT_QUEUE_FULL = -10,// Corresponds to ENOBUFS
	REASON_CHANNEL_LOST = -11,		 // Corresponds to channel lost
	REASON_SHUTTING_DOWN = -12,		 // Corresponds to shutting down app.
	REASON_WEBSOCKET_ERROR = -13,    // html5 error.
	REASON_CHANNEL_CONDEMN = -14,	 // condemn error.
};

inline
const char * reasonToString(Reason reason)
{
	const char * reasons[] =
	{
		"REASON_SUCCESS",
		"REASON_TIMER_EXPIRED",
		"REASON_NO_SUCH_PORT",
		"REASON_GENERAL_NETWORK",
		"REASON_CORRUPTED_PACKET",
		"REASON_NONEXISTENT_ENTRY",
		"REASON_WINDOW_OVERFLOW",
		"REASON_INACTIVITY",
		"REASON_RESOURCE_UNAVAILABLE",
		"REASON_CLIENT_DISCONNECTED",
		"REASON_TRANSMIT_QUEUE_FULL",
		"REASON_CHANNEL_LOST",
		"REASON_SHUTTING_DOWN",
		"REASON_WEBSOCKET_ERROR",
		"REASON_CHANNEL_CONDEMN"
	};

	unsigned int index = -reason;

	if (index < sizeof(reasons)/sizeof(reasons[0]))
	{
		return reasons[index];
	}

	return "REASON_UNKNOWN";
}


#define NETWORK_SEND_TO_ENDPOINT(ep, op, pPacket)															\
{																											\
	int retries = 0;																						\
	Network::Reason reason;																					\
																											\
	while(true)																								\
	{																										\
		retries++;																							\
		int slen = ep->op(pPacket->data(), pPacket->totalSize());											\
																											\
		if(slen != (int)pPacket->totalSize())																\
		{																									\
			reason = Network::PacketSender::checkSocketErrors(ep, slen, pPacket->totalSize());				\
						/*if the transmission has an error then we can continue to try once and exit more than 3 times.*/										\
			if (reason == Network::REASON_NO_SUCH_PORT && retries <= 3)										\
			{																								\
				continue;																					\
			}																								\
																											\
						/*if the system send buffer is full, then we wait 10ms*/													\
			if ((reason == REASON_RESOURCE_UNAVAILABLE || reason == REASON_GENERAL_NETWORK)					\
															&& retries <= 3)								\
			{																								\
				WARNING_MSG(fmt::format("{}: "																\
					"Transmit queue full, waiting for space... ({})\n",										\
					__FUNCTION__, retries));																\
																											\
				Ouroboros::sleep(10);																		\
				continue;																					\
			}																								\
																											\
			if(retries > 3 && reason != Network::REASON_SUCCESS)											\
			{																								\
				ERROR_MSG(fmt::format("NETWORK_SEND::send: packet discarded(reason={}).\n",					\
															(reasonToString(reason))));						\
				break;																						\
			}																								\
		}																									\
		else																								\
		{																									\
			break;																							\
		}																									\
	}																										\
																											\
}																											\


#define SEND_BUNDLE_COMMON(SND_FUNC, BUNDLE)																\
	BUNDLE.finiMessage();																					\
																											\
	Network::Bundle::Packets::iterator iter = BUNDLE.packets().begin();										\
	for (; iter != BUNDLE.packets().end(); ++iter)															\
	{																										\
		Packet* pPacket = (*iter);																			\
		int retries = 0;																					\
		Reason reason;																						\
		pPacket->sentSize = 0;																				\
																											\
		while(true)																							\
		{																									\
			++retries;																						\
			int slen = SND_FUNC;																			\
																											\
			if(slen > 0)																					\
				pPacket->sentSize += slen;																	\
																											\
			if(pPacket->sentSize != pPacket->length())														\
			{																								\
				reason = PacketSender::checkSocketErrors(&ep);												\
								/*if the transmission has an error then we can continue to try once and exit more than 60 times.*/									\
				if (reason == REASON_NO_SUCH_PORT && retries <= 3)											\
				{																							\
					continue;																				\
				}																							\
																											\
								/*if the system send buffer is full, then we wait 10ms*/												\
				if ((reason == REASON_RESOURCE_UNAVAILABLE || reason == REASON_GENERAL_NETWORK)				\
																					&& retries <= 60)		\
				{																							\
					WARNING_MSG(fmt::format("{}: "															\
						"Transmit queue full, waiting for space... ({})\n",									\
						__FUNCTION__, retries));															\
																											\
					ep.waitSend();																			\
					continue;																				\
				}																							\
																											\
				if(retries > 60 && reason != REASON_SUCCESS)												\
				{																							\
					ERROR_MSG(fmt::format("Bundle::basicSendWithRetries: packet discarded(reason={}).\n",	\
															(reasonToString(reason))));						\
					break;																					\
				}																							\
			}																								\
			else																							\
			{																								\
				break;																						\
			}																								\
		}																									\
																											\
	}																										\
																											\
	BUNDLE.clearPackets();																					\
																											\


#define SEND_BUNDLE(ENDPOINT, BUNDLE)																		\
{																											\
	EndPoint& ep = ENDPOINT;																				\
	SEND_BUNDLE_COMMON(ENDPOINT.send(pPacket->data() + pPacket->sentSize,									\
	pPacket->length() - pPacket->sentSize), BUNDLE);														\
}																											\


#define SENDTO_BUNDLE(ENDPOINT, ADDR, PORT, BUNDLE)															\
{																											\
	EndPoint& ep = ENDPOINT;																				\
	SEND_BUNDLE_COMMON(ENDPOINT.sendto(pPacket->data() + pPacket->sentSize,									\
	pPacket->length() - pPacket->sentSize, PORT, ADDR), BUNDLE);											\
}																											\

#define MALLOC_PACKET(outputPacket, isTCPPacket)															\
{																											\
	if(isTCPPacket)																							\
		outputPacket = TCPPacket::createPoolObject(OBJECTPOOL_POINT);										\
	else																									\
		outputPacket = UDPPacket::createPoolObject(OBJECTPOOL_POINT);										\
}																											\


#define RECLAIM_PACKET(isTCPPacket, pPacket)																\
{																											\
	if(isTCPPacket)																							\
		TCPPacket::reclaimPoolObject(static_cast<TCPPacket*>(pPacket));										\
	else																									\
		UDPPacket::reclaimPoolObject(static_cast<UDPPacket*>(pPacket));										\
}																											\


// Used with the server configuration option trace_packet to track a message packet to be output
#define TRACE_MESSAGE_PACKET(isrecv, pPacket, pCurrMsgHandler, length, addr, readPacketHead)				\
	if(Network::g_trace_packet > 0)																			\
	{																										\
		if(Network::g_trace_packet_use_logfile)																\
			DebugHelper::getSingleton().changeLogger("packetlogs");											\
																											\
		size_t headsize = 0;																				\
		if(pCurrMsgHandler && readPacketHead)																\
		{																									\
			headsize = NETWORK_MESSAGE_ID_SIZE;																\
			if (pCurrMsgHandler->msgLen == NETWORK_VARIABLE_MESSAGE)										\
			{																								\
				headsize += NETWORK_MESSAGE_LENGTH_SIZE;													\
				if (length >= NETWORK_MESSAGE_MAX_SIZE)														\
					headsize += NETWORK_MESSAGE_LENGTH1_SIZE;												\
			}																								\
		}																									\
																											\
		bool isprint = true;																				\
		if(pCurrMsgHandler)																					\
		{																									\
			std::vector<std::string>::iterator iter = std::find(Network::g_trace_packet_disables.begin(),	\
														Network::g_trace_packet_disables.end(),				\
															pCurrMsgHandler->name);							\
																											\
			if(iter != Network::g_trace_packet_disables.end())												\
			{																								\
				isprint = false;																			\
			}																								\
			else																							\
			{																								\
				DEBUG_MSG(fmt::format("{} {}:msgID:{}, currMsgLength:{}, addr:{}\n",						\
						((isrecv == true) ? "====>" : "<===="),												\
						pCurrMsgHandler->name.c_str(),														\
						pCurrMsgHandler->msgID,																\
						(length + headsize),																\
						addr));																				\
			}																								\
		}																									\
																											\
		if(isprint)																							\
		{																									\
																											\
			size_t rpos = pPacket->rpos();																	\
			if(headsize > 0)																				\
				pPacket->rpos(pPacket->rpos() - headsize);													\
																											\
			switch(Network::g_trace_packet)																	\
			{																								\
			case 1:																							\
				pPacket->hexlike();																			\
				break;																						\
			case 2:																							\
				pPacket->textlike();																		\
				break;																						\
			default:																						\
				pPacket->print_storage();																	\
				break;																						\
			};																								\
																											\
			pPacket->rpos(rpos);																			\
		}																									\
																											\
		if(Network::g_trace_packet_use_logfile)																\
			DebugHelper::getSingleton().changeLogger(COMPONENT_NAME_EX(g_componentType));					\
	}																										\


bool ouro_poll(int fd);

void destroyObjPool();

// network stats
extern uint64						g_numPacketsSent;
extern uint64						g_numPacketsReceived;
extern uint64						g_numBytesSent;
extern uint64						g_numBytesReceived;

// packet receiving window overflow
extern uint32						g_receiveWindowMessagesOverflowCritical;
extern uint32						g_intReceiveWindowMessagesOverflow;
extern uint32						g_extReceiveWindowMessagesOverflow;
extern uint32						g_intReceiveWindowBytesOverflow;
extern uint32						g_extReceiveWindowBytesOverflow;

extern uint32						g_sendWindowMessagesOverflowCritical;
extern uint32						g_intSendWindowMessagesOverflow;
extern uint32						g_extSendWindowMessagesOverflow;
extern uint32						g_intSendWindowBytesOverflow;
extern uint32						g_extSendWindowBytesOverflow;
extern uint32						g_intSentWindowBytesOverflow;
extern uint32						g_extSentWindowBytesOverflow;

bool initializeWatcher();
bool initialize();
void finalise(void);

}
}

#endif // OURO_NETWORK_COMMON_H
