// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_CREATE_AND_LOGIN_HANDLER_H
#define OURO_CREATE_AND_LOGIN_HANDLER_H

#include "common/common.h"
#include "common/tasks.h"
#include "common/timer.h"
#include "helper/debug_helper.h"

namespace Ouroboros { 

class CreateAndLoginHandler : public TimerHandler
{
public:
	CreateAndLoginHandler();
	virtual ~CreateAndLoginHandler();

protected:
	virtual void handleTimeout(TimerHandle handle, void * arg);

	TimerHandle timerHandle_;

};



}

#endif // OURO_CREATE_AND_LOGIN_HANDLER_H
