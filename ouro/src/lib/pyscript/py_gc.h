// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_PY_GC_H
#define OURO_PY_GC_H

#include "common/common.h"
#include "scriptobject.h"

namespace Ouroboros{ namespace script{

class PyGC
{						
public:	
	static uint32 DEBUG_STATS;
	static uint32 DEBUG_COLLECTABLE;
	static uint32 DEBUG_UNCOLLECTABLE;
	static uint32 DEBUG_SAVEALL;
	static uint32 DEBUG_LEAK;
	
	/** 
		Initialize pickler
	*/
	static bool initialize(void);
	static void finalise(void);
	
	/** 
		Forced recycling of garbage
	*/
	static void collect(int8 generations = -1);

	/** 
		Set debug flag
	*/
	static void set_debug(uint32 flags);
	
	/**
		Increase count
	*/
	static void incTracing(std::string name);

	/**
		Reduce count
	*/
	static void decTracing(std::string name);

	/**
		Debug traces the py object count of the ouro package
	*/
	static void debugTracing(bool shuttingdown = true);

	/**
		Script call
	*/
	static PyObject* __py_debugTracing(PyObject* self, PyObject* args);

private:
	static PyObject* collectMethod_; // cPicket.dumps method pointer
	static PyObject* set_debugMethod_; // cPicket.loads method pointer

	static bool isInit; // whether it has been initialized

	static OUROUnordered_map<std::string, int> tracingCountMap_; // Track specific object counters
} ;

}
}

#endif // OURO_PY_GC_H
