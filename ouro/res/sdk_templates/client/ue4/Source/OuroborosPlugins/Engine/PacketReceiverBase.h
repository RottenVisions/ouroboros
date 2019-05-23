// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "KBECommon.h"

namespace Ouroboros
{

class NetworkInterfaceBase;
class MessageReader;
class MemoryStream;

/*
	Packet receiving module (corresponding to the name of the server network part)
	Handling the reception of network data
*/
class OUROBOROSPLUGINS_API PacketReceiverBase
{
public:
	PacketReceiverBase(NetworkInterfaceBase* pNetworkInterface);
	virtual ~PacketReceiverBase();

public:
	virtual void process();

protected:
	NetworkInterfaceBase * pNetworkInterface_;
	MessageReader* pMessageReader_;
	MemoryStream* pBuffer_;
};

}