// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "KBECommon.h"

#include "Runtime/Sockets/Public/SocketSubsystem.h"
#include "Runtime/Core/Public/Templates/SharedPointer.h"
#include "Runtime/Networking/Public/Networking.h"
#include "Runtime/Sockets/Public/Sockets.h"
#include "NetworkInterfaceBase.h"

/*
	Network module
	Handling connections, sending and receiving data
*/
namespace Ouroboros
{

class OUROBOROSPLUGINS_API NetworkInterfaceTCP : public NetworkInterfaceBase
{
public:
	NetworkInterfaceTCP();
	virtual ~NetworkInterfaceTCP();
	
protected:
	PacketSenderBase* createPacketSender() override;
	PacketReceiverBase* createPacketReceiver() override;

};

}
