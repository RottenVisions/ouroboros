// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_NETWORKTCPLISTENER_RECEIVER_H
#define OURO_NETWORKTCPLISTENER_RECEIVER_H

#include "common/common.h"
#include "common/timer.h"
#include "helper/debug_helper.h"
#include "network/common.h"
#include "network/interfaces.h"
#include "network/packet.h"
#include "network/channel.h"
#include "network/listener_receiver.h"

namespace Ouroboros { 
namespace Network
{

class ListenerTcpReceiver : public ListenerReceiver
{
public:
	ListenerTcpReceiver(EndPoint & endpoint, Channel::Traits traits, NetworkInterface & networkInterface);
	virtual ~ListenerTcpReceiver();

protected:
	virtual int handleInputNotification(int fd);

protected:
};

}
}

#ifdef CODE_INLINE
#include "listener_tcp_receiver.inl"
#endif
#endif // OURO_NETWORKTCPLISTENER_RECEIVER_H
