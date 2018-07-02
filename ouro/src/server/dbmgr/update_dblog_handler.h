// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_UPDATE_DB_SERVER_LOG_HANDLER_H
#define OURO_UPDATE_DB_SERVER_LOG_HANDLER_H

// common include
#include "helper/debug_helper.h"
#include "common/common.h"

namespace Ouroboros{

class UpdateDBServerLogHandler : public TimerHandler
{	
public:	
	UpdateDBServerLogHandler();

	~UpdateDBServerLogHandler();

private:
	virtual void handleTimeout(TimerHandle handle, void * pUser);

	virtual void onRelease( TimerHandle handle, void * /*pUser*/ ){};

	void cancel();
	
	TimerHandle* pTimerHandle_;
};	


}

#endif // OURO_UPDATE_DB_SERVER_LOG_HANDLER_H
