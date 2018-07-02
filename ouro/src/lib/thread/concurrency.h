// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com



#ifndef OURO_CONCURENCY_H
#define OURO_CONCURENCY_H

#include "common/platform.h"
#include "helper/debug_helper.h"
namespace Ouroboros{

extern void (*pMainThreadIdleStartCallback)();
extern void (*pMainThreadIdleEndCallback)();

namespace OUROConcurrency
{

/**
	Fires when the main thread is idle
*/
inline void onStartMainThreadIdling()
{
	if(pMainThreadIdleStartCallback)
		(*pMainThreadIdleStartCallback)();
}

/**
	Triggered when the main thread ends idle and starts to be busy
*/
inline void onEndMainThreadIdling()
{
	if(pMainThreadIdleEndCallback)
		(*pMainThreadIdleEndCallback)();
}

/**
	Set callback function
	Notify them when callbacks are triggered
*/
inline void setMainThreadIdleCallbacks(void (*pStartCallback)(), void (*pEndCallback)())
{
	pMainThreadIdleStartCallback = pStartCallback;
	pMainThreadIdleEndCallback = pEndCallback;
}

}

}

#endif // OURO_CONCURENCY_H
