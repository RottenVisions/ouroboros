// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com


#ifndef OURO_CLIENT_ENTITY_H
#define OURO_CLIENT_ENTITY_H
	
// common include	
#include "common/common.h"
//#include "network/channel.h"
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

class ClientEntity;
class ScriptDefModule;
class PropertyDescription;

class ClientEntityComponent : public script::ScriptObject
{
		/** Subclassing populates some py operations into derived classes*/
	INSTANCE_SCRIPT_HREADER(ClientEntityComponent, ScriptObject)
public:
	ClientEntityComponent(PropertyDescription* pComponentPropertyDescription, ClientEntity* pClientEntity);

	~ClientEntityComponent();

	ScriptDefModule* pComponentScriptDefModule();

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

protected:
	ClientEntity*							pClientEntity_;
	PropertyDescription*					pComponentPropertyDescription_;

};

class ClientEntity : public script::ScriptObject
{
		/** Subclassing populates some py operations into derived classes*/
	INSTANCE_SCRIPT_HREADER(ClientEntity, ScriptObject)
public:
	ClientEntity(ENTITY_ID srcEntityID, ENTITY_ID clientEntityID);
	
	~ClientEntity();
	
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

	ENTITY_ID srcEntityID() const {
		return srcEntityID_;
	}

	ENTITY_ID clientEntityID() const {
		return clientEntityID_;
	}

protected:
	ENTITY_ID								srcEntityID_;						// srcEntityID_

	ENTITY_ID								clientEntityID_;					// clientEntityID_
};

}

#endif // OURO_CLIENT_ENTITY_H
