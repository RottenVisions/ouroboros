// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com


#ifndef OUROBOROS_CLIENT_ENTITY_METHOD_H
#define OUROBOROS_CLIENT_ENTITY_METHOD_H

#include "common/common.h"
#if OURO_PLATFORM == PLATFORM_WIN32
#pragma warning (disable : 4910)
#pragma warning (disable : 4251)
#endif
// common include
#include "entitydef/datatype.h"
#include "entitydef/datatypes.h"
#include "helper/debug_helper.h"
#include "pyscript/scriptobject.h"
//#define NDEBUG
// windows include
#if OURO_PLATFORM == PLATFORM_WIN32
#else
// linux include
#endif

namespace Ouroboros{

class ClientEntityMethod : public script::ScriptObject
{
	/** Subclassing fills some py operations into derived classes */
	INSTANCE_SCRIPT_HREADER(ClientEntityMethod, script::ScriptObject)
public:
	ClientEntityMethod(PropertyDescription* pComponentPropertyDescription,
		const ScriptDefModule* pScriptModule, MethodDescription* methodDescription,
		ENTITY_ID srcEntityID, ENTITY_ID clientEntityID);

	virtual ~ClientEntityMethod();

	const char* getName(void) const
	{
		return methodDescription_->getName();
	};

	MethodDescription* getDescription(void) const
	{
		return methodDescription_;
	}

	static PyObject* tp_call(PyObject* self,
			PyObject* args, PyObject* kwds);

	PyObject* callmethod(PyObject* args, PyObject* kwds);

protected:
	PropertyDescription*					pComponentPropertyDescription_;

	const ScriptDefModule*					pScriptModule_;						// The script module object used by this entity

	MethodDescription*						methodDescription_;					// Description of this method

	ENTITY_ID								srcEntityID_;						// srcEntityID_

	ENTITY_ID								clientEntityID_;					// clientEntityID_
};

}

#endif // OUROBOROS_CLIENT_ENTITY_METHOD_H
