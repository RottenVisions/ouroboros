// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_NETWORK_INTERFACES_H
#define OURO_NETWORK_INTERFACES_H

namespace Ouroboros {
namespace Network
{
class Channel;
class MessageHandler;

/** This type of interface is used to receive normal Network input messages
*/
class InputNotificationHandler
{
public:
	virtual ~InputNotificationHandler() {};
	virtual int handleInputNotification(int fd) = 0;
};

/** This type of interface is used to receive normal Network output messages
*/
class OutputNotificationHandler
{
public:
	virtual ~OutputNotificationHandler() {};
	virtual int handleOutputNotification(int fd) = 0;
};

/** This type of interface is used to receive a network channel timeout message
*/
class ChannelTimeOutHandler
{
public:
	virtual void onChannelTimeOut(Channel * pChannel) = 0;
};

/** This type of interface is used to receive an internal network channel deregistration
*/
class ChannelDeregisterHandler
{
public:
	virtual void onChannelDeregister(Channel * pChannel) = 0;
};

/** This type of interface is used to listen for NetworkStats events
*/
class NetworkStatsHandler
{
public:
	virtual void onSendMessage(const MessageHandler& msgHandler, int size) = 0;
	virtual void onRecvMessage(const MessageHandler& msgHandler, int size) = 0;
};


}
}

#endif // OURO_NETWORK_INTERFACES_H
