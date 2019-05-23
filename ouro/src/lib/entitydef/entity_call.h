// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com


#ifndef OURO_ENTITY_CELL_BASE_CLIENT__ENTITYCALL_H
#define OURO_ENTITY_CELL_BASE_CLIENT__ENTITYCALL_H
	
#include "common/common.h"
#include "entitydef/entitycallabstract.h"
#include "pyscript/scriptobject.h"

	
namespace Ouroboros{


class EntityCall : public EntityCallAbstract
{
		/** Subclassing populates some py operations into derived classes*/
	INSTANCE_SCRIPT_HREADER(EntityCall, EntityCallAbstract)
public:
	EntityCall(ScriptDefModule* pScriptModule, const Network::Address* pAddr, COMPONENT_ID componentID, 
		ENTITY_ID eid, ENTITYCALL_TYPE type);

	~EntityCall();
	
	/** 
		Script request to get property or method
	*/
	PyObject* onScriptGetAttribute(PyObject* attr);						
			
	/** 
		Get the description of the object
	*/
	PyObject* tp_repr();
	PyObject* tp_str();
	
	void c_str(char* s, size_t size);

	/** 
		Unpick method
	*/
	static PyObject* __unpickle__(PyObject* self, PyObject* args);

	/** 
		Called when the script is installed
	*/
	static void onInstallScript(PyObject* mod);

	virtual RemoteEntityMethod* createRemoteMethod(MethodDescription* pMethodDescription);

	void reload();

	typedef std::vector<EntityCall*> ENTITYCALLS;
	static ENTITYCALLS entityCalls;
	
	ScriptDefModule* pScriptModule() {
		return pScriptModule_;
	}
	
	virtual void newCall(Network::Bundle& bundle);

	PyObject* pyGetComponent(const std::string& componentName, bool all);
	static PyObject* __py_pyGetComponent(PyObject* self, PyObject* args);

protected:
	std::string								scriptModuleName_;

	// The script module object used by the entity
	ScriptDefModule*						pScriptModule_;	

	void _setATIdx(ENTITYCALLS::size_type idx) {
		atIdx_ = idx; 
	}

	ENTITYCALLS::size_type	atIdx_;
};

}
#endif // OURO_ENTITY_CELL_BASE_CLIENT__ENTITYCALL_H
