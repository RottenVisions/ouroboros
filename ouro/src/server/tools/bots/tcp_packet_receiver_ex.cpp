// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com


#include "tcp_packet_receiver_ex.h"
#include "clientobject.h"
#include "bots.h"

#include "network/address.h"
#include "network/bundle.h"
#include "network/channel.h"
#include "network/endpoint.h"
#include "network/event_dispatcher.h"
#include "network/network_interface.h"
#include "network/event_poller.h"
#include "network/error_reporter.h"

namespace Ouroboros { 
namespace Network
{

//-------------------------------------------------------------------------------------
TCPPacketReceiverEx::TCPPacketReceiverEx(EndPoint & endpoint,
	   NetworkInterface & networkInterface, ClientObject* pClientObject) :
	TCPPacketReceiver(endpoint, networkInterface),
	pClientObject_(pClientObject)
{
}

//-------------------------------------------------------------------------------------
TCPPacketReceiverEx::~TCPPacketReceiverEx()
{
	//DEBUG_MSG("TCPPacketReceiverEx::~TCPPacketReceiverEx()\n");
}

//-------------------------------------------------------------------------------------
Channel* TCPPacketReceiverEx::getChannel()
{
	return pClientObject_->pServerChannel();
}

//-------------------------------------------------------------------------------------
void TCPPacketReceiverEx::onGetError(Channel* pChannel, const std::string& err)
{
	pClientObject_->destroy();
}

//-------------------------------------------------------------------------------------
}
}

