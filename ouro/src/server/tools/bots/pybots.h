// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com


#ifndef OURO_PYBOTS_H
#define OURO_PYBOTS_H
	
// common include	
#include "profile.h"
#include "create_and_login_handler.h"
#include "common/timer.h"
#include "pyscript/script.h"
#include "network/endpoint.h"
#include "helper/debug_helper.h"
#include "xml/xml.h"	
#include "common/singleton.h"
#include "common/smartpointer.h"
#include "common/timer.h"
#include "network/interfaces.h"
#include "network/event_dispatcher.h"
#include "network/network_interface.h"
#include "network/event_poller.h"
#include "client_lib/clientapp.h"
#include "pyscript/pyobject_pointer.h"
#include "entitydef/entitydef.h"

namespace Ouroboros{

class PyBots  : public script::ScriptObject
{
	/** 
		Subclassing populates some py operations into a derived class
	*/
	INSTANCE_SCRIPT_HREADER(PyBots, ScriptObject)	
public:
	PyBots();
	~PyBots();

	/** 
		Expose some dictionary methods to python
	*/
	DECLARE_PY_MOTHOD_ARG1(pyHas_key, ENTITY_ID);
	DECLARE_PY_MOTHOD_ARG0(pyKeys);
	DECLARE_PY_MOTHOD_ARG0(pyValues);
	DECLARE_PY_MOTHOD_ARG0(pyItems);
	
	static PyObject* __py_pyGet(PyObject * self, 
		PyObject * args, PyObject* kwds);

	/** 
		Map operation function related
	*/
	static PyObject* mp_subscript(PyObject * self, PyObject * key);

	static int mp_length(PyObject * self);

	static PyMappingMethods mappingMethods;
protected:
};

}

#endif // OURO_PYBOTS_H
