// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_MESSAGE_HANDLER_H
#define OURO_MESSAGE_HANDLER_H

#include "common/memorystream.h"
#include "common/smartpointer.h"
#include "helper/debug_helper.h"
#include "network/common.h"

namespace Ouroboros{

class OURO_MD5;

namespace Network
{

class Channel;
class MessageHandlers;

/** Parameter abstract class for a message*/
class MessageArgs
{
public:
	enum MESSAGE_ARGS_TYPE
	{
		MESSAGE_ARGS_TYPE_VARIABLE = -1, // variable parameter length
		MESSAGE_ARGS_TYPE_FIXED = 0 // fixed parameter length
	};

	MessageArgs():strArgsTypes(){};
	virtual ~MessageArgs(){};
	virtual void createFromStream(MemoryStream& s) = 0;
	virtual void addToStream(MemoryStream& s) = 0;
	virtual int32 dataSize(void) = 0;
	virtual MessageArgs::MESSAGE_ARGS_TYPE type(void){ return MESSAGE_ARGS_TYPE_FIXED; }

	std::vector<std::string> strArgsTypes;
};

struct ExposedMessageInfo
{
	std::string name;
	Network::MessageID id;
	int16 msgLen; // External message will not exceed 1500
	int8 argsType;
	std::vector<uint8> argsTypes;
};

class MessageHandler
{
public:
	MessageHandler();
	virtual ~MessageHandler();

	std::string name;
	MessageID msgID;
	MessageArgs* pArgs;
	int32 msgLen; // if the length is -1, it is a non-fixed length message
	bool exposed;
	MessageHandlers* pMessageHandlers;

	// stats
	volatile mutable uint32 send_size;
	volatile mutable uint32 send_count;
	volatile mutable uint32 recv_size;
	volatile mutable uint32 recv_count;

	uint32 sendsize() const  { return send_size; }
	uint32 sendcount() const  { return send_count; }
	uint32 sendavgsize() const  { return (send_count <= 0) ? 0 : send_size / send_count; }

	uint32 recvsize() const  { return recv_size; }
	uint32 recvcount() const  { return recv_count; }
	uint32 recvavgsize() const  { return (recv_count <= 0) ? 0 : recv_size / recv_count; }

	/**
		The default return category is component message
	*/
	virtual NETWORK_MESSAGE_TYPE type() const
	{ 
		return NETWORK_MESSAGE_TYPE_COMPONENT; 
	}

	virtual int32 msglenMax(){ return NETWORK_MESSAGE_MAX_SIZE; }

	const char* c_str();

	/**
		When this handler is called after it is installed into MessageHandlers
	*/
	virtual void onInstall(){}

	virtual void handle(Channel* pChannel, MemoryStream& s)
	{
		pArgs->createFromStream(s);
		
		// Pass the parameters to the final interface
	};
};

class MessageHandlers
{
public:
	static Network::MessageHandlers* pMainMessageHandlers;
	typedef std::map<MessageID, MessageHandler*> MessageHandlerMap;
	MessageHandlers(const std::string& name);
	~MessageHandlers();
	
	MessageHandler* add(std::string ihName, MessageArgs* args, int32 msgLen, 
						MessageHandler* msgHandler);
	
	bool pushExposedMessage(std::string msgname);

	MessageHandler* find(MessageID msgID);
	
	MessageID lastMsgID() {return msgID_ - 1;}

	bool initializeWatcher();
	
	static void finalise(void);
	static std::vector<MessageHandlers*>& messageHandlers();

	const MessageHandlerMap& msgHandlers(){ return msgHandlers_; }

	static std::string getDigestStr();

	std::string name() const {
		return name_;
	}

private:
	MessageHandlerMap msgHandlers_;
	MessageID msgID_;

	std::vector< std::string > exposedMessages_;
	std::string name_;
};

}
}
#endif 
