// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com


#include "entity_component.h"
#include "scriptdef_module.h"
#include "pyscript/py_gc.h"
#include "helper/profile.h"
#include "helper/debug_helper.h"
#include "network/network_interface.h"
#include "client_lib/client_interface.h"
#include "client_lib/clientobjectbase.h"
#include "entitydef/entitydef.h"
#include "entitydef/common.h"

#include "../../server/baseapp/baseapp_interface.h"
#include "../../server/cellapp/cellapp_interface.h"

#ifndef CODE_INLINE
#include "entity_component.inl"
#endif

namespace Ouroboros{

SCRIPT_MEMBER_DECLARE_BEGIN(EntityComponent)
SCRIPT_MEMBER_DECLARE_END()

/* is implemented by a different app
SCRIPT_GETSET_DECLARE_BEGIN(EntityComponent)
BASE_SCRIPT_INIT(EntityComponent, 0, 0, 0, 0, 0)
*/

EntityComponent::ENTITY_COMPONENTS EntityComponent::entity_components;


#define DEBUG_OP_ATTRIBUTE(op, ccattr)																		\
		if(g_debugEntity)																					\
		{																									\
			const char* ccattr_DEBUG_OP_ATTRIBUTE = PyUnicode_AsUTF8AndSize(ccattr, NULL);					\
			DEBUG_MSG(fmt::format("{}.{}(refc={}, id={})::debug_op_attr:op={}, {}.\n",						\
												owner()->ob_type->tp_name,									\
										(pComponentDescrs_ ? pComponentDescrs_->getName() : ""),			\
												static_cast<PyObject*>(this)->ob_refcnt, this->ownerID(),	\
															op, ccattr_DEBUG_OP_ATTRIBUTE));				\
		}																									\

#define DEBUG_CREATE_NAMESPACE																				\
		if(g_debugEntity)																					\
		{																									\
			const char* ccattr_DEBUG_CREATE_NAMESPACE = PyUnicode_AsUTF8AndSize(key, NULL);					\
			PyObject* pytsval = PyObject_Str(value);														\
			const char* cccpytsval = PyUnicode_AsUTF8AndSize(pytsval, NULL);								\
			Py_DECREF(pytsval);																				\
			DEBUG_MSG(fmt::format("{}.{}(refc={}, id={})::debug_createNamespace:add {}({}).\n",				\
												owner()->ob_type->tp_name,									\
										(pComponentDescrs_ ? pComponentDescrs_->getName() : ""),			\
												static_cast<PyObject*>(this)->ob_refcnt,					\
												this->ownerID(),											\
																ccattr_DEBUG_CREATE_NAMESPACE,				\
																cccpytsval));								\
		}																									\


//-------------------------------------------------------------------------------------
EntityComponent::EntityComponent(ENTITY_ID ownerID, ScriptDefModule* pComponentDescrs, COMPONENT_TYPE assignmentToComponentType):
ScriptObject(getScriptType(), true),
componentType_(assignmentToComponentType),
ownerID_(-1),
owner_(NULL),
pComponentDescrs_(pComponentDescrs),
atIdx_(ENTITY_COMPONENTS::size_type(-1)),
onDataChangedEvent_(),
pPropertyDescription_(NULL),
clientappID_(0)
{
	atIdx_ = EntityComponent::entity_components.size();
	EntityComponent::entity_components.push_back(this);
	clientappID_ = EntityDef::context().currClientappID;

	script::PyGC::incTracing("EntityComponent");

	initProperty(false);
	ownerID_ = ownerID;
}

//-------------------------------------------------------------------------------------
EntityComponent::~EntityComponent()
{
	OURO_ASSERT(atIdx_ < EntityComponent::entity_components.size());
	OURO_ASSERT(EntityComponent::entity_components[atIdx_] == this);

	// If there are 2 or more EntityCalls, move the last EntityCall to the deleted location of this EntityCall.
	EntityComponent* pBack = EntityComponent::entity_components.back();
	pBack->_setATIdx(atIdx_);
	EntityComponent::entity_components[atIdx_] = pBack;
	atIdx_ = ENTITY_COMPONENTS::size_type(-1);
	EntityComponent::entity_components.pop_back();

	script::PyGC::decTracing("EntityComponent");

	// The component and the destroyed condition can no longer be dereferenced. See the description in onOwnerDestroyEnd
	if(!isDestroyed() && owner_)
		Py_DECREF(owner_);

	//ERROR_MSG(fmt::format("{}::~{}!\n", pComponentDescrs_->getName(), pComponentDescrs_->getName()));
}

//-------------------------------------------------------------------------------------
ENTITY_ID EntityComponent::ownerID() const
{
	return ownerID_;
}

//-------------------------------------------------------------------------------------
PyObject* EntityComponent::pyGetOwnerID()
{
	return PyLong_FromLong(ownerID());
}

//-------------------------------------------------------------------------------------
PyObject* EntityComponent::owner(bool attempt)
{
	if (!owner_)
	{
		if (ownerID_ == 0)
		{
			return NULL;
		}

		if (clientappID_ > 0)
		{
			owner_ = EntityDef::tryGetEntity(clientappID_, ownerID_);
		}
		else
		{
			owner_ = EntityDef::tryGetEntity(g_componentID, ownerID_);
		}

		if(owner_)
			Py_INCREF(owner_);
		else if (!attempt)
			OURO_ASSERT(owner_);
	}

	return owner_;
}

//-------------------------------------------------------------------------------------
void EntityComponent::updateOwner(ENTITY_ID id, PyObject* pOwner)
{
	if (ownerID_ == id)
	{
		if (owner_)
		{
			OURO_ASSERT(pOwner == owner_);
			return;
		}
	}

	ownerID_ = id;
	owner_ = pOwner;

	if (owner_)
		Py_INCREF(owner_);
}

//-------------------------------------------------------------------------------------
PyObject* EntityComponent::pyGetOwner()
{
	PyObject* pyobj = owner();

	if (!pyobj)
	{
		S_Return;
	}

	Py_INCREF(pyobj);
	return pyobj;
}

//-------------------------------------------------------------------------------------
PyObject* EntityComponent::pyName()
{
	return PyUnicode_FromString(pPropertyDescription_->getName());
}

//-------------------------------------------------------------------------------------
PyObject* EntityComponent::pyIsDestroyed()
{
	return ::PyBool_FromLong(isDestroyed());
}

//-------------------------------------------------------------------------------------
void EntityComponent::initializeScript()
{
	// If the attribute creates __cellData__ in createFromPersistentStream, then it should be set to owner's __cellData__.
	// The Entity property will be set to cellData when createName is created. The component data will be called when the entity is created due to structural problems.
	// EntityComponent::createFromPersistentStream (this interface only returns the attribute data of the base part, the cell part is cached here),
	// Get the __cellData__ attribute and delay it to initializeScript and update it.
	PyObject* pyCellData = PyObject_GetAttrString(this, const_cast<char*>("__cellData__"));
	if (pyCellData)
	{
		PyObject* cellDataDict = PyObject_GetAttrString(owner(), "cellData");
		if (!cellDataDict)
		{
			PyErr_Clear();
		}
		else
		{
			if (-1 == PyObject_DelAttrString(this, const_cast<char*>("__cellData__")))
			{
				SCRIPT_ERROR_CHECK();
			}

			PyObject* pyDict = PyDict_GetItemString(cellDataDict, pPropertyDescription_->getName());
			if (0 != PyDict_Update(pyDict, pyCellData))
			{
				SCRIPT_ERROR_CHECK();
				OURO_ASSERT(false);
			}

			Py_DECREF(cellDataDict);
		}

		Py_DECREF(pyCellData);
	}
	else
	{
		PyErr_Clear();
	}

	if (PyObject_HasAttrString(this, "__init__"))
	{
		PyObject* pyResult = PyObject_CallMethod(this, const_cast<char*>("__init__"),
			const_cast<char*>(""));

		if (pyResult != NULL)
			Py_DECREF(pyResult);
		else
			SCRIPT_ERROR_CHECK();
	}
}

//-------------------------------------------------------------------------------------
void EntityComponent::onAttached()
{
	if (PyObject_HasAttrString(this, "onAttached"))
	{
		PyObject* pyResult = PyObject_CallMethod(this, const_cast<char*>("onAttached"),
			const_cast<char*>("O"), owner());

		if (pyResult != NULL)
			Py_DECREF(pyResult);
		else
			SCRIPT_ERROR_CHECK();
	}
}

//-------------------------------------------------------------------------------------
void EntityComponent::onDetached()
{
	if (PyObject_HasAttrString(this, "onDetached"))
	{
		PyObject* pyResult = PyObject_CallMethod(this, const_cast<char*>("onDetached"),
			const_cast<char*>("O"), owner());

		if (pyResult != NULL)
			Py_DECREF(pyResult);
		else
			SCRIPT_ERROR_CHECK();
	}
}

//-------------------------------------------------------------------------------------
void EntityComponent::initProperty(bool isReload)
{
	ScriptDefModule::PROPERTYDESCRIPTION_MAP* oldpropers = NULL;
	if (isReload)
	{
		ScriptDefModule* pOldScriptDefModule =
		EntityDef::findOldScriptModule(pComponentDescrs_->getName());

		if (!pOldScriptDefModule)
		{
			ERROR_MSG(fmt::format("{}::initProperty: not found old_module!\n",
				pComponentDescrs_->getName()));

			OURO_ASSERT(false && "EntityComponent::initProperty: not found old_module");
		}

		oldpropers = &pOldScriptDefModule->getPropertyDescrs();
	}

	const ScriptDefModule::PROPERTYDESCRIPTION_MAP* pPropertyDescrs = pChildPropertyDescrs();

	ScriptDefModule::PROPERTYDESCRIPTION_MAP::const_iterator iter = pPropertyDescrs->begin();
	for (; iter != pPropertyDescrs->end(); ++iter)
	{
		PropertyDescription* propertyDescription = iter->second;
		DataType* dataType = propertyDescription->getDataType();

		if (oldpropers)
		{
			ScriptDefModule::PROPERTYDESCRIPTION_MAP::iterator olditer = oldpropers->find(iter->first);
			if (olditer != oldpropers->end())
			{
			if (strcmp(olditer->second->getDataType()->getName(),
				propertyDescription->getDataType()->getName()) == 0 &&
				strcmp(olditer->second->getDataType()->getName(),
					propertyDescription->getDataType()->getName()) == 0)
				continue;
			}
		}

		if (dataType)
		{
			PyObject* defObj = propertyDescription->newDefaultVal();
			PyObject_SetAttrString(static_cast<PyObject*>(this),
			propertyDescription->getName(), defObj);
			Py_DECREF(defObj);

			/* DEBUG_MSG(fmt::format("EntityComponent::"#CLASS": added [{}] property ref={}.\n",	
			propertyDescription->getName(), defObj->ob_refcnt));*/
		}
		else
		{
			ERROR_MSG(fmt::format("EntityComponent::initProperty: {} dataType is NULL.\n",
				propertyDescription->getName()));
		}
	}
}

//-------------------------------------------------------------------------------------
PyObject* EntityComponent::__py_reduce_ex__(PyObject* self, PyObject* protocol)
{
	EntityComponent* pEntityComponent = static_cast<EntityComponent*>(self);
	
	PyObject* args = PyTuple_New(2);
	PyObject* unpickleMethod = script::Pickler::getUnpickleFunc("EntityComponent");
	PyTuple_SET_ITEM(args, 0, unpickleMethod);
	
	PyObject* args1 = PyTuple_New(3);
	PyTuple_SET_ITEM(args1, 0, PyLong_FromLong(pEntityComponent->ownerID()));
	PyTuple_SET_ITEM(args1, 1, PyLong_FromLong(pEntityComponent->pComponentDescrs()->getUType()));
	PyTuple_SET_ITEM(args1, 2, PyLong_FromLong(pEntityComponent->componentType()));

	PyTuple_SET_ITEM(args, 1, args1);

	if(unpickleMethod == NULL){
		Py_DECREF(args);
		return NULL;
	}
	return args;
}

//-------------------------------------------------------------------------------------
PyObject* EntityComponent::__unpickle__(PyObject* self, PyObject* args)
{
	ENTITY_ID ownerID = 0;
	ENTITY_SCRIPT_UID utype = 0;
	int ctype = 0;

	Py_ssize_t size = PyTuple_Size(args);
	if (size != 2)
	{
		ERROR_MSG("EntityComponent::__unpickle__: args error! size != 2.\n");
		S_Return;
	}

	if (!PyArg_ParseTuple(args, "iHH", &ownerID, &utype, &ctype))
	{
		ERROR_MSG("EntityComponent::__unpickle__: args error!\n");
		S_Return;
	}

	ScriptDefModule* sm = EntityDef::findScriptModule(utype);
	if (sm == NULL)
	{
		ERROR_MSG(fmt::format("EntityComponent::__unpickle__: not found utype {}!\n", utype));
		S_Return;
	}

	PyObject* pyobj = sm->createObject();

	// Execute the Entity constructor
	return new(pyobj) EntityComponent(ownerID, sm, (COMPONENT_TYPE)ctype);
}

//-------------------------------------------------------------------------------------
void EntityComponent::onInstallScript(PyObject* mod)
{
	/*
	static PyMethodDef __unpickle__Method =
	{ "EntityComponent", (PyCFunction)&EntityComponent::__unpickle__, METH_VARARGS, 0 };

	PyObject* pyFunc = PyCFunction_New(&__unpickle__Method, NULL);
	script::Pickler::registerUnpickleFunc(pyFunc, "EntityComponent");

	Py_DECREF(pyFunc);
	*/
}

//-------------------------------------------------------------------------------------
int EntityComponent::onScriptSetAttribute(PyObject* attr, PyObject* value)
{
	DEBUG_OP_ATTRIBUTE("set", attr)
	const char* ccattr = PyUnicode_AsUTF8AndSize(attr, NULL);

	const ScriptDefModule::PROPERTYDESCRIPTION_MAP* pPropertyDescrs = &pComponentDescrs_->getPropertyDescrs();

	if (pComponentDescrs_)
	{
		ScriptDefModule::PROPERTYDESCRIPTION_MAP::const_iterator iter = pPropertyDescrs->find(ccattr);
		if (iter != pPropertyDescrs->end())
		{
			PropertyDescription* propertyDescription = iter->second;
			DataType* dataType = propertyDescription->getDataType();

			if (isDestroyed())
			{
				PyErr_Format(PyExc_AssertionError, "can't set %s.%s to %s. entity is destroyed!",
					scriptName(), ccattr, value->ob_type->tp_name);
				PyErr_PrintEx(0);
				return 0;
			}

			if (!dataType->isSameType(value))
			{
				PyErr_Format(PyExc_ValueError, "can't set %s.%s to %s.",
					scriptName(), ccattr, value->ob_type->tp_name);
				PyErr_PrintEx(0);
				return 0;
			}
			else
			{
				Py_ssize_t ob_refcnt = value->ob_refcnt;
				PyObject* pySetObj = propertyDescription->onSetValue(this, value);

								/*if the def attribute data has changed, it may need to be broadcast.*/
				if (pySetObj != NULL)
				{
					// If the current component object does not belong to the property of the current process entity, then the property does not need to be broadcast.
					if (g_componentType == componentType_)
					{
						if (onDataChangedEvent_)
						{
							onDataChangedEvent_(this, propertyDescription, pySetObj);
						}
						else
						{
						_DO1:
							if (g_componentType != CLIENT_TYPE && g_componentType != BOTS_TYPE)
							{
								if (owner_)
								{
									PyObject* pyDatachangePtr = PyObject_CallMethod(owner_, const_cast<char*>("__getDEP__"), const_cast<char*>(""));

									void* ptr = PyLong_AsVoidPtr(pyDatachangePtr);
									Py_DECREF(pyDatachangePtr);

									if (!ptr)
									{
										SCRIPT_ERROR_CHECK();
									}
									else
									{
										onDataChangedEvent_ = (*static_cast<EntityComponent::OnDataChangedEvent*>(ptr));
										onDataChangedEvent_(this, propertyDescription, pySetObj);
									}
								}
								else
								{
									if (ownerID_ > 0)
									{
										if (owner())
										{
											goto _DO1;
										}
									}
								}
							}
						}
					}

					if (pySetObj == value && pySetObj->ob_refcnt - ob_refcnt > 1)
						Py_DECREF(pySetObj);
				}

				return pySetObj == NULL ? -1 : 0;
			}
		}
	}

	return ScriptObject::onScriptSetAttribute(attr, value);
}

//-------------------------------------------------------------------------------------
int EntityComponent::onScriptDelAttribute(PyObject* attr)
{
	const char* ccattr = PyUnicode_AsUTF8AndSize(attr, NULL);
	DEBUG_OP_ATTRIBUTE("del", attr)

	const ScriptDefModule::PROPERTYDESCRIPTION_MAP* pPropertyDescrs = &pComponentDescrs_->getPropertyDescrs();

	if (pPropertyDescrs) 
	{
		ScriptDefModule::PROPERTYDESCRIPTION_MAP::const_iterator iter = pPropertyDescrs->find(ccattr);
		if (iter != pPropertyDescrs->end())
		{
			char err[255];
			ouro_snprintf(err, 255, "property[%s] defined in %s.def, del failed!", ccattr, scriptName());
			PyErr_SetString(PyExc_TypeError, err);
			PyErr_PrintEx(0);
			return 0;
		}
	}

	if (pComponentDescrs_->findMethodDescription(ccattr, g_componentType) != NULL)
	{
		char err[255];
		ouro_snprintf(err, 255, "method[%s] defined in %s.def, del failed!", ccattr, scriptName());
		PyErr_SetString(PyExc_TypeError, err);
		PyErr_PrintEx(0);
		return 0;
	}

	return ScriptObject::onScriptDelAttribute(attr);
}

//-------------------------------------------------------------------------------------
void EntityComponent::onEntityDestroy(PyObject* pEntity, ScriptDefModule* pEntityScriptDescrs, bool callScript, bool beforeDestroy)
{
	ScriptDefModule::COMPONENTDESCRIPTION_MAP& componentDescrs = pEntityScriptDescrs->getComponentDescrs();

	ScriptDefModule::COMPONENTDESCRIPTION_MAP::iterator comps_iter = componentDescrs.begin();
	for (; comps_iter != componentDescrs.end(); ++comps_iter)
	{
		if (g_componentType == BASEAPP_TYPE)
		{
			if (!comps_iter->second->hasBase())
				continue;
		}
		else if (g_componentType == CELLAPP_TYPE)
		{
			if (!comps_iter->second->hasCell())
				continue;
		}
		else
		{
			if (!comps_iter->second->hasClient())
				continue;
		}

		PyObject* pyObj = PyObject_GetAttrString(pEntity, comps_iter->first.c_str());
		if (pyObj)
		{
			EntityComponent* pEntityComponent = static_cast<EntityComponent*>(pyObj);

			if(beforeDestroy)
				pEntityComponent->onOwnerDestroyBegin(pEntity, pEntityScriptDescrs, callScript);
			else
				pEntityComponent->onOwnerDestroyEnd(pEntity, pEntityScriptDescrs, callScript);

			Py_DECREF(pyObj);
		}
		else
		{
			SCRIPT_ERROR_CHECK();
		}
	}
}

//-------------------------------------------------------------------------------------
void EntityComponent::onOwnerDestroyBegin(PyObject* pEntity, ScriptDefModule* pEntityScriptDescrs, bool callScript)
{
	if(callScript)
		onDetached();
}

//-------------------------------------------------------------------------------------
void EntityComponent::onOwnerDestroyEnd(PyObject* pEntity, ScriptDefModule* pEntityScriptDescrs, bool callScript)
{
	destroyed();

	if (owner_)
		Py_DECREF(owner_);

	// The reference is subtracted here to release the circular reference between the component and the entity and the problem cannot be released.
	// But this is not set to NULL, because in the case of multiple components, such as calling owner.destroy() in the onClientDeath of a component script.
	// Other scripts also need to be able to access owner, except that owner's isDestroyed is True
	//owner_ = NULL;
}

//-------------------------------------------------------------------------------------
PyObject* EntityComponent::tp_repr()
{
	char s[1024];
	c_str(s, 1024);
	return PyUnicode_FromString(s);
}

//-------------------------------------------------------------------------------------
void EntityComponent::c_str(char* s, size_t size)
{
	ouro_snprintf(s, size, "EntityComponent=%s, utype=%d, owner=%s, ownerID=%d, domain=%s.",
		pComponentDescrs_ ? pComponentDescrs_->getName() : "", pComponentDescrs_ ? pComponentDescrs_->getUType() : 0,
		(owner() ? owner()->ob_type->tp_name : "unknown"), ownerID(), COMPONENT_NAME_EX(componentType()));
}

//-------------------------------------------------------------------------------------
PyObject* EntityComponent::tp_str()
{
	return tp_repr();
}

//-------------------------------------------------------------------------------------
void EntityComponent::reload()
{
	pComponentDescrs_ = EntityDef::findScriptModule(pComponentDescrs_->getName());
}

//-------------------------------------------------------------------------------------
const ScriptDefModule::PROPERTYDESCRIPTION_MAP* EntityComponent::pChildPropertyDescrs()
{
	const ScriptDefModule::PROPERTYDESCRIPTION_MAP* pPropertyDescrs = NULL;

	if (componentType_ == BASEAPP_TYPE)
	{
		// EntityDef::currComponentType() == CLIENT_TYPE is set when addClientDataToStream
		if (EntityDef::context().currComponentType == CLIENT_TYPE)
			pPropertyDescrs = &pComponentDescrs_->getClientPropertyDescriptions();
		else
			pPropertyDescrs = &pComponentDescrs_->getBasePropertyDescriptions();
	}
	else if (componentType_ == CELLAPP_TYPE)
	{
		if (EntityDef::context().currComponentType == CLIENT_TYPE)
			pPropertyDescrs = &pComponentDescrs_->getClientPropertyDescriptions();
		else
			pPropertyDescrs = &pComponentDescrs_->getCellPropertyDescriptions();
	}
	else if (componentType_ == CLIENT_TYPE || componentType_ == BOTS_TYPE)
		pPropertyDescrs = &pComponentDescrs_->getClientPropertyDescriptions();
	else
		OURO_ASSERT(false);

	return pPropertyDescrs;
}

//-------------------------------------------------------------------------------------
bool EntityComponent::isSameType(PyObject* pyValue)
{
	const ScriptDefModule::PROPERTYDESCRIPTION_MAP* pPropertyDescrs = pChildPropertyDescrs();

	ScriptDefModule::PROPERTYDESCRIPTION_MAP::const_iterator iter = pPropertyDescrs->begin();
	for (; iter != pPropertyDescrs->end(); ++iter)
	{
		PropertyDescription* propertyDescription = iter->second;

		PyObject* pyVal = PyObject_GetAttrString(this, propertyDescription->getName());
		if (pyVal)
		{
			if (!propertyDescription->getDataType()->isSameType(pyVal))
			{
				Py_DECREF(pyVal);
				return false;
			}
			else
			{
			}

			Py_DECREF(pyVal);
		}
		else
		{
			SCRIPT_ERROR_CHECK();
			return false;
		}
	}

	return true;
}

//-------------------------------------------------------------------------------------
bool EntityComponent::isSamePersistentType(PyObject* pyValue)
{
	// Since the process may get the component from celldata, we need to package the properties of the base part while packing the cell property.
	// So here we need to find the owner and then try to find the component of the base part
	PyObject* pEntity = owner();

	PyObject* baseComponentPart = NULL;
	PyObject* cellComponentPart = NULL;

	if (pEntity)
	{
		OURO_ASSERT(g_componentType == BASEAPP_TYPE || g_componentType == CELLAPP_TYPE);
		OURO_ASSERT(componentType_ == BASEAPP_TYPE || componentType_ == CELLAPP_TYPE);

		if (g_componentType == BASEAPP_TYPE)
		{
			if (pPropertyDescription_->hasCell())
			{
				PyObject* cellDataDict = PyObject_GetAttrString(owner(), "cellData");
				if (!cellDataDict)
				{
					PyErr_Clear();
				}
				else
				{
					cellComponentPart = PyDict_GetItemString(cellDataDict, pPropertyDescription_->getName());
					Py_DECREF(cellDataDict);
					Py_INCREF(cellComponentPart);
				}
			}

			if (componentType_ == BASEAPP_TYPE)
			{
				baseComponentPart = this;
				Py_INCREF(baseComponentPart);
			}
			else
			{
				baseComponentPart = PyObject_GetAttrString(pEntity, pPropertyDescription_->getName());
			}
		}
		else
		{
			cellComponentPart = PyObject_GetAttrString(pEntity, pPropertyDescription_->getName());
		}
	}

	ScriptDefModule::PROPERTYDESCRIPTION_MAP& propertyDescrs = pComponentDescrs_->getPersistentPropertyDescriptions();
	ScriptDefModule::PROPERTYDESCRIPTION_MAP::const_iterator iter = propertyDescrs.begin();

	for (; iter != propertyDescrs.end(); ++iter)
	{
		PropertyDescription* propertyDescription = iter->second;

		PyObject* pyVal = NULL;
		if (propertyDescription->hasCell())
		{
			// Some entities have no cell part, so the cell attribute is ignored
			if (!cellComponentPart)
				continue;

			pyVal = PyDict_GetItemString(cellComponentPart, propertyDescription->getName());
			Py_XINCREF(pyVal);
		}
		else
		{
			pyVal = PyObject_GetAttrString(baseComponentPart, propertyDescription->getName());
		}

		if (pyVal)
		{
			if (!propertyDescription->getDataType()->isSameType(pyVal))
			{
				ERROR_MSG(fmt::format("EntityComponent::isSamePersistentType: {} type(curr_py: {} != {}) error! name={}, utype={}, owner={}, ownerID={}, domain={}.\n",
					propertyDescription->getName(), (pyVal ? pyVal->ob_type->tp_name : "unknown"), propertyDescription->getDataType()->getName(),
					pComponentDescrs_ ? pComponentDescrs_->getName() : "", pComponentDescrs_ ? pComponentDescrs_->getUType() : 0,
					owner()->ob_type->tp_name, ownerID(), COMPONENT_NAME_EX(componentType())));

				Py_DECREF(pyVal);

				return false;
			}

			Py_DECREF(pyVal);
		}
		else
		{
			SCRIPT_ERROR_CHECK();

			ERROR_MSG(fmt::format("EntityComponent::isSamePersistentType: not found property({}), use default values! name={}, utype={}, owner={}, ownerID={}, domain={}.\n",
				propertyDescription->getName(), pComponentDescrs_ ? pComponentDescrs_->getName() : "", pComponentDescrs_ ? pComponentDescrs_->getUType() : 0,
				owner()->ob_type->tp_name, ownerID(), COMPONENT_NAME_EX(componentType())));

			return false;
		}
	}

	if (baseComponentPart)
		Py_DECREF(baseComponentPart);

	if (cellComponentPart)
		Py_DECREF(cellComponentPart);

	return true;
}

//-------------------------------------------------------------------------------------
PyObject* EntityComponent::createFromPersistentStream(ScriptDefModule* pScriptModule, MemoryStream* mstream)
{
	OURO_ASSERT(g_componentType == BASEAPP_TYPE);

	// set to -1 to avoid attempting to broadcast properties in onScriptSetAttribute
	ENTITY_ID oid = ownerID_;
	ownerID_ = -1;

	ScriptDefModule::PROPERTYDESCRIPTION_MAP& propertyDescrs = pComponentDescrs_->getPersistentPropertyDescriptions();
	ScriptDefModule::PROPERTYDESCRIPTION_MAP::const_iterator iter = propertyDescrs.begin();

	PyObject* cellDataDict = NULL;

	for (; iter != propertyDescrs.end(); ++iter)
	{
		PropertyDescription* propertyDescription = iter->second;

		if (pScriptModule && !pScriptModule->hasCell() && !propertyDescription->hasBase())
		{
			continue;
		}

		PyObject* pyobj = propertyDescription->createFromStream(mstream);
		
		if (propertyDescription->hasCell() && !cellDataDict)
		{
			cellDataDict = PyDict_New();

			PyObject_SetAttrString(static_cast<PyObject*>(this),
				"__cellData__", cellDataDict);
		}

		if (pyobj == NULL)
		{
			SCRIPT_ERROR_CHECK();

			ERROR_MSG(fmt::format("EntityComponent::createFromPersistentStream: property({}) error, use default values! name={}, utype={}, owner={}, ownerID={}, domain={}.\n",
				propertyDescription->getName(), pComponentDescrs_ ? pComponentDescrs_->getName() : "", pComponentDescrs_ ? pComponentDescrs_->getUType() : 0,
				owner()->ob_type->tp_name, ownerID(), COMPONENT_NAME_EX(componentType())));

			pyobj = propertyDescription->newDefaultVal();
		}

		if (propertyDescription->hasCell())
		{
			PyDict_SetItemString(cellDataDict, propertyDescription->getName(), pyobj);
		}

		if (propertyDescription->hasBase())
		{
			PyObject_SetAttrString(static_cast<PyObject*>(this),
				propertyDescription->getName(), pyobj);
		}

		Py_DECREF(pyobj);
	}
	
	if(cellDataDict)
		Py_DECREF(cellDataDict);

	ownerID_ = oid;
	return this;
}

//-------------------------------------------------------------------------------------
void EntityComponent::addPersistentToStream(MemoryStream* mstream, PyObject* pyValue)
{
	// Since the process may get the component from celldata, we need to package the properties of the base part while packing the cell property.
	// So here we need to find the owner and then try to find the component of the base part
	PyObject* pEntity = owner();

	PyObject* baseComponentPart = NULL;
	PyObject* cellComponentPart = NULL;

	if (pEntity)
	{
		OURO_ASSERT(g_componentType == BASEAPP_TYPE || g_componentType == CELLAPP_TYPE);
		OURO_ASSERT(componentType_ == BASEAPP_TYPE || componentType_ == CELLAPP_TYPE);

		if (g_componentType == BASEAPP_TYPE)
		{
			if (pPropertyDescription_->hasCell())
			{
				PyObject* cellDataDict = PyObject_GetAttrString(owner(), "cellData");
				if (!cellDataDict)
				{
					PyErr_Clear();
				}
				else
				{
					cellComponentPart = PyDict_GetItemString(cellDataDict, pPropertyDescription_->getName());
					Py_DECREF(cellDataDict);
					Py_INCREF(cellComponentPart);
				}
			}

			if (componentType_ == BASEAPP_TYPE)
			{
				baseComponentPart = this;
				Py_INCREF(baseComponentPart);
			}
			else
			{
				baseComponentPart = PyObject_GetAttrString(pEntity, pPropertyDescription_->getName());
			}
		}
		else
		{
			cellComponentPart = PyObject_GetAttrString(pEntity, pPropertyDescription_->getName());
		}
	}

	ScriptDefModule::PROPERTYDESCRIPTION_MAP& propertyDescrs = pComponentDescrs_->getPersistentPropertyDescriptions();
	ScriptDefModule::PROPERTYDESCRIPTION_MAP::const_iterator iter = propertyDescrs.begin();

	for (; iter != propertyDescrs.end(); ++iter)
	{
		PropertyDescription* propertyDescription = iter->second;

		PyObject* pyVal = NULL;
		if (propertyDescription->hasCell())
		{
			// Some entities have no cell part, so the cell attribute is ignored
			if (!cellComponentPart)
				continue;

			pyVal = PyDict_GetItemString(cellComponentPart, propertyDescription->getName());
			Py_XINCREF(pyVal);
		}
		else
		{
			pyVal = PyObject_GetAttrString(baseComponentPart, propertyDescription->getName());
		}

		if (pyVal)
		{
			propertyDescription->addToStream(mstream, pyVal);
			Py_DECREF(pyVal);
		}
		else
		{
			SCRIPT_ERROR_CHECK();

			ERROR_MSG(fmt::format("EntityComponent::addPersistentToStream: not found property({}), use default values! name={}, utype={}, owner={}, ownerID={}, domain={}.\n",
				propertyDescription->getName(), pComponentDescrs_ ? pComponentDescrs_->getName() : "", pComponentDescrs_ ? pComponentDescrs_->getUType() : 0,
				owner()->ob_type->tp_name, ownerID(), COMPONENT_NAME_EX(componentType())));

			propertyDescription->addPersistentToStream(mstream, NULL);
		}
	}

	if(baseComponentPart)
		Py_DECREF(baseComponentPart);

	if (cellComponentPart)
		Py_DECREF(cellComponentPart);
}

//-------------------------------------------------------------------------------------
void EntityComponent::addToStream(MemoryStream* mstream, PyObject* pyValue)
{
	// EntityDef::currComponentType() == CLIENT_TYPE is set when addClientDataToStream
	// At this point, you need to determine whether the component has such a property containing the client part on the current process. If not, skip it.
	if (EntityDef::context().currComponentType == CLIENT_TYPE)
		addToClientStream(mstream, pyValue);
	else
		addToServerStream(mstream, pyValue);
}

//-------------------------------------------------------------------------------------
void EntityComponent::addToServerStream(MemoryStream* mstream, PyObject* pyValue)
{
	(*mstream) << componentType_ << ownerID_ << pComponentDescrs_->getUType();

	const ScriptDefModule::PROPERTYDESCRIPTION_MAP* pPropertyDescrs = pChildPropertyDescrs();

	uint16 count = (uint16)pPropertyDescrs->size();
	uint16 oldCount = count;
	size_t oldWpos = mstream->wpos();
	(*mstream) << count;

	ScriptDefModule::PROPERTYDESCRIPTION_MAP::const_iterator iter = pPropertyDescrs->begin();
	for (; iter != pPropertyDescrs->end(); ++iter)
	{
		PropertyDescription* propertyDescription = iter->second;
		PyObject* pyVal = PyObject_GetAttrString(this, propertyDescription->getName());
		if (pyVal)
		{
			if (!propertyDescription->getDataType()->isSameType(pyVal))
			{
				CRITICAL_MSG(fmt::format("EntityComponent::addToServerStream: {} type(curr_py: {} != {}) error! name={}, utype={}, owner={}, ownerID={}, domain={}.\n",
					propertyDescription->getName(), (pyVal ? pyVal->ob_type->tp_name : "unknown"), propertyDescription->getDataType()->getName(),
					pComponentDescrs_ ? pComponentDescrs_->getName() : "", pComponentDescrs_ ? pComponentDescrs_->getUType() : 0,
					owner()->ob_type->tp_name, ownerID(), COMPONENT_NAME_EX(componentType())));

				--count;
			}
			else
			{
				(*mstream) << pPropertyDescription_->getUType();
				(*mstream) << propertyDescription->getUType();

				propertyDescription->addToStream(mstream, pyVal);
			}

			Py_DECREF(pyVal);
		}
		else
		{
			SCRIPT_ERROR_CHECK();

			ERROR_MSG(fmt::format("EntityComponent::addToServerStream: not found property({}), use default values! name={}, utype={}, owner={}, ownerID={}, domain={}.\n",
				propertyDescription->getName(), pComponentDescrs_ ? pComponentDescrs_->getName() : "", pComponentDescrs_ ? pComponentDescrs_->getUType() : 0,
				owner()->ob_type->tp_name, ownerID(), COMPONENT_NAME_EX(componentType())));

			(*mstream) << pPropertyDescription_->getUType();
			(*mstream) << propertyDescription->getUType();

			PyObject* pyDefVal = propertyDescription->newDefaultVal();
			propertyDescription->addToStream(mstream, pyDefVal);
			Py_DECREF(pyDefVal);
		}
	}

	if (oldCount != count)
	{
		size_t wpos = mstream->wpos();
		mstream->wpos(oldWpos);
		(*mstream) << count;
		mstream->wpos(wpos);
	}
}

//-------------------------------------------------------------------------------------
void EntityComponent::addToClientStream(MemoryStream* mstream, PyObject* pyValue)
{
	COMPONENT_TYPE	t_componentType = CLIENT_TYPE;
	(*mstream) << t_componentType << ownerID_ << pComponentDescrs_->getUType();

	const ScriptDefModule::PROPERTYDESCRIPTION_MAP* pPropertyDescrs = pChildPropertyDescrs();

	std::vector<PropertyDescription*> propertys;

	{
		ScriptDefModule::PROPERTYDESCRIPTION_MAP::const_iterator iter = pPropertyDescrs->begin();
		for (; iter != pPropertyDescrs->end(); ++iter)
		{
			PropertyDescription* propertyDescription = iter->second;

			if (componentType() == BASEAPP_TYPE)
			{
				if ((propertyDescription->getFlags() & ENTITY_BASEAPP_ANDA_CLIENT_DATA_FLAGS) == 0)
					continue;
			}
			else if (componentType() == CELLAPP_TYPE)
			{
				if ((propertyDescription->getFlags() & ENTITY_CELLAPP_ANDA_CLIENT_DATA_FLAGS) == 0)
					continue;
			}
			else
			{
				OURO_ASSERT(false);
			}

			propertys.push_back(propertyDescription);
		}

		(*mstream) << ((uint16)propertys.size());
	}

	{
		std::vector<PropertyDescription*>::const_iterator iter = propertys.begin();
		for (; iter != propertys.end(); ++iter)
		{
			PropertyDescription* propertyDescription = (*iter);

			if (componentType() == BASEAPP_TYPE)
			{
				if ((propertyDescription->getFlags() & ENTITY_BASEAPP_ANDA_CLIENT_DATA_FLAGS) == 0)
					continue;
			}
			else if (componentType() == CELLAPP_TYPE)
			{
				if ((propertyDescription->getFlags() & ENTITY_CELLAPP_ANDA_CLIENT_DATA_FLAGS) == 0)
					continue;
			}
			else
			{
				OURO_ASSERT(false);
			}

			PyObject* pyVal = PyObject_GetAttrString(this, propertyDescription->getName());
			if (pyVal)
			{
				if (!propertyDescription->getDataType()->isSameType(pyVal))
				{
					CRITICAL_MSG(fmt::format("EntityComponent::addToClientStream: {} type(curr_py: {} != {}) error! name={}, utype={}, owner={}, ownerID={}, domain={}.\n",
						propertyDescription->getName(), (pyVal ? pyVal->ob_type->tp_name : "unknown"), propertyDescription->getDataType()->getName(),
						pComponentDescrs_ ? pComponentDescrs_->getName() : "", pComponentDescrs_ ? pComponentDescrs_->getUType() : 0,
						owner()->ob_type->tp_name, ownerID(), COMPONENT_NAME_EX(t_componentType)));
				}
				else
				{
					if (pComponentDescrs_->usePropertyDescrAlias())
					{
						(*mstream) << pPropertyDescription_->aliasIDAsUint8();
						(*mstream) << propertyDescription->aliasIDAsUint8();
					}
					else
					{
						(*mstream) << pPropertyDescription_->getUType();
						(*mstream) << propertyDescription->getUType();
					}

					propertyDescription->addToStream(mstream, pyVal);
				}

				Py_DECREF(pyVal);
			}
			else
			{
				SCRIPT_ERROR_CHECK();

				ERROR_MSG(fmt::format("EntityComponent::addToClientStream: not found property({}), use default values! name={}, utype={}, owner={}, ownerID={}, domain={}.\n",
					propertyDescription->getName(), pComponentDescrs_ ? pComponentDescrs_->getName() : "", pComponentDescrs_ ? pComponentDescrs_->getUType() : 0,
					owner()->ob_type->tp_name, ownerID(), COMPONENT_NAME_EX(t_componentType)));

				if (pComponentDescrs_->usePropertyDescrAlias())
				{
					(*mstream) << pPropertyDescription_->aliasIDAsUint8();
					(*mstream) << propertyDescription->aliasIDAsUint8();
				}
				else
				{
					(*mstream) << pPropertyDescription_->getUType();
					(*mstream) << propertyDescription->getUType();
				}

				PyObject* pyDefVal = propertyDescription->newDefaultVal();
				propertyDescription->addToStream(mstream, pyDefVal);
				Py_DECREF(pyDefVal);
			}
		}
	}
}

//-------------------------------------------------------------------------------------
PyObject* EntityComponent::createFromStream(MemoryStream* mstream)
{
	ENTITY_SCRIPT_UID ComponentDescrsType;
	uint16 count;

	(*mstream) >> componentType_ >> ownerID_ >> ComponentDescrsType >> count;

	bool isClientApp = clientappID_ > 0;

	if (isClientApp)
	{
		// If it is a component of the process entity, you need to bind the relationship once.
		onAttached();
	}
	else if (!owner_ && g_componentType == CELLAPP_TYPE)
	{
		// This situation is caused by the migration of the entity across processes, you need to re-bind the owner
		owner(true);
	}

	for (uint16 i = 0; i < count; ++i)
	{
		PropertyDescription* propertyDescription = NULL;

		if (isClientApp && pComponentDescrs_->usePropertyDescrAlias())
		{
			uint8 aliasID = 0;

			// parent attribute ID
			(*mstream) >> aliasID;
			(*mstream) >> aliasID;

			propertyDescription = getProperty(aliasID);
			OURO_ASSERT(propertyDescription);
		}
		else
		{
			ENTITY_PROPERTY_UID utype;

			// parent attribute ID
			(*mstream) >> utype;
			(*mstream) >> utype;

			propertyDescription = getProperty(utype);
		}

		PyObject* pyobj = propertyDescription->createFromStream(mstream);

		if (pyobj == NULL)
		{
			SCRIPT_ERROR_CHECK();

			ERROR_MSG(fmt::format("EntityComponent::createFromStream: property({}) error, use default values! name={}, utype={}, owner={}, ownerID={}, domain={}.\n",
				propertyDescription->getName(), pComponentDescrs_ ? pComponentDescrs_->getName() : "", pComponentDescrs_ ? pComponentDescrs_->getUType() : 0,
				owner()->ob_type->tp_name, ownerID(), COMPONENT_NAME_EX(componentType())));

			pyobj = propertyDescription->newDefaultVal();

			PyObject_SetAttrString(static_cast<PyObject*>(this),
				propertyDescription->getName(), pyobj);

			Py_DECREF(pyobj);
		}
		else
		{
			PyObject_SetAttrString(static_cast<PyObject*>(this),
				propertyDescription->getName(), pyobj);

			Py_DECREF(pyobj);
		}
	}

	return this;
}

//-------------------------------------------------------------------------------------
PropertyDescription* EntityComponent::getProperty(ENTITY_PROPERTY_UID child_uid)
{
	bool isClientApp = clientappID_ > 0;

	PropertyDescription* propertyDescription = NULL;

	if (isClientApp && pComponentDescrs_->usePropertyDescrAlias())
	{
		propertyDescription = pComponentDescrs_->findAliasPropertyDescription((ENTITY_DEF_ALIASID)child_uid);
		OURO_ASSERT(propertyDescription);
	}
	else
	{
		if (isClientApp)
		{
			propertyDescription = pComponentDescrs_->findClientPropertyDescription(child_uid);
		}
		else
		{
			if (componentType_ == BASEAPP_TYPE)
			{
				// EntityDef::currComponentType() == CLIENT_TYPE is set when addClientDataToStream
				if (EntityDef::context().currComponentType == CLIENT_TYPE)
					propertyDescription = pComponentDescrs_->findClientPropertyDescription(child_uid);
				else
					propertyDescription = pComponentDescrs_->findBasePropertyDescription(child_uid);
			}
			else if (componentType_ == CELLAPP_TYPE)
			{
				if (EntityDef::context().currComponentType == CLIENT_TYPE)
					propertyDescription = pComponentDescrs_->findClientPropertyDescription(child_uid);
				else
					propertyDescription = pComponentDescrs_->findCellPropertyDescription(child_uid);
			}
			else if (componentType_ == CLIENT_TYPE || componentType_ == BOTS_TYPE)
				propertyDescription = pComponentDescrs_->findClientPropertyDescription(child_uid);
			else
				OURO_ASSERT(false);
		}
	}

	return propertyDescription;
}

//-------------------------------------------------------------------------------------
PyObject* EntityComponent::createCellData()
{
	return ((EntityComponentType*)pPropertyDescription_->getDataType())->createCellData();
}

//-------------------------------------------------------------------------------------
void EntityComponent::createFromDict(PyObject* pyDict, bool persistentData)
{
	// set to -1 to avoid attempting to broadcast properties in onScriptSetAttribute
	ENTITY_ID oid = ownerID_;
	ownerID_ = -1;

	const ScriptDefModule::PROPERTYDESCRIPTION_MAP* pPropertyDescrs = pChildPropertyDescrs();

	ScriptDefModule::PROPERTYDESCRIPTION_MAP::const_iterator iter = pPropertyDescrs->begin();
	for (; iter != pPropertyDescrs->end(); ++iter)
	{
		PropertyDescription* propertyDescription = iter->second;

		PyObject* pyVal = PyDict_GetItemString(pyDict, propertyDescription->getName());

		if (pyVal)
		{
			if (!propertyDescription->getDataType()->isSameType(pyVal))
			{
				CRITICAL_MSG(fmt::format("EntityComponent::createFromDict: {} type(curr_py: {} != {}) error! name={}, utype={}, owner={}, ownerID={}, domain={}.\n",
					propertyDescription->getName(), (pyVal ? pyVal->ob_type->tp_name : "unknown"), propertyDescription->getDataType()->getName(),
					pComponentDescrs_ ? pComponentDescrs_->getName() : "", pComponentDescrs_ ? pComponentDescrs_->getUType() : 0,
					(ownerID() > 0 ? owner()->ob_type->tp_name : "unknown"), ownerID(), COMPONENT_NAME_EX(componentType())));
			}
			else
			{
				PyObject_SetAttrString(static_cast<PyObject*>(this),
					propertyDescription->getName(), pyVal);
			}
		}
		else
		{
			if (persistentData)
			{
				SCRIPT_ERROR_CHECK();

				ERROR_MSG(fmt::format("EntityComponent::createFromDict: not found property({}), use default values! name={}, utype={}, owner={}, ownerID={}, domain={}.\n",
					propertyDescription->getName(), pComponentDescrs_ ? pComponentDescrs_->getName() : "", pComponentDescrs_ ? pComponentDescrs_->getUType() : 0,
					(ownerID() > 0 ? owner()->ob_type->tp_name : "unknown"), ownerID(), COMPONENT_NAME_EX(componentType())));
			}
			else
			{
				PyErr_Clear();
			}
		}
	}

	ownerID_ = oid;
}

//-------------------------------------------------------------------------------------
void EntityComponent::updateFromDict(PyObject* pOwner, PyObject* pyDict) 
{
	// set to -1 to avoid attempting to broadcast properties in onScriptSetAttribute
	ENTITY_ID oid = ownerID_;
	ownerID_ = -1;

	PyObject *key, *value;
	Py_ssize_t pos = 0;

	OURO_ASSERT(PyDict_Check(pyDict));

	PyObject* pyCellData = NULL;

	if (pOwner)
	{
		PyObject* cellDataDict = PyObject_GetAttrString(pOwner, "cellData");
		if (!cellDataDict)
		{
			PyErr_Clear();
		}
		else
		{
			pyCellData = PyDict_GetItemString(cellDataDict, pPropertyDescription_->getName());
			Py_DECREF(cellDataDict);
		}
	}

	const ScriptDefModule::PROPERTYDESCRIPTION_MAP* pPropertyDescrs = pChildPropertyDescrs();

	while (PyDict_Next(pyDict, &pos, &key, &value))
	{
		const char* ccattr = PyUnicode_AsUTF8AndSize(key, NULL);

		ScriptDefModule::PROPERTYDESCRIPTION_MAP::const_iterator iter = pPropertyDescrs->find(ccattr);
		if (iter != pPropertyDescrs->end())
		{
			PropertyDescription* propertyDescription = iter->second;

			if (!iter->second->getDataType()->isSameType(value))
			{
				CRITICAL_MSG(fmt::format("EntityComponent::updateFromDict: {} type(curr_py: {} != {}) error! name={}, utype={}, owner={}, ownerID={}, domain={}.\n",
					propertyDescription->getName(), (value ? value->ob_type->tp_name : "unknown"), propertyDescription->getDataType()->getName(),
					pComponentDescrs_ ? pComponentDescrs_->getName() : "", pComponentDescrs_ ? pComponentDescrs_->getUType() : 0,
					(pOwner ? pOwner->ob_type->tp_name : "unknown"), ownerID(), COMPONENT_NAME_EX(componentType())));
			}
			else
			{
				PyObject_SetAttrString(static_cast<PyObject*>(this),
					propertyDescription->getName(), value);
			}
		}

		if (pyCellData)
		{
			if (PyDict_Contains(pyCellData, key))
			{
				PyDict_SetItem(pyCellData, key, value);
			}
			else
			{
				PyObject_SetAttr(static_cast<PyObject*>(this),
					key, value);
			}
		}
		else
		{
			PyObject_SetAttr(static_cast<PyObject*>(this),
				key, value);
		}
	}

	SCRIPT_ERROR_CHECK();

	ownerID_ = oid;
}

//-------------------------------------------------------------------------------------
void EntityComponent::convertDictDataToEntityComponent(ENTITY_ID entityID, PyObject* pEntity, ScriptDefModule* pEntityScriptDescrs, PyObject* cellData, bool persistentData)
{
	ScriptDefModule::COMPONENTDESCRIPTION_MAP& componentDescrs = pEntityScriptDescrs->getComponentDescrs();

	ScriptDefModule::COMPONENTDESCRIPTION_MAP::iterator comps_iter = componentDescrs.begin();
	for (; comps_iter != componentDescrs.end(); ++comps_iter)
	{
		if (!comps_iter->second->getScriptType())
			continue;

		PyObject* pyObj = PyDict_GetItemString(cellData, comps_iter->first.c_str());
		if (!pyObj || !PyDict_Check(pyObj))
		{
			// Since there is a situation, there is no content in the component def, but there is a cell script. At this time, the baseapp cannot determine whether it has a cell attribute, so no data is written when writing celldata.
			if (g_componentType == BASEAPP_TYPE)
			{
				SCRIPT_ERROR_CHECK();
				continue;
			}
			else
			{
				PyErr_Clear();
			}
		}

		OURO_ASSERT(EntityDef::context().currEntityID > 0);

		PyObject* pyobj = comps_iter->second->createObject();

		// Execute the constructor of the Entity component
		PyObject* pyEntityComponent = new(pyobj) EntityComponent(entityID, comps_iter->second, g_componentType);

		EntityComponent* pEntityComponent = static_cast<EntityComponent*>(pyEntityComponent);

		if(pyObj)
			pEntityComponent->createFromDict(pyObj, persistentData);

		PropertyDescription* pPropertyDescription = pEntityScriptDescrs->findCellPropertyDescription(comps_iter->first.c_str());
		OURO_ASSERT(pPropertyDescription);

		pEntityComponent->pPropertyDescription(pPropertyDescription);
		pEntityComponent->updateOwner(entityID, pEntity);

		PyDict_SetItemString(cellData, comps_iter->first.c_str(), pEntityComponent);
		Py_DECREF(pEntityComponent);
	}
}

//-------------------------------------------------------------------------------------
std::vector<EntityComponent*> EntityComponent::getComponents(const std::string& name, PyObject* pEntity, ScriptDefModule* pEntityScriptDescrs)
{
	std::vector<EntityComponent*> founds;

	ScriptDefModule::COMPONENTDESCRIPTION_MAP& componentDescrs = pEntityScriptDescrs->getComponentDescrs();

	ScriptDefModule::COMPONENTDESCRIPTION_MAP::iterator comps_iter = componentDescrs.begin();
	for (; comps_iter != componentDescrs.end(); ++comps_iter)
	{
		if (name != comps_iter->second->getName())
			continue;

		if (g_componentType == BASEAPP_TYPE)
		{
			if (!comps_iter->second->hasBase())
				continue;
		}
		else if (g_componentType == CELLAPP_TYPE)
		{
			if (!comps_iter->second->hasCell())
				continue;
		}
		else
		{
			if (!comps_iter->second->hasClient())
				continue;
		}

		PyObject* pyObj = PyObject_GetAttrString(pEntity, comps_iter->first.c_str());
		if (pyObj)
		{
			founds.push_back((EntityComponent*)pyObj);
		}
		else
		{
			SCRIPT_ERROR_CHECK();
		}
	}

	return founds;
}

//-------------------------------------------------------------------------------------
PyObject* EntityComponent::pyGetCellEntityCall()
{
	PyObject* pEntity = owner();

	if (!pEntity)
	{
		PyErr_Format(PyExc_AssertionError, "%s: %d is destroyed!\n",
			scriptName(), ownerID_);
		PyErr_PrintEx(0);
		return 0;
	}

	PyObject* entityCall = PyObject_GetAttrString(pEntity, "cell");
	if (!entityCall)
	{
		PyErr_Format(PyExc_AttributeError, "'%s: %d' object has no attribute 'EntityComponent.cell'\n",
			pEntity->ob_type->tp_name, ownerID_);

		return NULL;
	}

	if (entityCall == Py_None)
	{
		// no need to dereference
		return entityCall;
	}

	PyObject* pyObj = PyObject_GetAttrString(entityCall, pPropertyDescription_->getName());
	Py_DECREF(entityCall);
	return pyObj;
}

//-------------------------------------------------------------------------------------
PyObject* EntityComponent::pyGetBaseEntityCall()
{
	PyObject* pEntity = owner();

	if (!pEntity)
	{
		PyErr_Format(PyExc_AssertionError, "%s: %d is destroyed!\n",
			scriptName(), ownerID_);
		PyErr_PrintEx(0);
		return 0;
	}

	PyObject* entityCall = PyObject_GetAttrString(pEntity, "base");
	if (!entityCall)
	{
		PyErr_Format(PyExc_AttributeError, "'%s: %d' object has no attribute 'EntityComponent.base'\n",
			pEntity->ob_type->tp_name, ownerID_);

		return NULL;
	}

	if (entityCall == Py_None)
	{
		// no need to dereference
		return entityCall;
	}

	PyObject* pyObj = PyObject_GetAttrString(entityCall, pPropertyDescription_->getName());
	Py_DECREF(entityCall);
	return pyObj;
}

//-------------------------------------------------------------------------------------
PyObject* EntityComponent::pyGetClientEntityCall()
{
	PyObject* pEntity = owner();

	if (!pEntity)
	{
		PyErr_Format(PyExc_AssertionError, "%s: %d is destroyed!\n",
			scriptName(), ownerID_);
		PyErr_PrintEx(0);
		return 0;
	}

	PyObject* entityCall = PyObject_GetAttrString(pEntity, "client");
	if (!entityCall)
	{
		PyErr_Format(PyExc_AttributeError, "'%s: %d' object has no attribute 'EntityComponent.client'\n",
			pEntity->ob_type->tp_name, ownerID_);

		return NULL;
	}

	if (entityCall == Py_None)
	{
		// no need to dereference
		return entityCall;
	}

	PyObject* pyObj = PyObject_GetAttrString(entityCall, pPropertyDescription_->getName());
	Py_DECREF(entityCall);
	return pyObj;
}

//-------------------------------------------------------------------------------------
PyObject* EntityComponent::pyGetAllClients()
{
	PyObject* pEntity = owner();

	if (!pEntity)
	{
		PyErr_Format(PyExc_AssertionError, "%s: %d is destroyed!\n",
			scriptName(), ownerID_);
		PyErr_PrintEx(0);
		return 0;
	}

	PyObject* entityCall = PyObject_GetAttrString(pEntity, "allClients");
	if (!entityCall)
	{
		PyErr_Format(PyExc_AttributeError, "'%s: %d' object has no attribute 'EntityComponent.allClients'\n",
			pEntity->ob_type->tp_name, ownerID_);

		return NULL;
	}

	if (entityCall == Py_None)
	{
		// no need to dereference
		return entityCall;
	}

	PyObject* pyObj = PyObject_GetAttrString(entityCall, pPropertyDescription_->getName());
	Py_DECREF(entityCall);
	return pyObj;
}

//-------------------------------------------------------------------------------------
PyObject* EntityComponent::pyGetOtherClients()
{
	PyObject* pEntity = owner();

	if (!pEntity)
	{
		PyErr_Format(PyExc_AssertionError, "%s: %d is destroyed!\n",
			scriptName(), ownerID_);
		PyErr_PrintEx(0);
		return 0;
	}

	PyObject* entityCall = PyObject_GetAttrString(pEntity, "otherClients");
	if (!entityCall)
	{
		PyErr_Format(PyExc_AttributeError, "'%s: %d' object has no attribute 'EntityComponent.otherClients'\n",
			pEntity->ob_type->tp_name, ownerID_);

		return NULL;
	}

	if (entityCall == Py_None)
	{
		// no need to dereference
		return entityCall;
	}

	PyObject* pyObj = PyObject_GetAttrString(entityCall, pPropertyDescription_->getName());
	Py_DECREF(entityCall);
	return pyObj;
}

//-------------------------------------------------------------------------------------
PyObject* EntityComponent::pyClientEntity(ENTITY_ID entityID)
{
	PyObject* pEntity = owner();

	if (!pEntity)
	{
		PyErr_Format(PyExc_AssertionError, "%s: %d is destroyed!\n",
			scriptName(), ownerID_);
		PyErr_PrintEx(0);
		return 0;
	}

	PyObject* pyResult = PyObject_CallMethod(this, const_cast<char*>("clientEntity"),
		const_cast<char*>("i"), entityID);

	if (pyResult != NULL)
	{
		PyObject* pObj =  PyObject_GetAttrString(pyResult, pPropertyDescription_->getName());
		Py_DECREF(pyResult);
		return pObj;
	}

	return NULL;
}

//-------------------------------------------------------------------------------------
PyObject* EntityComponent::pyAddTimer(float interval, float repeat, int32 userArg)
{
	PyObject* pEntity = owner();

	if (!pEntity)
	{
		PyErr_Format(PyExc_AssertionError, "%s: %d is destroyed!\n",
			scriptName(), ownerID_);
		PyErr_PrintEx(0);
		return 0;
	}

	return PyObject_CallMethod(pEntity, const_cast<char*>("addTimer"),
		const_cast<char*>("ffi"), interval, repeat, userArg);
}

//-------------------------------------------------------------------------------------
PyObject* EntityComponent::pyDelTimer(PyObject_ptr args)
{
	PyObject* pEntity = owner();

	if (!pEntity)
	{
		PyErr_Format(PyExc_AssertionError, "%s: %d is destroyed!\n",
			scriptName(), ownerID_);
		PyErr_PrintEx(0);
		return 0;
	}

	ScriptID timerID = 0;

	if (args == NULL)
	{																									
		PyErr_Format(PyExc_TypeError, 
			"%s::delTimer: args(id|int or \"All\"|str) error!", 
			scriptName());																		
		
		PyErr_PrintEx(0);																				
		return PyLong_FromLong(-1);																		
	}	

	if (PyUnicode_Check(args))
	{																									
		if (strcmp(PyUnicode_AsUTF8AndSize(args, NULL), "All") == 0)
		{																								
			return PyObject_CallMethod(pEntity, const_cast<char*>("delTimer"),
				const_cast<char*>("s"), "All");
		}																								
		else																							
		{																								
			PyErr_Format(PyExc_TypeError, 
				"%s::delTimer: args not is \"All\"!", 
				scriptName());																	
			
			PyErr_PrintEx(0);																			
			return PyLong_FromLong(-1);																	
		}																								
			
		return PyLong_FromLong(0);																		
	}																									
	else                                                                                                
	{																								
		if (!PyLong_Check(args))
		{																								
			PyErr_Format(PyExc_TypeError, 
				"%s::delTimer: args(id|int) error!", 
				scriptName());																	
			
			PyErr_PrintEx(0);																			
			return PyLong_FromLong(-1);																	
		}																								
			
		timerID = PyLong_AsLong(args);
	}																									
		
	return PyObject_CallMethod(pEntity, const_cast<char*>("delTimer"),
		const_cast<char*>("I"), timerID);
}

//-------------------------------------------------------------------------------------

}
