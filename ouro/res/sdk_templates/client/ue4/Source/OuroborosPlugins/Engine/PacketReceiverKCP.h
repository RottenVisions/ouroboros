// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "OUROCommon.h"
#include "PacketReceiverBase.h"
#include "Runtime/Sockets/Public/SocketSubsystem.h"

namespace Ouroboros
{

class NetworkInterfaceBase;
class MessageReader;
class MemoryStream;

/*
	Packet receiving module (corresponding to the name of the server network part)
	Handling the reception of network data
*/
class OUROBOROSPLUGINS_API PacketReceiverKCP : public PacketReceiverBase
{
public:
	PacketReceiverKCP(NetworkInterfaceBase* pNetworkInterface);
	virtual ~PacketReceiverKCP();

public:
	virtual void process();

protected:
	TSharedRef<FInternetAddr> remoteAddr_;
};

}
