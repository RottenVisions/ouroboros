// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_BUNDLE_BROADCAST_H
#define OURO_BUNDLE_BROADCAST_H

#include "common/common.h"
#include "helper/debug_helper.h"
#include "network/common.h"
#include "network/interfaces.h"
#include "message_handler.h"
#include "network/bundle.h"
#include "network/endpoint.h"

namespace Ouroboros { 
namespace Network
{
class NetworkInterface;

/*
	It can be conveniently handled, such as: broadcasting certain information to the local area network, and processing related information.
*/
class BundleBroadcast : public Bundle
{
public:
	BundleBroadcast(NetworkInterface & networkInterface, uint16 bindPort = OURO_PORT_BROADCAST_DISCOVERY, 
		uint32 recvWindowSize = PACKET_MAX_SIZE_UDP);
	virtual ~BundleBroadcast();

	EventDispatcher& dispatcher();
	
	bool broadcast(uint16 port = 0);
	bool receive(MessageArgs* recvArgs, sockaddr_in* psin = NULL, int32 timeout = 100000, bool showerr = true);

	Network::EndPoint& epListen() { 
		return epListen_; 
	}

	void close();

	bool good() const { 
		return epListen_.good() && good_; 
	}

	void itry(int8 i){ 
		itry_ = i; 
	}

	void addBroadCastAddress(std::string addr);

protected:
	Network::EndPoint epListen_, epBroadcast_;
	NetworkInterface & networkInterface_;
	uint32 recvWindowSize_;
	bool good_;
	int8 itry_;
	std::vector< std::string > machine_addresses_;
};

}
}

#ifdef CODE_INLINE
#include "bundle_broadcast.inl"
#endif
#endif // OURO_BUNDLE_BROADCAST_H
