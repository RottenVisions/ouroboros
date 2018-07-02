// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com


#ifndef OUROBOROS_REAL_ENTITY_METHOD_H
#define OUROBOROS_REAL_ENTITY_METHOD_H

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

class Entity;
class PropertyDescription;

class RealEntityMethod : public script::ScriptObject
{
	/** Subclassing fills some py operations into derived classes */
	INSTANCE_SCRIPT_HREADER(RealEntityMethod, script::ScriptObject)
public:
	RealEntityMethod(PropertyDescription* pComponentPropertyDescription, MethodDescription* methodDescription,
		Entity* ghostEntity);

	virtual ~RealEntityMethod();

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
	PropertyDescription*					pComponentPropertyDescription_;		// Is it a method in a component
	MethodDescription*						methodDescription_;					// Description of this method

	ENTITY_ID								ghostEntityID_;						// ghostEntityID_
	COMPONENT_ID							realCell_;

	std::string								scriptName_;
};

}

#endif // OUROBOROS_REAL_ENTITY_METHOD_H
