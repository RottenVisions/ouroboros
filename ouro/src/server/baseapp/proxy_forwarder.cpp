// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#include "baseapp.h"
#include "proxy.h"
#include "proxy_forwarder.h"
#include "server/serverconfig.h"

namespace Ouroboros{	

//-------------------------------------------------------------------------------------
ProxyForwarder::ProxyForwarder(Proxy * pProxy) : 
pProxy_(pProxy)
{
	timerHandle_ = Baseapp::getSingleton().dispatcher().addTimer(1000000 / g_ouroSrvConfig.gameUpdateHertz(), this,
							NULL);
}

//-------------------------------------------------------------------------------------
ProxyForwarder::~ProxyForwarder()
{
	timerHandle_.cancel();
}

//-------------------------------------------------------------------------------------
void ProxyForwarder::handleTimeout(TimerHandle, void * arg)
{
	pProxy_->sendToClient(false);
}


}
