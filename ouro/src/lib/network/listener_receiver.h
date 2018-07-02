// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_NETWORKLISTENER_RECEIVER_H
#define OURO_NETWORKLISTENER_RECEIVER_H

#include "common/common.h"
#include "common/timer.h"
#include "helper/debug_helper.h"
#include "network/common.h"
#include "network/interfaces.h"
#include "network/packet.h"
#include "network/channel.h"

namespace Ouroboros { 
namespace Network
{
class EndPoint;
class Address;
class NetworkInterface;
class EventDispatcher;

class ListenerReceiver : public InputNotificationHandler
{
public:
	ListenerReceiver(EndPoint & endpoint, Channel::Traits traits, NetworkInterface & networkInterface);
	virtual ~ListenerReceiver();

protected:
	virtual int handleInputNotification(int fd) = 0;
	EventDispatcher & dispatcher();

protected:
	EndPoint & endpoint_;
	Channel::Traits traits_;
	NetworkInterface & networkInterface_;
};

}
}

#ifdef CODE_INLINE
#include "listener_receiver.inl"
#endif
#endif // OURO_NETWORKLISTENER_RECEIVER_H
