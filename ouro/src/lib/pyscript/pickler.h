// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_SCRIPT_PICKLER_H
#define OURO_SCRIPT_PICKLER_H

#include "common/common.h"
#include "scriptobject.h"

namespace Ouroboros{ namespace script{

class Pickler
{
public:
	/**
		proxy cPicket.dumps
	*/
	static std::string pickle(PyObject* pyobj);
	static std::string pickle(PyObject* pyobj, int8 protocol);

	/**
		proxy cPicket.loads
	*/
	static PyObject* unpickle(const std::string& str);

	/**
		Initialize pickler
	*/
	static bool initialize(void);
	static void finalise(void);

	/**
		Get unpickle function table module object
	*/
	static PyObject* getUnpickleFuncTableModule(void){ return pyPickleFuncTableModule_; }
	static PyObject* getUnpickleFunc(const char* funcName);

	static void registerUnpickleFunc(PyObject* pyFunc, const char* funcName);

private:
	static PyObject* picklerMethod_;						// cPicket.Dumps method pointer
	static PyObject* unPicklerMethod_;						// cPicket.The method method pointer

	static PyObject* pyPickleFuncTableModule_;				// Unpickle function table module object unpickle function of all custom classes need to register here

	static bool	isInit;										// Has it been initialized
} ;

}
}

#endif // OURO_SCRIPT_PICKLER_H
