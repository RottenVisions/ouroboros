// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_PY_URL_H
#define OURO_PY_URL_H

#include "common/common.h"
#include "scriptobject.h"
#include "pyobject_pointer.h"
#include "network/http_utility.h"

namespace Ouroboros { namespace script{

class Script;

class PyUrl
{						
public:	
	/** 
		Initialize
	*/
	static bool initialize(Script* pScript);
	static void finalise(void);
	
	static PyObject* __py_urlopen(PyObject* self, PyObject* args);

	static void onHttpCallback(bool success, const Network::Http::Request& pRequest, const std::string& data);

private:
	static bool isInit; // whether it has been initialized
	static std::map<PyObject*, PyObjectPtr> pyCallbacks;

};

}
}

#endif // OURO_PY_URL_H
