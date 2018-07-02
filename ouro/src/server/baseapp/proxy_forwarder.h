// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_PROXY_FORWARDER_H
#define OURO_PROXY_FORWARDER_H

#include "helper/debug_helper.h"
#include "common/common.h"

namespace Ouroboros{

class ProxyForwarder : public TimerHandler
{
public:
	ProxyForwarder(Proxy * pProxy);
	virtual ~ProxyForwarder();

protected:
	virtual void handleTimeout(TimerHandle, void * arg);
	virtual void onRelease(TimerHandle handle, void  * pUser) {}

protected:
	Proxy * pProxy_;
	TimerHandle timerHandle_;
};

}

#endif // OURO_PROXY_FORWARDER_H
