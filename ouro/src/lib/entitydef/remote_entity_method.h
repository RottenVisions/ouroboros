// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com


#ifndef OUROBOROS_REMOTE_ENTITY_METHOD_H
#define OUROBOROS_REMOTE_ENTITY_METHOD_H

#include "common/common.h"
#if OURO_PLATFORM == PLATFORM_WIN32
#pragma warning (disable : 4910)
#pragma warning (disable : 4251)
#endif
#include "datatype.h"
#include "datatypes.h"
#include "helper/debug_helper.h"
#include "network/packet.h"
#include "entitycallabstract.h"
#include "pyscript/scriptobject.h"


namespace Ouroboros{

class MethodDescription;

class RemoteEntityMethod : public script::ScriptObject
{
	/** Subclassing fills some py operations into derived classes */
	INSTANCE_SCRIPT_HREADER(RemoteEntityMethod, script::ScriptObject)

public:
	RemoteEntityMethod(MethodDescription* methodDescription,
						EntityCallAbstract* entityCall, PyTypeObject* pyType = NULL);

	virtual ~RemoteEntityMethod();

	const char* getName(void) const;

	MethodDescription* getDescription(void) const
	{
		return methodDescription_;
	}

	static PyObject* tp_call(PyObject* self,
			PyObject* args, PyObject* kwds);

	EntityCallAbstract* getEntityCall(void) const
	{
		return pEntityCall_;
	}

protected:
	MethodDescription*		methodDescription_;					// Description of this method
	EntityCallAbstract*		pEntityCall_;						// The entityCall to which this method belongs
};
}

#endif // OUROBOROS_REMOTE_ENTITY_METHOD_H
