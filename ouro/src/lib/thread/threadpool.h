// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com


#ifndef OURO_THREADPOOL_H
#define OURO_THREADPOOL_H

#include "common/common.h"
#include "common/tasks.h"
#include "helper/debug_helper.h"
#include "thread/threadtask.h"
// windows include	
#if OURO_PLATFORM == PLATFORM_WIN32
#include <windows.h>          // for HANDLE
#include <process.h>          // for _beginthread()	
#include "helper/crashhandler.h"
#else
// linux include
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <pthread.h>	
#endif
	
namespace Ouroboros{ namespace thread{

// The thread pool active thread is larger than this number and is busy
#define THREAD_BUSY_SIZE 32

/*
	Thread base thread class
*/
class ThreadPool;
class TPThread
{
public:
	friend class ThreadPool;

	// Thread status -1 has not started yet, 0 sleep, 1 busy
	enum THREAD_STATE
	{
		THREAD_STATE_STOP = -1,
		THREAD_STATE_SLEEP = 0,
		THREAD_STATE_BUSY = 1,
		THREAD_STATE_END = 2,
		THREAD_STATE_PENDING = 3
	};

public:
	TPThread(ThreadPool* threadPool, int threadWaitSecond = 0):
	threadWaitSecond_(threadWaitSecond), 
	currTask_(NULL), 
	threadPool_(threadPool)
	{
		state_ = THREAD_STATE_SLEEP;
		initCond();
		initMutex();
	}
		
	virtual ~TPThread()
	{
		deleteCond();
		deleteMutex();

		DEBUG_MSG(fmt::format("TPThread::~TPThread(): {}\n", (void*)this));
	}
	
	virtual void onStart(){}
	virtual void onEnd(){}

	virtual void onProcessTaskStart(TPTask* pTask) {}
	virtual void processTask(TPTask* pTask){ pTask->process(); }
	virtual void onProcessTaskEnd(TPTask* pTask) {}

	INLINE THREAD_ID id(void) const;
	
	INLINE void id(THREAD_ID tidp);
	
	/**
		Create a thread and bind itself to the thread
	*/
	THREAD_ID createThread(void);
	
	virtual void initCond(void)
	{
		THREAD_SINGNAL_INIT(cond_);
	}

	virtual void initMutex(void)
	{
		THREAD_MUTEX_INIT(mutex_);	
	}

	virtual void deleteCond(void)
	{
		THREAD_SINGNAL_DELETE(cond_);
	}
	
	virtual void deleteMutex(void)
	{
		THREAD_MUTEX_DELETE(mutex_);
	}

	virtual void lock(void)
	{
		THREAD_MUTEX_LOCK(mutex_); 
	}
	
	virtual void unlock(void)
	{
		THREAD_MUTEX_UNLOCK(mutex_); 
	}	

	virtual TPTask* tryGetTask(void);
	
	/**
		Send condition signal
	*/
	int sendCondSignal(void)
	{
#if OURO_PLATFORM == PLATFORM_WIN32
		return THREAD_SINGNAL_SET(cond_);
#else
REATTEMPT:

		lock();

		if (state_ == THREAD_STATE_PENDING)
		{       
			unlock();
			goto REATTEMPT;
		}

		int ret = THREAD_SINGNAL_SET(cond_);
		unlock();
		return ret;
#endif
	}
	
	/**
		Thread notification wait condition signal
	*/
	bool onWaitCondSignal(void);
	
	bool join(void);

	/**
		Get the task to be processed by this thread
	*/
	INLINE TPTask* task(void) const;

	/**
		Set the task to be processed by this thread
	*/
	INLINE void task(TPTask* tpt);

	INLINE int state(void) const;
	
	/**
		The task to be processed by this thread has been processed. We decided to delete this obsolete task.
	*/
	void onTaskCompleted(void);

#if OURO_PLATFORM == PLATFORM_WIN32
	static unsigned __stdcall threadFunc(void *arg);
#else	
	static void* threadFunc(void* arg);
#endif

	/**
		Set the task to be processed by this thread
	*/
	INLINE ThreadPool* threadPool();

	/**
		Output thread working status
		Mainly used for watcher
	*/
	virtual std::string printWorkState()
	{
		char buf[128];
		lock();
		sprintf(buf, "%p,%u", currTask_, done_tasks_);
		unlock();
		return buf;
	}

	/**
		The thread starts a task count that is continuously executed without changing to the idle state.
	*/
	void reset_done_tasks(){ done_tasks_ = 0; }
	void inc_done_tasks(){ ++done_tasks_; }

protected:
	THREAD_SINGNAL cond_; // thread semaphore
	THREAD_MUTEX mutex_; // thread mutual complaint body
	int threadWaitSecond_; // thread idle state exceeds this number of seconds, the thread exits, less than 0 is a permanent thread (seconds)
	TPTask * currTask_; // The currently executing task of the thread
	THREAD_ID tidp_; // ID of this thread
	ThreadPool* threadPool_; // thread pool pointer
	THREAD_STATE state_; // Thread status: -1 has not been started, 0 sleep, 1 busy
	uint32 done_tasks_; // Thread starts a task count that is continuously executed without changing to idle state
};


class ThreadPool
{
public:		
	
	ThreadPool();
	virtual ~ThreadPool();
	
	void finalise();

	virtual void onMainThreadTick();
	
	bool hasThread(TPThread* pTPThread);

	/**
		Get all thread status of the current thread pool (for watch)
	*/
	std::string printThreadWorks();

	/**
		Get the current total number of threads
	*/	
	INLINE uint32 currentThreadCount(void) const;
	
	/**
		Get the current total number of idle threads
	*/		
	INLINE uint32 currentFreeThreadCount(void) const;
	
	/**
		Create a thread pool
		@param inewThreadCount : When the system is busy, the thread pool will add so many threads (temporary)
		@param inormalMaxThreadCount : The thread pool will keep this number of threads all the time
		@param imaxThreadCount : The thread pool can only have so many threads at most
	*/
	bool createThreadPool(uint32 inewThreadCount, 
			uint32 inormalMaxThreadCount, uint32 imaxThreadCount);
	
	/**
		Add a task to the thread pool
	*/		
	bool addTask(TPTask* tptask);
	bool _addTask(TPTask* tptask);
	INLINE bool addBackgroundTask(TPTask* tptask){ return addTask(tptask); }
	INLINE bool pushTask(TPTask* tptask){ return addTask(tptask); }

	/**
		Whether the number of threads reaches the maximum number
	*/
	INLINE bool isThreadCountMax(void) const;
	
	/**
		Whether the thread pool is busy
		Is there a lot of unprocessed tasks? The thread is very busy.
	*/
	INLINE bool isBusy(void) const;
	
	/** 
		Whether the thread pool has been initialized
	*/
	INLINE bool isInitialize(void) const;

	/**
		return whether it has been destroyed
	*/
	INLINE bool isDestroyed() const;

	/**
		return whether it has been destroyed
	*/
	INLINE void destroy();

	/** 
		Number of tasks that get cached
	*/
	INLINE uint32 bufferTaskSize() const;

	/** 
		Get cached tasks
	*/
	INLINE std::queue<thread::TPTask*>& bufferedTaskList();

	/** 
		Operation cached task lock
	*/
	INLINE void lockBufferedTaskList();
	INLINE void unlockBufferedTaskList();

	/** 
		Get the number of tasks that have been completed
	*/
	INLINE uint32 finiTaskSize() const;

	virtual std::string name() const { return "ThreadPool"; }

public:
	static int timeout;

	/**
		Create a thread pool thread
	*/
	virtual TPThread* createThread(int threadWaitSecond = ThreadPool::timeout, bool threadStartsImmediately = true);

	/**
		Save a task to an unprocessed list
	*/
	void bufferTask(TPTask* tptask);

	/**
		Take a task from the unprocessed list and remove it from the list
	*/
	TPTask* popbufferTask(void);

	/**
		Move a thread to the free list
	*/
	bool addFreeThread(TPThread* tptd);
	
	/**
		Move a thread to the busy list
	*/	
	bool addBusyThread(TPThread* tptd);
	
	/**
		Add a completed task to the list
	*/	
	void addFiniTask(TPTask* tptask);
	
	/**
		Delete a pending (timeout) thread
	*/	
	bool removeHangThread(TPThread* tptd);

	bool initializeWatcher();

protected:
	bool isInitialize_; // Whether the thread pool has been initialized
	
	std::queue<TPTask*> bufferedTaskList_; // List of tasks that have not been processed while the system is busy
	std::list<TPTask*> finiTaskList_; // list of completed tasks
	size_t finiTaskList_count_;

	THREAD_MUTEX bufferedTaskList_mutex_; // Handling bufferTaskList mutex
	THREAD_MUTEX threadStateList_mutex_; // Handling bufferTaskList and freeThreadList_ mutex
	THREAD_MUTEX finiTaskList_mutex_; // Handle finiTaskList mutex
	
	std::list<TPThread*> busyThreadList_; // busy thread list
	std::list<TPThread*> freeThreadList_; // list of idle threads
	std::list<TPThread*> allThreadList_; // list of all threads

	uint32 maxThreadCount_; // maximum number of threads
	uint32 extraNewAddThreadCount_; // If normalThreadCount_ is not enough, then create so many threads
	uint32 currentThreadCount_; // current thread count
	uint32 currentFreeThreadCount_; // The number of currently idle threads
	uint32 normalThreadCount_; // The total number of threads in the standard state ie: By default, the server starts to open so many threads
																	// If the thread is not enough, some new threads will be created, up to maxThreadNum.

	bool isDestroyed_;
};

}


}

#ifdef CODE_INLINE
#include "threadpool.inl"
#endif
#endif // OURO_THREADPOOL_H
