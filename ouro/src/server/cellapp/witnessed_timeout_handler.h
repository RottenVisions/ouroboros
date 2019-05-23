// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_WITNESSED_TIMEOUT_HANDLER_H
#define OURO_WITNESSED_TIMEOUT_HANDLER_H

// common include
#include "helper/debug_helper.h"
#include "common/common.h"
// #define NDEBUG
// windows include	
#if OURO_PLATFORM == PLATFORM_WIN32
#else
// linux include
#endif

namespace Ouroboros{

class WitnessedTimeoutHandler : public TimerHandler
{	
public:	
	WitnessedTimeoutHandler();

	~WitnessedTimeoutHandler();

	void addWitnessed(Entity* pEntity);
	void delWitnessed(Entity* pEntity);

private:
	virtual void handleTimeout(TimerHandle handle, void * pUser);

	virtual void onRelease( TimerHandle handle, void * /*pUser*/ ){};

	void cancel();

	std::map<ENTITY_ID, uint16>		witnessedEntityIDs_;
	TimerHandle* pTimerHandle_;
};	


}

#endif // OURO_WITNESSED_TIMEOUT_HANDLER_H
