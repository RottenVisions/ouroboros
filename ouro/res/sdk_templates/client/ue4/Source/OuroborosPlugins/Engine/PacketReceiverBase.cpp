
#include "PacketReceiverBase.h"
#include "Ouroboros.h"
#include "NetworkInterfaceBase.h"
#include "MessageReader.h"
#include "KBDebug.h"
#include "MemoryStream.h"

namespace Ouroboros
{

PacketReceiverBase::PacketReceiverBase(NetworkInterfaceBase* pNetworkInterface):
	pNetworkInterface_(pNetworkInterface),
	pMessageReader_(new MessageReader()),
	pBuffer_(new MemoryStream())
{
}

PacketReceiverBase::~PacketReceiverBase()
{
	OURO_SAFE_RELEASE(pMessageReader_);
	OURO_SAFE_RELEASE(pBuffer_);
	
	INFO_MSG("PacketReceiverBase::~PacketReceiverBase(), destroyed!");
}

void PacketReceiverBase::process()
{
}

}
