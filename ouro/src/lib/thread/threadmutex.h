// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

/*
	Thread mutual complaint body:
	usage:
		ThreadMutex tm;
		tm.lockMutex();
		....Security code
		tm.unlockMutex();

		It is best to use with ThreadGuard
		Defining mutual partner members in a class
		ThreadMutex tm;
		Where protection is needed:
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
		// Copying constructor mutex is not allowed here, which is very dangerous
		// Will cause multiple THREAD_MUTEX_DELETE
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
