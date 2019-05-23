// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#include "timer.h"
#include "helper/debug_helper.h"
#include "thread/threadguard.h"

namespace Ouroboros
{
//-------------------------------------------------------------------------------------
void TimerHandle::cancel()
{
	if (pTime_ != NULL)
	{
		TimeBase* pTime = pTime_;
		pTime_ = NULL;
		pTime->cancel();
	}
}

//-------------------------------------------------------------------------------------
} 
