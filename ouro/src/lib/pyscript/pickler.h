// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_SCRIPT_PICKLER_H
#define OURO_SCRIPT_PICKLER_H

#include "common/common.h"
#include "scriptobject.h"

namespace Ouroboros{ namespace script{

class Pickler
{						
public:	
	/** 
		Agent cPicket.dumps
	*/
	static std::string pickle(PyObject* pyobj);
	static std::string pickle(PyObject* pyobj, int8 protocol);

	/** 
		Proxy cPicket.loads
	*/
	static PyObject* unpickle(const std::string& str);

	/** 
		Initialize pickler
	*/
	static bool initialize(void);
	static void finalise(void);
	
	/** 
		Get the unpickle function table module object
	*/
	static PyObject* getUnpickleFuncTableModule(void){ return pyPickleFuncTableModule_; }
	static PyObject* getUnpickleFunc(const char* funcName);

	static void registerUnpickleFunc(PyObject* pyFunc, const char* funcName);

private:
	static PyObject* picklerMethod_; // cPicket.dumps method pointer
	static PyObject* unPicklerMethod_; // cPicket.loads method pointer

	static PyObject* pyPickleFuncTableModule_; // unpickle function table module object All the unpick functions of the custom class need to be registered here

	static bool isInit; // whether it has been initialized
} ;

}
}

#endif // OURO_SCRIPT_PICKLER_H
