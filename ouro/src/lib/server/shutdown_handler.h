// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_SHUTDOWN_HANDLER_H
#define OURO_SHUTDOWN_HANDLER_H

#include "common/common.h"
#include "common/timer.h"
#include "helper/debug_helper.h"

namespace Ouroboros { 

class ShutdownHandler 
{
public:
	enum SHUTDOWN_STATE
	{
		SHUTDOWN_STATE_STOP = COMPONENT_STATE_RUN,
		SHUTDOWN_STATE_BEGIN = COMPONENT_STATE_SHUTTINGDOWN_BEGIN,
		SHUTDOWN_STATE_RUNNING = COMPONENT_STATE_SHUTTINGDOWN_RUNNING,
		SHUTDOWN_STATE_END = COMPONENT_STATE_STOP
	};

	enum CAN_SHUTDOWN_STATE
	{
		CAN_SHUTDOWN_STATE_USER_FALSE = 0,
		CAN_SHUTDOWN_STATE_USER_TRUE = 1,
		CAN_SHUTDOWN_STATE_FALSE = 2,
		CAN_SHUTDOWN_STATE_TRUE = 3
	};

	ShutdownHandler():lastShutdownFailReason_("tasks"),
	shuttingdown_(SHUTDOWN_STATE_STOP){
	}
	
	virtual ~ShutdownHandler(){}
	
	virtual void onShutdownBegin() = 0;
	virtual void onShutdown(bool first) = 0;
	virtual void onShutdownEnd() = 0;
	
	virtual CAN_SHUTDOWN_STATE canShutdown() { return CAN_SHUTDOWN_STATE_TRUE; }
	
	void setShuttingdown(SHUTDOWN_STATE state){ shuttingdown_ = state; }
	bool isShuttingdown() const{ return shuttingdown_ != SHUTDOWN_STATE_STOP; }
	SHUTDOWN_STATE shuttingdown() const{ return shuttingdown_; }
	const std::string& lastShutdownFailReason(){ return lastShutdownFailReason_; }

protected:
	std::string lastShutdownFailReason_; // The reason for the last shutdown failure
	SHUTDOWN_STATE shuttingdown_;
};

}

#endif // OURO_SHUTDOWN_HANDLER_H
