// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

/*
	Thread daemon:
		Avoid malicious competition between threads to create a deadlock problem.
	Usage:
		Defining a mutual complaint member in a class
		ThreadMutex tm;
		Where you need protection:
		void XXCLASS::foo(void)
		{
			ThreadGuard tg(this->tm);
			The following code is safe
			...
		}
*/
#ifndef OURO_THREADGUARD_H
#define OURO_THREADGUARD_H
	
#include "thread/threadmutex.h"
#include <assert.h>
	
namespace Ouroboros{ namespace thread{

class ThreadGuard
{
public:
	explicit ThreadGuard(ThreadMutex* mutexPtr):mutexPtr_(mutexPtr)
	{
		mutexPtr_->lockMutex();
	}

	virtual ~ThreadGuard(void) 
	{ 
		mutexPtr_->unlockMutex();
	}	
	
protected:
	ThreadMutex* mutexPtr_;
};

}
}

#endif // OURO_THREADGUARD_H
