// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

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

	ShutdownHandler():lastShutdownFailReason_("tasks"),
	shuttingdown_(SHUTDOWN_STATE_STOP){
	}

	virtual ~ShutdownHandler(){}

	virtual void onShutdownBegin() = 0;
	virtual void onShutdown(bool first) = 0;
	virtual void onShutdownEnd() = 0;

	virtual bool canShutdown(){ return true; }

	void setShuttingdown(SHUTDOWN_STATE state){ shuttingdown_ = state; }
	bool isShuttingdown() const{ return shuttingdown_ != SHUTDOWN_STATE_STOP; }
	SHUTDOWN_STATE shuttingdown() const{ return shuttingdown_; }
	const std::string& lastShutdownFailReason(){ return lastShutdownFailReason_; }

protected:
	std::string lastShutdownFailReason_; // Reason for the last shutdown failure
	SHUTDOWN_STATE shuttingdown_;
};

}

#endif // OURO_SHUTDOWN_HANDLER_H
