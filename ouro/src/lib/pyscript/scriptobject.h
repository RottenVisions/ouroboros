// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_SCRIPTOBJECT_H
#define OURO_SCRIPTOBJECT_H

#include <vector>	
#include "Python.h"     
#include "py_macros.h" 
#include "helper/debug_helper.h"
#include "common/common.h"
#include <structmember.h>
#include <assert.h>

namespace Ouroboros{ namespace script{

// python's default null return value
#define S_Return { Py_INCREF(Py_None); return Py_None; }			
#define S_RETURN S_Return	
#define PY_RETURN S_Return	

#define S_INCREF(pyObj)																		\
	if(pyObj){																				\
		Py_INCREF(pyObj);																	\
	}																						\

#define S_DECREF(pyObj)																		\
	if(pyObj){																				\
		Py_DECREF(pyObj);																	\
	}																						\

// python object release
#define S_RELEASE(pyObj)																	\
	if(pyObj){																				\
		Py_DECREF(pyObj);																	\
		pyObj = NULL;																		\
	}																						\

/// Output the error message generated by the current script
#define SCRIPT_ERROR_CHECK()																\
{																							\
 	if (PyErr_Occurred())																	\
 	{																						\
		PyErr_PrintEx(0);																	\
	}																						\
}

// script object header (usually the object generated by python default allocation object method)
#define SCRIPT_OBJECT_HREADER(CLASS, SUPERCLASS)											\
	SCRIPT_HREADER_BASE(CLASS, SUPERCLASS);													\
	/** Objects created by python are released from python
	*/																						\
	static void _tp_dealloc(PyObject* self)													\
	{																						\
		CLASS::_scriptType.tp_free(self);													\
	}																						\

// base script object header (this module is usually provided as a base class for inheritance in python scripts)
#define BASE_SCRIPT_HREADER(CLASS, SUPERCLASS)												\
	SCRIPT_HREADER_BASE(CLASS, SUPERCLASS);													\
	/** Objects created by python are released from python
	*/																						\
	static void _tp_dealloc(PyObject* self)													\
	{																						\
		static_cast<CLASS*>(self)->~CLASS();												\
		CLASS::_scriptType.tp_free(self);													\
	}																						\

// instance script object header (this script object is generated by c++)
#define INSTANCE_SCRIPT_HREADER(CLASS, SUPERCLASS)											\
	SCRIPT_HREADER_BASE(CLASS, SUPERCLASS);													\
	/** The object created by c++new is deleted.
	*/																						\
	static void _tp_dealloc(PyObject* self)													\
	{																						\
		delete static_cast<CLASS*>(self);													\
	}																						\

																							
#define SCRIPT_HREADER_BASE(CLASS, SUPERCLASS)												\
		/*Current script module category*/																	\
	static PyTypeObject _scriptType;														\
	typedef CLASS ThisClass;																\
																							\
	static PyObject* _tp_repr(PyObject* self)												\
	{																						\
		return static_cast<CLASS*>(self)->tp_repr();										\
	}																						\
																							\
	static PyObject* _tp_str(PyObject* self)												\
	{																						\
		return static_cast<CLASS*>(self)->tp_str();											\
	}																						\
																							\
	/** Script module object created from python
	*/																						\
	static PyObject* _tp_new(PyTypeObject* type, PyObject* args, PyObject* kwds)			\
	{																						\
		return CLASS::tp_new(type, args, kwds);												\
	}																						\
																							\
	/** python request to get the properties or methods of this module
	*/																						\
	static PyObject* _tp_getattro(PyObject* self, PyObject* name)							\
	{																						\
		return static_cast<CLASS*>(self)->onScriptGetAttribute(name);						\
	}																						\
																							\
	/** python request to set the properties or methods of this module
	*/																						\
	static int _tp_setattro(PyObject* self, PyObject* name, PyObject* value)				\
	{																						\
		return (value != NULL) ?															\
				static_cast<CLASS*>(self)->onScriptSetAttribute(name, value):				\
				static_cast<CLASS*>(self)->onScriptDelAttribute(name);						\
	}																						\
																							\
	/** python request to initialize this module object
	*/																						\
	static int _tp_init(PyObject* self, PyObject *args, PyObject* kwds)						\
	{																						\
		return static_cast<CLASS*>(self)->onScriptInit(self, args, kwds);					\
	}																						\
																							\
public:																						\
		/*Method and member storage list that will eventually be installed into the script module*/											\
	static PyMethodDef* _##CLASS##_lpScriptmethods;											\
	static PyMemberDef* _##CLASS##_lpScriptmembers;											\
	static PyGetSetDef* _##CLASS##_lpgetseters;												\
		/*The methods and members of this module that are to be leaked to the script will eventually be imported into the list of 2 pointers above.*/					\
	static PyMethodDef _##CLASS##_scriptMethods[];											\
	static PyMemberDef _##CLASS##_scriptMembers[];											\
	static PyGetSetDef _##CLASS##_scriptGetSeters[];										\
																							\
	static bool _##CLASS##_py_installed;													\
																							\
	/** read-only property of getset
	*/																						\
	static int __py_readonly_descr(PyObject* self, PyObject* value, void* closure)			\
	{																						\
		PyErr_Format(PyExc_TypeError,														\
		"Sorry, this attribute %s.%s is read-only", (self != NULL ? self->ob_type->tp_name\
		: #CLASS), (closure != NULL ? (char*)closure : "unknown"));							\
		PyErr_PrintEx(0);																	\
		return 0;																			\
	}																						\
																							\
	/** getset's write-only property
	*/																						\
	static int __py_writeonly_descr(PyObject* self, PyObject* value, void* closure)			\
	{																						\
		PyErr_Format(PyExc_TypeError,														\
		"Sorry, this attribute %s.%s is write-only", (self != NULL ? self->ob_type->tp_name\
		: #CLASS), (closure != NULL ? (char*)(closure) : "unknown"));						\
		PyErr_PrintEx(0);																	\
		return 0;																			\
	}																						\
																							\
	/** This interface can get the script category of the current module
	*/																						\
	static PyTypeObject* getScriptType(void)												\
	{																						\
		return &_scriptType;																\
	}																						\
	static PyTypeObject* getBaseScriptType(void)											\
	{																						\
		if(strcmp("ScriptObject", #SUPERCLASS) == 0)										\
			return 0;																		\
		return SUPERCLASS::getScriptType();													\
	}																						\
																							\
	static long calcDictOffset(void)														\
	{																						\
		if(strcmp("ScriptObject", #SUPERCLASS) == 0)										\
			return 0;																		\
		return 0;																			\
	}																						\
																							\
	/** Calculate the number of exposed methods for all inherited modules
	*/																						\
	static int calcTotalMethodCount(void)													\
	{																						\
		int nlen = 0;																		\
		while(true)																			\
		{																					\
			PyMethodDef* pmf = &_##CLASS##_scriptMethods[nlen];								\
			if(!pmf->ml_doc && !pmf->ml_flags && !pmf->ml_meth && !pmf->ml_name)			\
				break;																		\
			nlen++;																			\
		}																					\
																							\
		if(strcmp(#CLASS, #SUPERCLASS) == 0)												\
			return nlen;																	\
		return SUPERCLASS::calcTotalMethodCount() + nlen;									\
	}																						\
																							\
	/** Calculates the number of exposed members of all inherited modules
	*/																						\
	static int calcTotalMemberCount(void)													\
	{																						\
		int nlen = 0;																		\
		while(true)																			\
		{																					\
			PyMemberDef* pmd = &_##CLASS##_scriptMembers[nlen];								\
			if(!pmd->doc && !pmd->flags && !pmd->type && !pmd->name && !pmd->offset)		\
				break;																		\
			nlen++;																			\
		}																					\
																							\
		if(strcmp(#CLASS, #SUPERCLASS) == 0)												\
			return nlen;																	\
		return SUPERCLASS::calcTotalMemberCount() + nlen;									\
	}																						\
																							\
	/** Calculates the number of exposed getets for all inherited modules
	*/																						\
	static int calcTotalGetSetCount(void)													\
	{																						\
		int nlen = 0;																		\
		while(true)																			\
		{																					\
			PyGetSetDef* pgs = &_##CLASS##_scriptGetSeters[nlen];							\
			if(!pgs->doc && !pgs->get && !pgs->set && !pgs->name && !pgs->closure)			\
				break;																		\
			nlen++;																			\
		}																					\
																							\
		if(strcmp(#CLASS, #SUPERCLASS) == 0)												\
			return nlen;																	\
		return SUPERCLASS::calcTotalGetSetCount() + nlen;									\
	}																						\
																							\
	/** Installs all parent classes and exposed members and methods of the current module into the list of scripts that will eventually be imported
	*/																						\
	static void setupScriptMethodAndAttribute(PyMethodDef* lppmf, PyMemberDef* lppmd,		\
	PyGetSetDef* lppgs)																		\
	{																						\
		int i = 0;																			\
		PyMethodDef* pmf = NULL;															\
		PyMemberDef* pmd = NULL;															\
		PyGetSetDef* pgs = NULL;															\
																							\
		while(true){																		\
			pmf = &_##CLASS##_scriptMethods[i];												\
			if(!pmf->ml_doc && !pmf->ml_flags && !pmf->ml_meth && !pmf->ml_name)			\
				break;																		\
			i++;																			\
			*(lppmf++) = *pmf;																\
		}																					\
																							\
		i = 0;																				\
		while(true){																		\
			pmd = &_##CLASS##_scriptMembers[i];												\
			if(!pmd->doc && !pmd->flags && !pmd->type && !pmd->name && !pmd->offset)		\
				break;																		\
			i++;																			\
			*(lppmd++) = *pmd;																\
		}																					\
																							\
		i = 0;																				\
		while(true){																		\
			pgs = &_##CLASS##_scriptGetSeters[i];											\
			if(!pgs->doc && !pgs->get && !pgs->set && !pgs->name && !pgs->closure)			\
				break;																		\
			i++;																			\
			*(lppgs++) = *pgs;																\
		}																					\
																							\
		if(strcmp(#CLASS, #SUPERCLASS) == 0){												\
			*(lppmf) = *pmf;																\
			*(lppmd) = *pmd;																\
			*(lppgs) = *pgs;																\
			return;																			\
		}																					\
																							\
		SUPERCLASS::setupScriptMethodAndAttribute(lppmf, lppmd, lppgs);						\
	}																						\
																							\
	/** Registration Script Module
		@param mod: the main module to be imported
	*/																						\
	static void registerScript(PyObject* mod, const char* name = #CLASS)					\
	{																						\
		int nMethodCount = CLASS::calcTotalMethodCount();									\
		int nMemberCount = CLASS::calcTotalMemberCount();									\
		int nGetSetCount = CLASS::calcTotalGetSetCount();									\
																							\
		_##CLASS##_lpScriptmethods = new PyMethodDef[nMethodCount + 2];						\
		_##CLASS##_lpScriptmembers	= new PyMemberDef[nMemberCount + 2];					\
		_##CLASS##_lpgetseters		= new PyGetSetDef[nGetSetCount + 2];					\
																							\
		setupScriptMethodAndAttribute(_##CLASS##_lpScriptmethods,							\
									  _##CLASS##_lpScriptmembers,							\
									  _##CLASS##_lpgetseters);								\
																							\
		_scriptType.tp_methods		= _##CLASS##_lpScriptmethods;							\
		_scriptType.tp_members		= _##CLASS##_lpScriptmembers;							\
		_scriptType.tp_getset		= _##CLASS##_lpgetseters;								\
																							\
		if (PyType_Ready(&_scriptType) < 0){												\
			ERROR_MSG("registerScript(): PyType_Ready(" #CLASS ") error!");					\
			PyErr_Print();																	\
			return;																			\
		}																					\
																							\
		if(mod)																				\
		{																					\
			Py_INCREF(&_scriptType);														\
			if(PyModule_AddObject(mod, name, (PyObject *)&_scriptType) < 0)					\
			{																				\
				ERROR_MSG(fmt::format("registerScript(): PyModule_AddObject({}) error!", name));\
			}																				\
		}																					\
																							\
		SCRIPT_ERROR_CHECK();																\
		_##CLASS##_py_installed = true;														\
	}																						\
																							\
	/** Install the current script module
		@param mod: the main module to be imported
	*/																						\
	static void installScript(PyObject* mod, const char* name = #CLASS)						\
	{																						\
		CLASS::onInstallScript(mod);														\
																							\
		registerScript(mod, name);															\
		ScriptObject::scriptObjectTypes[name] = &_scriptType;								\
	}																						\
																							\
	/** Logout script module
	*/																						\
	static void unregisterScript(void)														\
	{																						\
		SAFE_RELEASE_ARRAY(_##CLASS##_lpScriptmethods);										\
		SAFE_RELEASE_ARRAY(_##CLASS##_lpScriptmembers);										\
		SAFE_RELEASE_ARRAY(_##CLASS##_lpgetseters);											\
																							\
		if(_##CLASS##_py_installed)															\
			Py_DECREF(&_scriptType);														\
	}																						\
																							\
	/** Uninstall the current script module
	*/																						\
	static void uninstallScript(void)														\
	{																						\
		CLASS::onUninstallScript();															\
		unregisterScript();																	\
	}																						\




/** This macro officially initializes a script module and populates the necessary information into the python type object.
*/
#define SCRIPT_INIT(CLASS, CALL, SEQ, MAP, ITER, ITERNEXT)									\
		TEMPLATE_SCRIPT_INIT(;,CLASS, CLASS, CALL, SEQ, MAP, ITER, ITERNEXT)				\


#define TEMPLATE_SCRIPT_INIT(TEMPLATE_HEADER, TEMPLATE_CLASS,								\
	CLASS, CALL, SEQ, MAP, ITER, ITERNEXT)													\
	TEMPLATE_HEADER PyMethodDef* TEMPLATE_CLASS::_##CLASS##_lpScriptmethods = NULL;			\
	TEMPLATE_HEADER PyMemberDef* TEMPLATE_CLASS::_##CLASS##_lpScriptmembers = NULL;			\
	TEMPLATE_HEADER PyGetSetDef* TEMPLATE_CLASS::_##CLASS##_lpgetseters = NULL;				\
																							\
	TEMPLATE_HEADER																			\
	PyTypeObject TEMPLATE_CLASS::_scriptType =												\
	{																						\
		PyVarObject_HEAD_INIT(&PyType_Type, 0)												\
		#CLASS,													/* tp_name            */	\
		sizeof(TEMPLATE_CLASS),									/* tp_basicsize       */	\
		0,														/* tp_itemsize        */	\
		(destructor)TEMPLATE_CLASS::_tp_dealloc,				/* tp_dealloc         */	\
		0,														/* tp_print           */	\
		0,														/* tp_getattr         */	\
		0,														/* tp_setattr         */	\
		0,														/* tp_compare         */	\
		TEMPLATE_CLASS::_tp_repr,								/* tp_repr            */	\
		0,														/* tp_as_number       */	\
		SEQ,													/* tp_as_sequence     */	\
		MAP,													/* tp_as_mapping      */	\
		0,														/* tp_hash            */	\
		CALL,													/* tp_call            */	\
		TEMPLATE_CLASS::_tp_str,								/* tp_str             */	\
		(getattrofunc)CLASS::_tp_getattro,						/* tp_getattro        */	\
		(setattrofunc)CLASS::_tp_setattro,						/* tp_setattro        */	\
		0,														/* tp_as_buffer       */	\
		Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,				/* tp_flags           */	\
		"Ouroboros::" #CLASS " objects.",						/* tp_doc             */	\
		0,														/* tp_traverse        */	\
		0,														/* tp_clear           */	\
		0,														/* tp_richcompare     */	\
		0,														/* tp_weaklistoffset  */	\
		(getiterfunc)ITER,										/* tp_iter            */	\
		(iternextfunc)ITERNEXT,									/* tp_iternext        */	\
		0,														/* tp_methods         */	\
		0,														/* tp_members         */	\
		0,														/* tp_getset          */	\
		TEMPLATE_CLASS::getBaseScriptType(),					/* tp_base            */	\
		0,														/* tp_dict            */	\
		0,														/* tp_descr_get       */	\
		0,														/* tp_descr_set       */	\
		TEMPLATE_CLASS::calcDictOffset(),						/* tp_dictoffset      */	\
		(initproc)TEMPLATE_CLASS::_tp_init,						/* tp_init            */	\
		0,														/* tp_alloc           */	\
		TEMPLATE_CLASS::_tp_new,								/* tp_new             */	\
		PyObject_GC_Del,										/* tp_free            */	\
	};																						\

// BASE_SCRIPT_HREADER base class script initialization, this class is inherited by the script
#define BASE_SCRIPT_INIT(CLASS, CALL, SEQ, MAP, ITER, ITERNEXT)								\
	PyMethodDef* CLASS::_##CLASS##_lpScriptmethods = NULL;									\
	PyMemberDef* CLASS::_##CLASS##_lpScriptmembers = NULL;									\
	PyGetSetDef* CLASS::_##CLASS##_lpgetseters = NULL;										\
																							\
	PyTypeObject CLASS::_scriptType =														\
	{																						\
		PyVarObject_HEAD_INIT(NULL, 0)														\
		#CLASS,													/* tp_name            */	\
		sizeof(CLASS),											/* tp_basicsize       */	\
		0,														/* tp_itemsize        */	\
		(destructor)CLASS::_tp_dealloc,							/* tp_dealloc         */	\
		0,														/* tp_print           */	\
		0,														/* tp_getattr         */	\
		0,														/* tp_setattr         */	\
		0,														/* void *tp_reserved  */	\
		CLASS::_tp_repr,										/* tp_repr            */	\
		0,														/* tp_as_number       */	\
		SEQ,													/* tp_as_sequence     */	\
		MAP,													/* tp_as_mapping      */	\
		0,														/* tp_hash            */	\
		CALL,													/* tp_call            */	\
		CLASS::_tp_str,											/* tp_str             */	\
		(getattrofunc)CLASS::_tp_getattro,						/* tp_getattro        */	\
		(setattrofunc)CLASS::_tp_setattro,						/* tp_setattro        */	\
		0,														/* tp_as_buffer       */	\
		Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,				/* tp_flags           */	\
		0,														/* tp_doc             */	\
		0,														/* tp_traverse        */	\
		0,														/* tp_clear           */	\
		0,														/* tp_richcompare     */	\
		0,														/* tp_weaklistoffset  */	\
		(getiterfunc)ITER,										/* tp_iter            */	\
		(iternextfunc)ITERNEXT,									/* tp_iternext        */	\
		0,														/* tp_methods         */	\
		0,														/* tp_members         */	\
		0,														/* tp_getset          */	\
		CLASS::getBaseScriptType(),								/* tp_base            */	\
		0,														/* tp_dict            */	\
		0,														/* tp_descr_get       */	\
		0,														/* tp_descr_set       */	\
		CLASS::calcDictOffset(),								/* tp_dictoffset      */	\
		0,														/* tp_init            */	\
		0,														/* tp_alloc           */	\
		0,														/* tp_new             */	\
		PyObject_GC_Del,										/* tp_free            */	\
		0,														/* tp_is_gc           */	\
		0,														/* tp_bases           */	\
		0,														/* tp_mro             */	\
		0,														/* tp_cache           */	\
		0,														/* tp_subclasses      */	\
		0,														/* tp_weaklist        */	\
		0,														/* tp_del			  */	\
	};																						\


class ScriptObject: public PyObject
{
	/** 
		Subclassing populates some py operations into a derived class
	*/
	SCRIPT_OBJECT_HREADER(ScriptObject, ScriptObject)							
public:	
	ScriptObject(PyTypeObject* pyType, bool isInitialised = false);
	~ScriptObject();

	// all ouro script categories
	typedef OUROUnordered_map<std::string, PyTypeObject*> SCRIPTOBJECT_TYPES;
	static SCRIPTOBJECT_TYPES scriptObjectTypes;
	static PyTypeObject* getScriptObjectType(const std::string& name);

	/** 
		Script object reference count
	*/
	void incRef() const				{ Py_INCREF((PyObject*)this); }
	void decRef() const				{ Py_DECREF((PyObject*)this); }

	int refCount() const			{ return int(((PyObject*)this)->ob_refcnt); }
	
	/** 
		Get the description of the object
	*/
	PyObject* tp_repr();
	PyObject* tp_str();

	DECLARE_PY_GET_MOTHOD(py__module__);
	DECLARE_PY_GET_MOTHOD(py__qualname__);
	DECLARE_PY_GET_MOTHOD(py__name__);

	/** 
		The script requests to create one of the objects
	*/
	static PyObject* tp_new(PyTypeObject* type, PyObject* args, 
		PyObject* kwds);

	/** 
		Script request to get property or method
	*/
	PyObject* onScriptGetAttribute(PyObject* attr);						

	/** 
		Script request setting property or method
	*/
	int onScriptSetAttribute(PyObject* attr, PyObject* value);			

	/** 
		The script requests to delete an attribute
	*/
	int onScriptDelAttribute(PyObject* attr);

	/** 
		Script request initialization
	*/
	int onScriptInit(PyObject* self, PyObject *args, 
		PyObject* kwds);

	/** 
		Get object category name
	*/
	const char* scriptName() const { return ob_type->tp_name; }

	/** 
		Called when the script is installed
	*/
	static void onInstallScript(PyObject* mod) {}

	/** 
		Called when the script is unloaded
	*/
	static void onUninstallScript() {}
} ;

}
}


#ifdef CODE_INLINE
#include "scriptobject.inl"
#endif

#endif // OURO_SCRIPTOBJECT_H
