// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com



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
	Triggered when the main thread is idle
*/
inline void onStartMainThreadIdling()
{
	if(pMainThreadIdleStartCallback)
		(*pMainThreadIdleStartCallback)();
}

/**
	Triggered when the main thread ends idle and starts busy
*/
inline void onEndMainThreadIdling()
{
	if(pMainThreadIdleEndCallback)
		(*pMainThreadIdleEndCallback)();
}

/**
	Set callback function
	Notify them when the callback is triggered
*/
inline void setMainThreadIdleCallbacks(void (*pStartCallback)(), void (*pEndCallback)())
{
	pMainThreadIdleStartCallback = pStartCallback;
	pMainThreadIdleEndCallback = pEndCallback;
}

}

}

#endif // OURO_CONCURENCY_H
