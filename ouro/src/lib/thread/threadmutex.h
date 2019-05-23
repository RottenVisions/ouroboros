// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

/*
	Thread mutual complaint body:
	Usage:
		ThreadMutex tm;
		tm.lockMutex();
		....security code
		tm.unlockMutex();
		
		It is best to use it with ThreadGuard
		Defining a mutual complaint member in a class
		ThreadMutex tm;
		Where you need protection:
		void XXCLASS::func(void)
		{
			ThreadGuard tg(this->tm);
			The following code is safe
			...
		}
*/
#ifndef __THREADMUTEX_H__
#define __THREADMUTEX_H__
	
#include "common/common.h"


namespace Ouroboros{ namespace thread{

class ThreadMutexNull
{
public:
	ThreadMutexNull(void)
	{
	}

	virtual ~ThreadMutexNull(void)
	{
	}

	virtual void lockMutex(void)
	{
	}

	virtual void unlockMutex(void)
	{
	}
};

class ThreadMutex : public ThreadMutexNull
{
public:
	ThreadMutex(void)
	{
		THREAD_MUTEX_INIT(mutex_);
	}

	ThreadMutex(const ThreadMutex& v)
	{
		// This does not allow copying the construct mutex_, which is very dangerous
		// will cause multiple THREAD_MUTEX_DELETE
		THREAD_MUTEX_INIT(mutex_);
	}

	virtual ~ThreadMutex(void)
	{ 
		THREAD_MUTEX_DELETE(mutex_);
	}	
	
	virtual void lockMutex(void)
	{
		THREAD_MUTEX_LOCK(mutex_);
	}

	virtual void unlockMutex(void)
	{
		THREAD_MUTEX_UNLOCK(mutex_);
	}

protected:
	THREAD_MUTEX mutex_;
};

}
}
#endif
