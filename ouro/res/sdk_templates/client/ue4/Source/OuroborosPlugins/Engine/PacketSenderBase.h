// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "KBECommon.h"

namespace Ouroboros
{

class NetworkInterfaceBase;
class MemoryStream;

/*
	Packet sending module (corresponding to the name of the server network part)
	Handling the transmission of network data
*/
class OUROBOROSPLUGINS_API PacketSenderBase
{
public:
	PacketSenderBase(NetworkInterfaceBase* pNetworkInterface);
	virtual ~PacketSenderBase();

public:
	virtual bool send(MemoryStream* pMemoryStream);

protected:
	NetworkInterfaceBase * pNetworkInterface_;
};

}