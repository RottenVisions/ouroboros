
#include "OuroborosArgs.h"
#include "KBDebug.h"

namespace Ouroboros
{

OuroborosArgs::OuroborosArgs():
	ip(TEXT("127.0.0.1")),
	port(20013),
	clientType(EKCLIENT_TYPE::CLIENT_TYPE_WIN),
	networkEncryptType(NETWORK_ENCRYPT_TYPE::ENCRYPT_TYPE_NONE),
	syncPlayerMS(100),
	useAliasEntityID(true),
	isOnInitCallPropertysSetMethods(true),
	forceDisableUDP(false),
	serverHeartbeatTick(15),
	TCP_SEND_BUFFER_MAX(TCP_PACKET_MAX),
	UDP_SEND_BUFFER_MAX(128),
	TCP_RECV_BUFFER_MAX(TCP_PACKET_MAX),
	UDP_RECV_BUFFER_MAX(128)
{
}

OuroborosArgs::~OuroborosArgs()
{
}

int OuroborosArgs::getTCPRecvBufferSize()
{
	return (int)TCP_RECV_BUFFER_MAX;
}

int OuroborosArgs::getTCPSendBufferSize()
{
	return (int)TCP_SEND_BUFFER_MAX;
}

int OuroborosArgs::getUDPRecvBufferSize()
{
	return (int)UDP_RECV_BUFFER_MAX;
}

int OuroborosArgs::getUDPSendBufferSize()
{
	return (int)UDP_SEND_BUFFER_MAX;
}

}