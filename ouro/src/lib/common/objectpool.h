// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_OBJECTPOOL_H
#define OURO_OBJECTPOOL_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <map>
#include <list>
#include <vector>
#include <queue>

#include "common/timestamp.h"
#include "thread/threadmutex.h"

namespace Ouroboros{

#define OBJECT_POOL_INIT_SIZE			16
#define OBJECT_POOL_INIT_MAX_SIZE		OBJECT_POOL_INIT_SIZE * 1024

// Check for slimming every 5 minutes
#define OBJECT_POOL_REDUCING_TIME_OUT	300 * stampsPerSecondD()

template< typename T >
class SmartPoolObject;

/*
	Some objects will be created very frequently. For example: MemoryStream, Bundle, TCPPacket, etc.
	This object pool creates some object caches in advance through server-side spike estimates.
	When used, it directly gets one from the object pool. Unused objects can be.
*/
template< typename T, typename THREADMUTEX = Ouroboros::thread::ThreadMutexNull >
class ObjectPool
{
public:
	typedef std::list<T*> OBJECTS;

	ObjectPool(std::string name):
		objects_(),
		max_(OBJECT_POOL_INIT_MAX_SIZE),
		isDestroyed_(false),
		pMutex_(new THREADMUTEX()),
		name_(name),
		total_allocs_(0),
		obj_count_(0),
		lastReducingCheckTime_(timestamp())
	{
	}

	ObjectPool(std::string name, unsigned int preAssignVal, size_t max):
		objects_(),
		max_((max == 0 ? 1 : max)),
		isDestroyed_(false),
		pMutex_(new THREADMUTEX()),
		name_(name),
		total_allocs_(0),
		obj_count_(0),
		lastReducingCheckTime_(timestamp())
	{
	}

	~ObjectPool()
	{
		destroy();
		SAFE_RELEASE(pMutex_);
	}

	void destroy()
	{
		pMutex_->lockMutex();

		isDestroyed_ = true;

		typename OBJECTS::iterator iter = objects_.begin();
		for(; iter!=objects_.end(); ++iter)
		{
			(*iter)->isEnabledPoolObject(false);
			if(!(*iter)->destructorPoolObject())
			{
				delete (*iter);
			}
		}

		objects_.clear();
		obj_count_ = 0;
		pMutex_->unlockMutex();
	}

	const OBJECTS& objects(void) const
	{
		return objects_;
	}

	void pMutex(Ouroboros::thread::ThreadMutexNull* pMutex)
	{
		SAFE_RELEASE(pMutex_);
		pMutex_ = pMutex;
	}

	Ouroboros::thread::ThreadMutexNull* pMutex()
	{
		return pMutex_;
	}

	void assignObjs(unsigned int preAssignVal = OBJECT_POOL_INIT_SIZE)
	{
		for(unsigned int i=0; i<preAssignVal; ++i)
		{
			T* t = new T();
			t->isEnabledPoolObject(false);
			objects_.push_back(t);
			++total_allocs_;
			++obj_count_;
		}
	}

	/**
		Forces the creation of an object of the specified type.
		If the buffer has already been created then return the existing one, otherwise create a new one, this object must be inherited from T.
	*/
	template<typename T1>
	T* createObject(void)
	{
		pMutex_->lockMutex();

		while(true)
		{
			if(obj_count_ > 0)
			{
				T* t = static_cast<T1*>(*objects_.begin());
				objects_.pop_front();
				--obj_count_;
				t->onEabledPoolObject();
				t->isEnabledPoolObject(true);
				pMutex_->unlockMutex();
				return t;
			}

			assignObjs();
		}

		pMutex_->unlockMutex();

		return NULL;
	}

	/**
		Create an object. If the buffer has been created then return the existing one, otherwise
		Create a new one.
	*/
	T* createObject(void)
	{
		pMutex_->lockMutex();

		while(true)
		{
			if(obj_count_ > 0)
			{
				T* t = static_cast<T*>(*objects_.begin());
				objects_.pop_front();
				--obj_count_;
				t->onEabledPoolObject();
				t->isEnabledPoolObject(true);
				pMutex_->unlockMutex();
				return t;
			}

			assignObjs();
		}

		pMutex_->unlockMutex();

		return NULL;
	}

	/**
		Recycling an object
	*/
	void reclaimObject(T* obj)
	{
		pMutex_->lockMutex();
		reclaimObject_(obj);
		pMutex_->unlockMutex();
	}

	/**
		Recycling an object container
	*/
	void reclaimObject(std::list<T*>& objs)
	{
		pMutex_->lockMutex();

		typename std::list< T* >::iterator iter = objs.begin();
		for(; iter != objs.end(); ++iter)
		{
			reclaimObject_((*iter));
		}

		objs.clear();

		pMutex_->unlockMutex();
	}

	/**
		Recycling an object container
	*/
	void reclaimObject(std::vector< T* >& objs)
	{
		pMutex_->lockMutex();

		typename std::vector< T* >::iterator iter = objs.begin();
		for(; iter != objs.end(); ++iter)
		{
			reclaimObject_((*iter));
		}

		objs.clear();

		pMutex_->unlockMutex();
	}

	/**
		Recycling an object container
	*/
	void reclaimObject(std::queue<T*>& objs)
	{
		pMutex_->lockMutex();

		while(!objs.empty())
		{
			T* t = objs.front();
			objs.pop();
			reclaimObject_(t);
		}

		pMutex_->unlockMutex();
	}

	size_t size(void) const { return obj_count_; }

	std::string c_str()
	{
		char buf[1024];

		pMutex_->lockMutex();

		sprintf(buf, "ObjectPool::c_str(): name=%s, objs=%d/%d, isDestroyed=%s.\n",
			name_.c_str(), (int)obj_count_, (int)max_, (isDestroyed() ? "true" : "false"));

		pMutex_->unlockMutex();

		return buf;
	}

	size_t max() const { return max_; }
	size_t totalAllocs() const { return total_allocs_; }

	bool isDestroyed() const { return isDestroyed_; }

protected:
	/**
		Recycling an object
	*/
	void reclaimObject_(T* obj)
	{
		if(obj != NULL)
		{
			// Reset state first
			obj->onReclaimObject();
			obj->isEnabledPoolObject(false);

			if(size() >= max_ || isDestroyed_)
			{
				delete obj;
				--total_allocs_;
			}
			else
			{
				objects_.push_back(obj);
				++obj_count_;
			}
		}

		uint64 now_timestamp = timestamp();

		if (obj_count_ <= OBJECT_POOL_INIT_SIZE)
		{
			// Less than or equal to refresh the inspection time
			lastReducingCheckTime_ = now_timestamp;
		}
		else if (now_timestamp - lastReducingCheckTime_ > OBJECT_POOL_REDUCING_TIME_OUT)
		{
			// Unused objects that have been greater than OBJECT_POOL_INIT_SIZE for a long time begin cleaning up
			size_t reducing = std::min(objects_.size(), std::min((size_t)OBJECT_POOL_INIT_SIZE, (size_t)(obj_count_ - OBJECT_POOL_INIT_SIZE)));

			//printf("ObjectPool::reclaimObject_(): start reducing..., name=%s, currsize=%d, OBJECT_POOL_INIT_SIZE=%d\n",
			//	name_.c_str(), (int)objects_.size(), OBJECT_POOL_INIT_SIZE);

			while (reducing-- > 0)
			{
				T* t = static_cast<T*>(*objects_.begin());
				objects_.pop_front();
				delete t;

				--obj_count_;
			}

			//printf("ObjectPool::reclaimObject_(): reducing over, name=%s, currsize=%d\n",
			//	name_.c_str(), (int)objects_.size());

			lastReducingCheckTime_ = now_timestamp;
		}
	}

protected:
	OBJECTS objects_;

	size_t max_;

	bool isDestroyed_;

	// For some reason the lock is still necessary
	// For example: log output in dbmgr task thread, log output due to thread callback after loading navmesh in cellapp
	THREADMUTEX* pMutex_;

	std::string name_;

	size_t total_allocs_;

	// Linux environment, list.size()Using std::distance(begin(), end())Way to get
	// Will have an impact on performance, here we make a record of size
	size_t obj_count_;

	// Last slimming check
	// If OBJECT_POOL_REDUCING_TIME_OUT is greater than OBJECT_POOL_INIT_SIZE, then up to OBJECT_POOL_INIT_SIZE
	uint64 lastReducingCheckTime_;
};

/*
	Pool object, all objects using the pool must implement the recycling function.
*/
class PoolObject
{
public:
	PoolObject() :
		isEnabledPoolObject_(false)
	{

	}

	virtual ~PoolObject(){}
	virtual void onReclaimObject() = 0;
	virtual void onEabledPoolObject() {
	}

	virtual size_t getPoolObjectBytes()
	{
		return 0;
	}

	/**
		Notification of the pool object before it is destroyed Some objects can do some work here
	*/
	virtual bool destructorPoolObject()
	{
		return false;
	}

	bool isEnabledPoolObject() const
	{
		return isEnabledPoolObject_;
	}

	void isEnabledPoolObject(bool v)
	{
		isEnabledPoolObject_ = v;
	}

protected:

	// Pool object is active (removed from the pool) state
	bool isEnabledPoolObject_;
};

template< typename T >
class SmartObjectPool : public ObjectPool<T>
{
public:
};

template< typename T >
class SmartPoolObject
{
public:
	SmartPoolObject(T* pPoolObject, ObjectPool<T>& objectPool):
	  pPoolObject_(pPoolObject),
	  objectPool_(objectPool)
	{
	}

	~SmartPoolObject()
	{
		onReclaimObject();
	}

	void onReclaimObject()
	{
		if(pPoolObject_ != NULL)
		{
			objectPool_.reclaimObject(pPoolObject_);
			pPoolObject_ = NULL;
		}
	}

	T* get()
	{
		return pPoolObject_;
	}

	T* operator->()
	{
		return pPoolObject_;
	}

	T& operator*()
	{
		return *pPoolObject_;
	}

private:
	T* pPoolObject_;
	ObjectPool<T>& objectPool_;
};


#define NEW_POOL_OBJECT(TYPE) TYPE::createPoolObject();


}
#endif
