// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_ENTITIES_H
#define OURO_ENTITIES_H
	
// common include	
#include "helper/debug_helper.h"
#include "common/common.h"
#include "common/smartpointer.h"
#include "pyscript/scriptobject.h"
#include "pyscript/pyobject_pointer.h"
#include "entitydef/entity_garbages.h"
	
namespace Ouroboros{

template<typename T>
class Entities : public script::ScriptObject
{
	/** 
		Subclassing populates some py operations into a derived class
	*/
	INSTANCE_SCRIPT_HREADER(Entities, ScriptObject)	
public:
	typedef KBEUnordered_map<ENTITY_ID, PyObjectPtr> ENTITYS_MAP;

	Entities():
	ScriptObject(getScriptType(), false),
	_pGarbages(new EntityGarbages<T>())
	{			
	}

	~Entities()
	{
		finalise();
		S_RELEASE(_pGarbages);
	}	

	void finalise()
	{
		clear(false);
	}

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
	static PySequenceMethods mappingSequenceMethods;

	ENTITYS_MAP& getEntities(void) { return _entities; }

	void add(ENTITY_ID id, T* entity);
	void clear(bool callScript);
	void clear(bool callScript, std::vector<ENTITY_ID> excludes);
	PyObjectPtr erase(ENTITY_ID id);

	T* find(ENTITY_ID id);

	size_t size() const { return _entities.size(); }

	EntityGarbages<T>* pGetbages() { return _pGarbages; }
	DECLARE_PY_GET_MOTHOD(pyGarbages);

private:
	ENTITYS_MAP _entities;

	// The entity that has been called destroy but not destructed will be stored here, and has not been destructed for a long time.
	// There is a possibility of a circular reference in the script layer causing a memory leak.
	EntityGarbages<T>* _pGarbages;
};

/** 
	Method table required for Python Entities operations
*/
template<typename T>
PyMappingMethods Entities<T>::mappingMethods =
{
	(lenfunc)mp_length,								// mp_length
	(binaryfunc)mp_subscript,						// mp_subscript
	NULL											// mp_ass_subscript
};

// ²Î¿¼ objects/dictobject.c
// Hack to implement "key in dict"
template<typename T>
PySequenceMethods Entities<T>::mappingSequenceMethods = 
{
    0,											/* sq_length */
    0,											/* sq_concat */
    0,											/* sq_repeat */
    0,											/* sq_item */
    0,											/* sq_slice */
    0,											/* sq_ass_item */
    0,											/* sq_ass_slice */
    PyMapping_HasKey,							/* sq_contains */
    0,											/* sq_inplace_concat */
    0,											/* sq_inplace_repeat */
};

TEMPLATE_SCRIPT_METHOD_DECLARE_BEGIN(template<typename T>, Entities<T>, Entities)
SCRIPT_METHOD_DECLARE("has_key",			pyHas_key,				METH_VARARGS,		0)
SCRIPT_METHOD_DECLARE("keys",				pyKeys,					METH_VARARGS,		0)
SCRIPT_METHOD_DECLARE("values",				pyValues,				METH_VARARGS,		0)
SCRIPT_METHOD_DECLARE("items",				pyItems,				METH_VARARGS,		0)
SCRIPT_METHOD_DECLARE("get",				pyGet,					METH_VARARGS,		0)
SCRIPT_METHOD_DECLARE_END()

TEMPLATE_SCRIPT_MEMBER_DECLARE_BEGIN(template<typename T>, Entities<T>, Entities)
SCRIPT_MEMBER_DECLARE_END()

TEMPLATE_SCRIPT_GETSET_DECLARE_BEGIN(template<typename T>, Entities<T>, Entities)
SCRIPT_GET_DECLARE("garbages",				pyGarbages,				0,							0)
SCRIPT_GETSET_DECLARE_END()
TEMPLATE_SCRIPT_INIT(template<typename T>, Entities<T>, Entities, 0, &Entities<T>::mappingSequenceMethods, &Entities<T>::mappingMethods, 0, 0)	


//-------------------------------------------------------------------------------------
template<typename T>
int Entities<T>::mp_length(PyObject * self)
{
	return (int)static_cast<Entities*>(self)->getEntities().size();
}
	
//-------------------------------------------------------------------------------------
template<typename T>
PyObject * Entities<T>::mp_subscript(PyObject* self, PyObject* key /*entityID*/)
{
	Entities* lpEntities = static_cast<Entities*>(self);
	ENTITY_ID entityID = PyLong_AsLong(key);
	if (PyErr_Occurred())
		return NULL;

	PyObject * pyEntity = NULL;

	ENTITYS_MAP& entities = lpEntities->getEntities();
	ENTITYS_MAP::const_iterator iter = entities.find(entityID);
	if (iter != entities.end())
		pyEntity = iter->second.get();

	if(pyEntity == NULL)
	{
		PyErr_Format(PyExc_KeyError, "%d", entityID);
		//PyErr_PrintEx(0);
		return NULL;
	}

	Py_INCREF(pyEntity);
	return pyEntity;
}

//-------------------------------------------------------------------------------------
template<typename T>
PyObject* Entities<T>::pyHas_key(ENTITY_ID entityID)
{
	ENTITYS_MAP& entities = getEntities();
	return PyLong_FromLong((entities.find(entityID) != entities.end()));
}

//-------------------------------------------------------------------------------------
template<typename T>
PyObject* Entities<T>::pyKeys()
{
	ENTITYS_MAP& entities = getEntities();
	PyObject* pyList = PyList_New(entities.size());
	int i = 0;

	ENTITYS_MAP::const_iterator iter = entities.begin();
	while (iter != entities.end())
	{
		PyObject* entityID = PyLong_FromLong(iter->first);
		PyList_SET_ITEM(pyList, i, entityID);

		i++;
		iter++;
	}

	return pyList;
}

//-------------------------------------------------------------------------------------
template<typename T>
PyObject* Entities<T>::pyValues()
{
	ENTITYS_MAP& entities = getEntities();
	PyObject* pyList = PyList_New(entities.size());
	int i = 0;

	ENTITYS_MAP::const_iterator iter = entities.begin();
	while (iter != entities.end())
	{
		Py_INCREF(iter->second.get());
		PyList_SET_ITEM(pyList, i, iter->second.get());

		i++;
		iter++;
	}

	return pyList;
}

//-------------------------------------------------------------------------------------
template<typename T>
PyObject* Entities<T>::pyItems()
{
	ENTITYS_MAP& entities = getEntities();
	PyObject* pyList = PyList_New(entities.size());
	int i = 0;

	ENTITYS_MAP::const_iterator iter = entities.begin();
	while (iter != entities.end())
	{
		PyObject * pTuple = PyTuple_New(2);
		PyObject* entityID = PyLong_FromLong(iter->first);
		Py_INCREF(iter->second.get());

		PyTuple_SET_ITEM(pTuple, 0, entityID);
		PyTuple_SET_ITEM(pTuple, 1, iter->second.get());
		PyList_SET_ITEM(pyList, i, pTuple);
		i++;
		iter++;
	}

	return pyList;
}

//-------------------------------------------------------------------------------------
template<typename T>
PyObject* Entities<T>::__py_pyGet(PyObject* self, PyObject * args, PyObject* kwds)
{
	Entities* lpEntities = static_cast<Entities*>(self);
	PyObject * pDefault = Py_None;
	ENTITY_ID id = 0;
	if (!PyArg_ParseTuple( args, "i|O", &id, &pDefault))
	{
		return NULL;
	}

	PyObject* pEntity = lpEntities->find(id);

	if (!pEntity)
	{
		pEntity = pDefault;
	}

	Py_INCREF(pEntity);
	return pEntity;
}

//-------------------------------------------------------------------------------------
template<typename T>
void Entities<T>::add(ENTITY_ID id, T* entity)
{ 
	ENTITYS_MAP::const_iterator iter = _entities.find(id);
	if(iter != _entities.end())
	{
		ERROR_MSG(fmt::format("Entities::add: entityID:{} has exist\n.", id));
		return;
	}

	_entities[id] = entity; 
}

//-------------------------------------------------------------------------------------
template<typename T>
void Entities<T>::clear(bool callScript)
{
	ENTITYS_MAP::const_iterator iter = _entities.begin();
	while (iter != _entities.end())
	{
		T* entity = (T*)iter->second.get();
		_pGarbages->add(entity->id(), entity);
		entity->destroy(callScript);
		iter++;
	}

	_entities.clear();
}

//-------------------------------------------------------------------------------------
template<typename T>
void Entities<T>::clear(bool callScript, std::vector<ENTITY_ID> excludes)
{
	ENTITYS_MAP::const_iterator iter = _entities.begin();
	for (;iter != _entities.end();)
	{
		if(std::find(excludes.begin(), excludes.end(), iter->first) != excludes.end())
		{
			++iter;
			continue;
		}

		T* entity = (T*)iter->second.get();
		_pGarbages->add(entity->id(), entity);
		entity->destroy(callScript);
		_entities.erase(iter++);
	}
	
	// Cannot be emptied due to the presence of excludes
	// _entities.clear();
}

//-------------------------------------------------------------------------------------
template<typename T>
T* Entities<T>::find(ENTITY_ID id)
{
	ENTITYS_MAP::const_iterator iter = _entities.find(id);
	if(iter != _entities.end())
	{
		return static_cast<T*>(iter->second.get());
	}
	
	return NULL;
}

//-------------------------------------------------------------------------------------
template<typename T>
PyObjectPtr Entities<T>::erase(ENTITY_ID id)
{
	ENTITYS_MAP::iterator iter = _entities.find(id);
	if(iter != _entities.end())
	{
		T* entity = static_cast<T*>(iter->second.get());
		_pGarbages->add(id, entity);
		_entities.erase(iter);
		return entity;
	}
	
	return NULL;
}

//-------------------------------------------------------------------------------------
template<typename T>
PyObject* Entities<T>::pyGarbages()
{ 
	if(_pGarbages == NULL)
		S_Return;

	Py_INCREF(_pGarbages);
	return _pGarbages; 
}

}
#endif // OURO_ENTITIES_H

