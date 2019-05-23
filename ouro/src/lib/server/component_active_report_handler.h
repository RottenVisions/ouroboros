// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_COMPONENT_ACTIVE_REPORT_HANDLER_H
#define OURO_COMPONENT_ACTIVE_REPORT_HANDLER_H

#include "common/common.h"
#include "common/tasks.h"
#include "common/timer.h"
#include "helper/debug_helper.h"

namespace Ouroboros { 

class ServerApp;

class ComponentActiveReportHandler : public TimerHandler
{
public:
	enum TimeOutType
	{
		TIMEOUT_ACTIVE_TICK,
		TIMEOUT_MAX
	};
	
	ComponentActiveReportHandler(ServerApp* pApp);
	virtual ~ComponentActiveReportHandler();
	
	void startActiveTick(float period);

	void cancel();

protected:
	virtual void handleTimeout(TimerHandle handle, void * arg);

	ServerApp* pApp_;
	TimerHandle pActiveTimerHandle_;

};

}

#endif // OURO_COMPONENT_ACTIVE_REPORT_HANDLER_H
