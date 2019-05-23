// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "OUROCommon.h"


/*
Initialize the parameter class of Ouroboros
*/
namespace Ouroboros
{

class OUROBOROSPLUGINS_API OuroborosArgs
{
public:
	OuroborosArgs();
	virtual ~OuroborosArgs();

	int getTCPRecvBufferSize();
	int getTCPSendBufferSize();

	int getUDPRecvBufferSize();
	int getUDPSendBufferSize();

public:
	FString ip;
	int port;

	// client type
	// Reference: http://www.ouroboros.org/docs/programming/clientsdkprogramming.html, client types
	EKCLIENT_TYPE clientType;
	NETWORK_ENCRYPT_TYPE  networkEncryptType;

	int syncPlayerMS;
	bool useAliasEntityID;
	bool isOnInitCallPropertysSetMethods;

	// Forced to disable UDP communication
	bool forceDisableUDP;

	// heartbeat frequency (tick number)
	int serverHeartbeatTick;

	// send buffer size
	MessageLengthEx TCP_SEND_BUFFER_MAX;
	MessageLengthEx UDP_SEND_BUFFER_MAX;

	// Receive buffer size
	MessageLengthEx TCP_RECV_BUFFER_MAX;
	MessageLengthEx UDP_RECV_BUFFER_MAX;

};

}