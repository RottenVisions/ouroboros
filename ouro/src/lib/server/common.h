// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_SERVER_COMMON_H
#define OURO_SERVER_COMMON_H

#include "common/timer.h"
#include "common/common.h"
#include "server/server_errors.h"


namespace Ouroboros { 

// The message is forwarded to a component
#define NETWORK_MESSAGE_FORWARD(SEND_INTERFACE, SENDBUNDLE, FORWARDBUNDLE, MYCOMPONENT_ID, FORWARD_COMPONENT_ID)						\
	SENDBUNDLE.newMessage(SEND_INTERFACE::forwardMessage);																				\
	SENDBUNDLE << MYCOMPONENT_ID << FORWARD_COMPONENT_ID;																				\
	FORWARDBUNDLE.finiMessage(true);																									\
	SENDBUNDLE.append(FORWARDBUNDLE);																									\

// cellapp forwards the message to the client
#define NETWORK_ENTITY_MESSAGE_FORWARD_CLIENT(ENTITYID, SENDBUNDLE, FORWARDBUNDLE)														\
	SENDBUNDLE.newMessage(BaseappInterface::forwardMessageToClientFromCellapp);															\
	SENDBUNDLE << ENTITYID;																												\
	FORWARDBUNDLE.finiMessage(true);																									\
	SENDBUNDLE.append(FORWARDBUNDLE);																									\

// cellapp forwards the message to cellapp
#define NETWORK_ENTITY_MESSAGE_FORWARD_CELLAPP(ENTITYID, SENDBUNDLE, FORWARDBUNDLE)														\
	SENDBUNDLE.newMessage(BaseappInterface::forwardMessageToCellappFromCellapp);														\
	SENDBUNDLE << ENTITYID;																												\
	FORWARDBUNDLE.finiMessage(true);																									\
	SENDBUNDLE.append(FORWARDBUNDLE);	

// cellapp forwards the message to the client to start
#define NETWORK_ENTITY_MESSAGE_FORWARD_CLIENT_BEGIN(ENTITYID, SENDBUNDLE)																\
	SENDBUNDLE.newMessage(BaseappInterface::forwardMessageToClientFromCellapp);															\
	SENDBUNDLE << ENTITYID;																												\

// cellapp forwards the message to the client message packet append message
#define NETWORK_ENTITY_MESSAGE_FORWARD_CLIENT_APPEND(SENDBUNDLE, FORWARDBUNDLE)															\
	FORWARDBUNDLE.finiMessage(true);																									\
	SENDBUNDLE.append(FORWARDBUNDLE);																									\

#define ENTITY_MESSAGE_FORWARD_CLIENT_END(SENDBUNDLE, MESSAGEHANDLE, ACTIONNAME)														\
{																																		\
	size_t messageLength = SENDBUNDLE->currMsgLength() - messageLength_last_##ACTIONNAME;												\
	Network::Packet* pCurrPacket = SENDBUNDLE->pCurrPacket();																			\
																																		\
	if(MESSAGEHANDLE.msgLen == NETWORK_VARIABLE_MESSAGE)																				\
	{																																	\
		if(messageLength >= NETWORK_MESSAGE_MAX_SIZE)																					\
		{																																\
			Network::MessageLength1 ex_msg_length = messageLength;																		\
			Ouroboros::EndianConvert(ex_msg_length);																						\
																																		\
			Network::MessageLength msgLen = NETWORK_MESSAGE_MAX_SIZE;																	\
			Ouroboros::EndianConvert(msgLen);																							\
																																		\
			memcpy(&pCurrPacket_##ACTIONNAME->data()[currMsgLengthPos_##ACTIONNAME], 													\
				(uint8*)&msgLen, NETWORK_MESSAGE_LENGTH_SIZE);																			\
																																		\
			pCurrPacket_##ACTIONNAME->insert(currMsgLengthPos_##ACTIONNAME + NETWORK_MESSAGE_LENGTH_SIZE, 								\
											(uint8*)&ex_msg_length, NETWORK_MESSAGE_LENGTH1_SIZE);										\
			SENDBUNDLE->currMsgLength(SENDBUNDLE->currMsgLength() + NETWORK_MESSAGE_LENGTH1_SIZE);										\
		}																																\
		else																															\
		{																																\
			Network::MessageLength msgLen = messageLength;																				\
			Ouroboros::EndianConvert(msgLen);																							\
																																		\
			memcpy(&pCurrPacket_##ACTIONNAME->data()[currMsgLengthPos_##ACTIONNAME], 													\
				(uint8*)&msgLen, NETWORK_MESSAGE_LENGTH_SIZE);																			\
		}																																\
	}																																	\
																																		\
	Network::NetworkStats::getSingleton().trackMessage(Network::NetworkStats::SEND, MESSAGEHANDLE, messageLength);						\
																																		\
	if (Network::g_trace_packet > 0)																									\
		Network::Bundle::debugCurrentMessages(MESSAGEHANDLE.msgID, &MESSAGEHANDLE, 														\
				pCurrPacket, SENDBUNDLE->packets(), messageLength, SENDBUNDLE->pChannel());												\
}																																		\


// cellapp forwards the message to the client message packet append message (added directly in SENDBUNDLE)
#define ENTITY_MESSAGE_FORWARD_CLIENT_BEGIN(SENDBUNDLE, MESSAGEHANDLE, ACTIONNAME)														\
	(*SENDBUNDLE) << MESSAGEHANDLE.msgID;																								\
	size_t currMsgLengthPos_##ACTIONNAME = 0;																							\
	Network::Packet* pCurrPacket_##ACTIONNAME = SENDBUNDLE->pCurrPacket();																\
	if(MESSAGEHANDLE.msgLen == NETWORK_VARIABLE_MESSAGE)																				\
	{																																	\
		if(SENDBUNDLE->packetMaxSize() - pCurrPacket_##ACTIONNAME->wpos() - 1 < NETWORK_MESSAGE_LENGTH_SIZE)							\
		{																																\
			SENDBUNDLE->finiCurrPacket();																								\
			SENDBUNDLE->newPacket();																									\
			pCurrPacket_##ACTIONNAME = SENDBUNDLE->pCurrPacket();																		\
		}																																\
																																		\
		Network::MessageLength msglen = 0;																								\
		currMsgLengthPos_##ACTIONNAME = pCurrPacket_##ACTIONNAME->wpos();																\
		(*SENDBUNDLE) << msglen;																										\
	}																																	\
																																		\
	size_t messageLength_last_##ACTIONNAME = SENDBUNDLE->currMsgLength();																\


// public message
#define COMMON_NETWORK_MESSAGE(COMPONENTTYPE, BUNDLE, MESSAGENAME)											\
		switch(COMPONENTTYPE)																				\
		{																									\
		case CELLAPPMGR_TYPE:																				\
			{																								\
				BUNDLE.newMessage(CellappmgrInterface::MESSAGENAME);										\
			}																								\
			break;																							\
		case BASEAPPMGR_TYPE:																				\
			{																								\
				BUNDLE.newMessage(BaseappmgrInterface::MESSAGENAME);										\
			}																								\
			break;																							\
		case DBMGR_TYPE:																					\
			{																								\
				BUNDLE.newMessage(DbmgrInterface::MESSAGENAME);												\
			}																								\
			break;																							\
		case CELLAPP_TYPE:																					\
			{																								\
				BUNDLE.newMessage(CellappInterface::MESSAGENAME);											\
			}																								\
			break;																							\
		case BASEAPP_TYPE:																					\
			{																								\
				BUNDLE.newMessage(BaseappInterface::MESSAGENAME);											\
			}																								\
			break;																							\
		case LOGINAPP_TYPE:																					\
			{																								\
				BUNDLE.newMessage(LoginappInterface::MESSAGENAME);											\
			}																								\
			break;																							\
		case LOGGER_TYPE:																					\
			{																								\
				BUNDLE.newMessage(LoggerInterface::MESSAGENAME);											\
			}																								\
			break;																							\
		case INTERFACES_TYPE:																				\
			{																								\
				BUNDLE.newMessage(InterfacesInterface::MESSAGENAME);										\
			}																								\
			break;																							\
		default:																							\
			ERROR_MSG(fmt::format("not support componentType={}({})\n",										\
					COMPONENTTYPE, COMPONENT_NAME_EX(COMPONENTTYPE)));										\
			OURO_ASSERT(false && "not support componentType!\n");											\
			break;																							\
		};																									\


#define ENTITTAPP_COMMON_NETWORK_MESSAGE(COMPONENTTYPE, BUNDLE, MESSAGENAME)								\
		switch(COMPONENTTYPE)																				\
		{																									\
		case CELLAPP_TYPE:																					\
			{																								\
				BUNDLE.newMessage(CellappInterface::MESSAGENAME);											\
			}																								\
			break;																							\
		case BASEAPP_TYPE:																					\
			{																								\
				BUNDLE.newMessage(BaseappInterface::MESSAGENAME);											\
			}																								\
			break;																							\
		default:																							\
			ERROR_MSG(fmt::format("not support componentType={}({})\n",										\
					COMPONENTTYPE, COMPONENT_NAME_EX(COMPONENTTYPE)));										\
			OURO_ASSERT(false && "not support componentType!\n");											\
			break;																							\
		};																									\

/**
Convert seconds to tick
@lowerBound: at least no less than Ntick
*/
int32 secondsToTicks(float seconds, int lowerBound);

/**
	Converts the time in seconds to the stamps consumed per second
*/
inline uint64 secondsToStamps(float seconds)
{
	return (uint64)(seconds * stampsPerSecondD());
}

void autoFixUserDigestUID();

/*
 ?Maximum length of account and password
*/
#define ACCOUNT_NAME_MAX_LENGTH						128
#define ACCOUNT_PASSWD_MAX_LENGTH					255

// The maximum length of information attached to the login
#define ACCOUNT_DATA_MAX_LENGTH						1024

// is used to describe any option that is automatically set to not do after only one time.
#define OURO_NEXT_ONLY								2

/** c/c++ data category converted to KBEDataTypeID*/
#define OURO_DATATYPE2ID_MAX							21
uint16 datatype2id(std::string datatype);

/** c/c++ data categories are converted to native categories UINT16 ...*/
std::string datatype2nativetype(std::string datatype);
std::string datatype2nativetype(uint16 datatype);

int getMacMD5();
int getMD5(std::string data);

}

#endif // OURO_SERVER_COMMON_H
