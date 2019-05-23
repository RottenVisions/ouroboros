// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

	
/*
	CallbackMgr (callback manager)
		Since some callback operations are asynchronous, we manage these callbacks through a manager and return an external one.
		Identifies the unique id of the callback that the external can use to trigger this callback.
		
	Usage:
	typedef CallbackMgr<std::tr1::function<void(Entity*, int64, bool)>> CALLBACK_MGR;
	CALLBACK_MGR callbackMgr;
	void xxx(Entity*, int64, bool){}
	CALLBACK_ID callbackID = callbackMgr.save(&xxx); // Bind can be used to bind a class member function
*/

#ifndef OURO_CALLBACKMGR_H
#define OURO_CALLBACKMGR_H
	
#include "Python.h"
#include "idallocate.h"
#include "serverconfig.h"
#include "helper/debug_helper.h"
#include "common/common.h"
#include "common/memorystream.h"
#include "common/timer.h"
#include "pyscript/pyobject_pointer.h"
#include "pyscript/pickler.h"
	
namespace Ouroboros{

template<typename T>
class CallbackMgr
{
public:
	typedef std::map< CALLBACK_ID, std::pair< T, uint64 > > CALLBACKS;

	CallbackMgr():
	cbMap_(),
	idAlloc_(),
	lastTimestamp_(0)
	{
	}

	~CallbackMgr()
	{
		finalise();
	}	
	
	void finalise()
	{
		cbMap_.clear();
	}


	void addToStream(Ouroboros::MemoryStream& s);

	void createFromStream(Ouroboros::MemoryStream& s);

	/** 
		Add a callback to the manager
	*/
	CALLBACK_ID save(T callback, uint64 timeout = 0/*secs*/)
	{
		if(timeout == 0)
			timeout = uint64(ServerConfig::getSingleton().callback_timeout_);

		CALLBACK_ID cbID = idAlloc_.alloc();
		cbMap_.insert(typename CALLBACKS::value_type(cbID, 
			std::pair< T, uint64 >(callback, timestamp() + (timeout * stampsPerSecond()))));

		tick();
		return cbID;
	}
	
	/** 
		Take callback through callbackID
	*/
	T take(CALLBACK_ID cbID)
	{
		typename CALLBACKS::iterator itr = cbMap_.find(cbID);
		if(itr != cbMap_.end()){
			T t = itr->second.first;
			idAlloc_.reclaim(itr->first);
			cbMap_.erase(itr);
			return t;
		}
		
		tick();
		return NULL;
	}

	/**
		tick
	*/
	void tick()
	{
		if(timestamp() - lastTimestamp_ < (ServerConfig::getSingleton().callback_timeout_ * stampsPerSecond()))
			return;

		lastTimestamp_ = timestamp(); 
		typename CALLBACKS::iterator iter = cbMap_.begin();
		for(; iter!= cbMap_.end(); )
		{
			if(lastTimestamp_ > iter->second.second)
			{
				if(processTimeout(iter->first, iter->second.first))
				{
					idAlloc_.reclaim(iter->first);
					cbMap_.erase(iter++);
					continue;
				}
			}

			++iter;
		}
	}

	/**
		Timeout callback
	*/
	bool processTimeout(CALLBACK_ID cbID, T callback)
	{
		INFO_MSG(fmt::format("CallbackMgr::processTimeout: {} timeout!\n", cbID));
		return true;
	}

protected:
	CALLBACKS cbMap_; // All callbacks are stored in this map
	IDAllocate<CALLBACK_ID> idAlloc_; // callback id allocator
	uint64 lastTimestamp_;
};

template<>
inline void CallbackMgr<PyObjectPtr>::addToStream(Ouroboros::MemoryStream& s)
{
	uint32 size = (uint32)cbMap_.size();

	s << idAlloc_.lastID() << size;

	CALLBACKS::iterator iter = cbMap_.begin();
	for(; iter != cbMap_.end(); ++iter)
	{
		s << iter->first;
		s.appendBlob(script::Pickler::pickle(iter->second.first.get()));
		s << iter->second.second;
	}
}

template<>
inline void CallbackMgr<PyObjectPtr>::createFromStream(Ouroboros::MemoryStream& s)
{
	CALLBACK_ID v;
	s >> v;

	idAlloc_.lastID(v);

	uint32 size;
	s >> size;

	for(uint32 i=0; i<size; ++i)
	{
		CALLBACK_ID cbID;
		s >> cbID;

		std::string data;
		s.readBlob(data);

		PyObject* pyCallback = NULL;
		
		if(data.size() > 0)
			pyCallback = script::Pickler::unpickle(data);
		
		uint64 timeout;
		s >> timeout;

		if(pyCallback == NULL || cbID == 0)
		{
			ERROR_MSG(fmt::format("CallbackMgr::createFromStream: pyCallback({}) error!\n", cbID));
			continue;
		}

		cbMap_.insert(CallbackMgr<PyObjectPtr>::CALLBACKS::value_type(cbID, 
			std::pair< PyObjectPtr, uint64 >(pyCallback, timeout)));

		Py_DECREF(pyCallback);
	}
}

template<>
inline void CallbackMgr<PyObject*>::finalise()
{
	std::map< CALLBACK_ID, std::pair< PyObject*, uint64 > >::iterator iter = cbMap_.begin();
	for(; iter!= cbMap_.end(); ++iter)
	{
		Py_DECREF(iter->second.first);
	}

	cbMap_.clear();
}	

template<>
inline bool CallbackMgr<PyObject*>::processTimeout(CALLBACK_ID cbID, PyObject* callback)
{
	PyObject* pystr = PyObject_Str(callback);
	const char* ccattr = PyUnicode_AsUTF8AndSize(callback, NULL);
	Py_DECREF(pystr);

	INFO_MSG(fmt::format("CallbackMgr::processTimeout: callbackID:{}, callback({}) timeout!\n", cbID,
		ccattr));

	Py_DECREF(callback);
	return true;
}

typedef CallbackMgr<PyObjectPtr> PY_CALLBACKMGR;

}

#endif // OURO_CALLBACKMGR_H
