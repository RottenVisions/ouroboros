// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "OUROCommon.h"
#include "PacketSenderBase.h"

namespace Ouroboros
{

class NetworkInterfaceBase;
class MemoryStream;

/*
	Packet sending module (corresponding to the name of the server network part)
	Handling the transmission of network data
*/
class OUROBOROSPLUGINS_API PacketSenderKCP : public PacketSenderBase
{
public:
	PacketSenderKCP(NetworkInterfaceBase* pNetworkInterface);
	virtual ~PacketSenderKCP();

public:
	virtual bool send(MemoryStream* pMemoryStream);

protected:

};

}