// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "KBECommon.h"
#include "PacketReceiverBase.h"

namespace Ouroboros
{

class NetworkInterfaceBase;
class MessageReader;
class MemoryStream;

/*
	Packet receiving module (corresponding to the name of the server network part)
	Handling the reception of network data
*/
class OUROBOROSPLUGINS_API PacketReceiverTCP : public PacketReceiverBase
{
public:
	PacketReceiverTCP(NetworkInterfaceBase* pNetworkInterface);
	virtual ~PacketReceiverTCP();

public:
	virtual void process();

protected:

};

}