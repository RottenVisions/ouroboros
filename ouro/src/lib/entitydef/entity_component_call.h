// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com


#ifndef OURO_ENTITY_COMPONENT_CELL_BASE_CLIENT__CALL_H
#define OURO_ENTITY_COMPONENT_CELL_BASE_CLIENT__CALL_H

#include "common/common.h"
#include "pyscript/scriptobject.h"
#include "entitydef/entitycallabstract.h"

#ifdef OURO_SERVER
#include "server/components.h"
#endif


namespace Ouroboros{

namespace Network
{
class Channel;
}

class EntityCall;
class ScriptDefModule;
class RemoteEntityMethod;
class MethodDescription;
class PropertyDescription;

class EntityComponentCall : public EntityCallAbstract
{
	/** Subclassing fills some py operations into derived classes */
	INSTANCE_SCRIPT_HREADER(EntityComponentCall, EntityCallAbstract)
public:
	typedef std::tr1::function<RemoteEntityMethod* (MethodDescription* pMethodDescription, EntityComponentCall* pEntityCall)> EntityCallCallHookFunc;
	typedef std::tr1::function<Network::Channel* (EntityComponentCall&)> FindChannelFunc;

	EntityComponentCall(EntityCall* pEntityCall, PropertyDescription* pComponentPropertyDescription);

	~EntityComponentCall();

	virtual ENTITYCALL_CLASS entitycallClass() const {
		return ENTITYCALL_CLASS_ENTITY_COMPONENT;
	}

	/**
		Script request get property or method
	*/
	PyObject* onScriptGetAttribute(PyObject* attr);

	/**
		Get the description of the object
	*/
	PyObject* tp_repr();
	PyObject* tp_str();

	void c_str(char* s, size_t size);

	/**
		Script is called when it is installed
	*/
	static void onInstallScript(PyObject* mod);

	virtual RemoteEntityMethod* createRemoteMethod(MethodDescription* pMethodDescription);

	/**
		Unpickle method
	*/
	static PyObject* __unpickle__(PyObject* self, PyObject* args);

	ScriptDefModule* pComponentScriptDefModule();

	virtual void newCall(Network::Bundle& bundle);

protected:
	EntityCall*								pEntityCall_;
	PropertyDescription*					pComponentPropertyDescription_;
};

}
#endif // OURO_ENTITY_COMPONENT_CELL_BASE_CLIENT__CALL_H