// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com


#include "shutdowner.h"
#include "network/event_dispatcher.h"

namespace Ouroboros { 


//-------------------------------------------------------------------------------------
Shutdowner::Shutdowner(ShutdownHandler* pShutdownHandler) :
	pShutdownHandler_(pShutdownHandler),
	pTimerHandle_(),
	pDispatcher_(0),
	tickPeriod_(1.0f)
{
}

//-------------------------------------------------------------------------------------
Shutdowner::~Shutdowner()
{
	cancel();
}

//-------------------------------------------------------------------------------------
void Shutdowner::cancel()
{
	pTimerHandle_.cancel();
}

//-------------------------------------------------------------------------------------
void Shutdowner::shutdown(float period, float tickPeriod, Network::EventDispatcher& dispatcher)
{
	pTimerHandle_.cancel();
	INFO_MSG(fmt::format("Shutdowner::onShutdownBegin: shutting down(period={}, tickPeriod={})\n", 
		period, tickPeriod));
	
	tickPeriod_ = tickPeriod;
	pShutdownHandler_->setShuttingdown(ShutdownHandler::SHUTDOWN_STATE_BEGIN);
	
	if(period <= 0.f)
	{
		pShutdownHandler_->onShutdownBegin();
		
		INFO_MSG( "Shutdowner::onShutdown: shutting down\n" );
		pShutdownHandler_->setShuttingdown(ShutdownHandler::SHUTDOWN_STATE_RUNNING);
		pShutdownHandler_->onShutdown(true);
		
		INFO_MSG( "Shutdowner::onShutdownEnd: shutting down\n" );
		pShutdownHandler_->setShuttingdown(ShutdownHandler::SHUTDOWN_STATE_END);
		pShutdownHandler_->onShutdownEnd();
		return;	
	}
	
	pTimerHandle_ = dispatcher.addTimer(int(period * 1000000),
									this, (void *)TIMEOUT_SHUTDOWN_TICK);
	
	pDispatcher_ = &dispatcher;
	pShutdownHandler_->onShutdownBegin();
}

//-------------------------------------------------------------------------------------
void Shutdowner::handleTimeout(TimerHandle handle, void * arg)
{
	switch (reinterpret_cast<uintptr>(arg))
	{
		case TIMEOUT_SHUTDOWN_TICK:
		{
			INFO_MSG( "Shutdowner::onShutdown: shutting down\n" );
			pShutdownHandler_->setShuttingdown(ShutdownHandler::SHUTDOWN_STATE_RUNNING);
			pShutdownHandler_->onShutdown(true);
			cancel();
			
			pTimerHandle_ = pDispatcher_->addTimer(int(tickPeriod_ * 1000000),
											this, (void *)TIMEOUT_SHUTDOWN_END_TICK);
	
			break;
		}
		case TIMEOUT_SHUTDOWN_END_TICK:
			pShutdownHandler_->setShuttingdown(ShutdownHandler::SHUTDOWN_STATE_END);
			if(!pShutdownHandler_->canShutdown())
			{
				//INFO_MSG(fmt::format("Shutdowner::onShutdownEnd: waiting for {} to complete!\n",
				//	pShutdownHandler_->lastShutdownFailReason()));
				
				pShutdownHandler_->onShutdown(false);
				
				cancel();
				
				pTimerHandle_ = pDispatcher_->addTimer(int(100000),
												this, (void *)TIMEOUT_SHUTDOWN_END_TICK);
			
				break;
			}
			
			INFO_MSG( "Shutdowner::onShutdownEnd: shutting down\n" );

			cancel();
			pShutdownHandler_->onShutdownEnd();
			break;
		default:
			break;
	}
}

//-------------------------------------------------------------------------------------

}
