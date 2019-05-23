// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com


#ifndef OURO_ALL_CLIENTS_H
#define OURO_ALL_CLIENTS_H

#include "common/common.h"
#include "pyscript/scriptobject.h"
#include "entitydef/common.h"
#include "network/address.h"

//#define NDEBUG
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>	
#include <map>	
#include <vector>	
// windows include	
#if OURO_PLATFORM == PLATFORM_WIN32
#include <time.h> 
#else
// linux include
#include <errno.h>
#endif
	
namespace Ouroboros{

namespace Network
{
class Channel;
class Bundle;
}

class AllClients;
class ScriptDefModule;
class PropertyDescription;

class AllClientsComponent : public script::ScriptObject
{
		/** Subclassing populates some py operations into derived classes*/
	INSTANCE_SCRIPT_HREADER(AllClientsComponent, ScriptObject)
public:
	AllClientsComponent(PropertyDescription* pComponentPropertyDescription, AllClients* pAllClients);

	~AllClientsComponent();

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

	ScriptDefModule* pComponentScriptDefModule();

protected:
	AllClients* pAllClients_;
	PropertyDescription* pComponentPropertyDescription_;
};

class AllClients : public script::ScriptObject
{
		/** Subclassing populates some py operations into derived classes*/
	INSTANCE_SCRIPT_HREADER(AllClients, ScriptObject)
public:
	AllClients(const ScriptDefModule* pScriptModule, 
		ENTITY_ID eid, 
		bool otherClients);
	
	~AllClients();
	
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
		Get entityID
	*/
	ENTITY_ID id() const{ return id_; }
	void setID(int id){ id_ = id; }
	DECLARE_PY_GET_MOTHOD(pyGetID);

	void setScriptModule(const ScriptDefModule*	pScriptModule){ 
		pScriptModule_ = pScriptModule; 
	}

	bool isOtherClients() const {
		return otherClients_;
	}

protected:
	const ScriptDefModule* pScriptModule_; // The script module object used by the entity

	ENTITY_ID								id_;					// entityID

	bool otherClients_; // Is it just another client, not including myself?
};

}

#endif // OURO_ALL_CLIENTS_H
