
#include "PacketSenderBase.h"
#include "MemoryStream.h"
#include "KBDebug.h"
#include "NetworkInterfaceBase.h"

namespace Ouroboros
{

PacketSenderBase::PacketSenderBase(NetworkInterfaceBase* pNetworkInterface) :
	pNetworkInterface_(pNetworkInterface)
{
}

PacketSenderBase::~PacketSenderBase()
{
}

bool PacketSenderBase::send(MemoryStream* pMemoryStream)
{
	return true;
}

}