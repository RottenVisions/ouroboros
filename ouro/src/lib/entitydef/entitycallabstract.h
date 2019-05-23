// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com


#ifndef OURO_ENTITYCALL_BASE_H
#define OURO_ENTITYCALL_BASE_H
	
#include "common/common.h"
#include "pyscript/scriptobject.h"
#include "entitydef/common.h"
#include "network/address.h"

#ifdef OURO_SERVER
#include "server/components.h"
#endif

namespace Ouroboros{

class ScriptDefModule;
class RemoteEntityMethod;
class MethodDescription;

namespace Network
{
class Channel;
class Bundle;
}

class EntityCallAbstract : public script::ScriptObject
{
		/** Subclassing populates some py operations into derived classes*/
	INSTANCE_SCRIPT_HREADER(EntityCallAbstract, ScriptObject)
public:
	EntityCallAbstract(PyTypeObject* scriptType, 
		const Network::Address* pAddr, 
		COMPONENT_ID componentID, 
		ENTITY_ID eid, 
		uint16 utype, 
		ENTITYCALL_TYPE type);
	
	virtual ~EntityCallAbstract();

	typedef std::tr1::function<RemoteEntityMethod* (MethodDescription* pMethodDescription, EntityCallAbstract* pEntityCall)> EntityCallCallHookFunc;
	typedef std::tr1::function<Network::Channel* (EntityCallAbstract&)> FindChannelFunc;

	enum ENTITYCALL_CLASS
	{
		ENTITYCALL_CLASS_ENTITY,
		ENTITYCALL_CLASS_ENTITY_COMPONENT,
	};

	/**
		Set the address of the __findChannelFunc function of entityCall
	*/
	static void setFindChannelFunc(FindChannelFunc func) {
		__findChannelFunc = func;
	};

	/**
		Set the address of the __hookCallFunc function of entityCall
	*/
	static void setEntityCallCallHookFunc(EntityCallCallHookFunc* pFunc) {
		__hookCallFuncPtr = pFunc;
	};

	static void resetCallHooks() {
		__hookCallFuncPtr = NULL;
		__findChannelFunc = FindChannelFunc();
	}

	virtual ENTITYCALL_CLASS entitycallClass() const {
		return ENTITYCALL_CLASS_ENTITY;
	}

	/** 
		Get entityID
	*/
	INLINE ENTITY_ID id() const;

	INLINE void id(int v);

	DECLARE_PY_GET_MOTHOD(pyGetID);

	/** 
		Get component ID
	*/
	INLINE COMPONENT_ID componentID(void) const;

	/** 
		Set the ID of the component
	*/
	INLINE void componentID(COMPONENT_ID cid);

	/** 
		Get utype
	*/
	INLINE ENTITY_SCRIPT_UID utype(void) const;

	/** 
		Get type
	*/
	INLINE ENTITYCALL_TYPE type(void) const;

	/** 
		Support for the pickler method
	*/
	static PyObject* __py_reduce_ex__(PyObject* self, PyObject* protocol);
	
	virtual Network::Channel* getChannel(void);

	virtual bool sendCall(Network::Bundle* pBundle);

	virtual void newCall_(Network::Bundle& bundle);
	virtual void newCall(Network::Bundle& bundle);

	const Network::Address& addr() const{ return addr_; }
	void addr(const Network::Address& saddr){ addr_ = saddr; }

	INLINE bool isClient() const;
	INLINE bool isCell() const;
	INLINE bool isCellReal() const;
	INLINE bool isCellViaBase() const;
	INLINE bool isBase() const;
	INLINE bool isBaseReal() const;
	INLINE bool isBaseViaCell() const;
	
	ScriptDefModule* pScriptDefModule();

protected:
	COMPONENT_ID componentID_; // ID of the remote machine component
	Network::Address addr_; // channel address
	ENTITYCALL_TYPE type_; // type of entityCall
	ENTITY_ID								id_;					// entityID
	ENTITY_SCRIPT_UID utype_; // entity utype in the order defined in entities.xml

	static EntityCallCallHookFunc*			__hookCallFuncPtr;
	static FindChannelFunc					__findChannelFunc;
};

}

#ifdef CODE_INLINE
#include "entitycallabstract.inl"
#endif
#endif // OURO_ENTITYCALL_BASE_H
 