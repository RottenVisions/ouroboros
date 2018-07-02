// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef _SCRIPT_MAP_H
#define _SCRIPT_MAP_H
#include "common/common.h"
#include "scriptobject.h"
#include "pickler.h"

namespace Ouroboros{ namespace script{

class Map : public ScriptObject
{
	/** Subclassing fills some py operations into derived classes */
	INSTANCE_SCRIPT_HREADER(Map, ScriptObject)

public:
	static PyMappingMethods mappingMethods;
	static PySequenceMethods mappingSequenceMethods;

	Map(PyTypeObject* pyType, bool isInitialised = false);
	virtual ~Map();

	/**
		Expose some dictionary methods to python
	*/
	static PyObject* __py_has_key(PyObject* self, PyObject* args);
	static PyObject* __py_keys(PyObject* self, PyObject* args);
	static PyObject* __py_values(PyObject* self, PyObject* args);
	static PyObject* __py_items(PyObject* self, PyObject* args);
	static PyObject* __py_update(PyObject* self, PyObject* args);
	static PyObject* __py_get(PyObject* self, PyObject* args);

	static int seq_contains(PyObject* self, PyObject* value);

	/**
		Map operation function related
	*/
	static PyObject* mp_subscript(PyObject* self, PyObject* key);

	static int mp_ass_subscript(PyObject* self,
		PyObject* key, PyObject* value);

	static int mp_length(PyObject* self);

	/**
		Get dictionary object
	*/
	INLINE PyObject* getDictObject(void) const;

	/**
		Data change notification
	*/
	virtual void onDataChanged(PyObject* key, PyObject* value,
		bool isDelete = false);

protected:
	// Dictionary data, all data is written here
	PyObject* pyDict_;
} ;

}
}

#ifdef CODE_INLINE
#include "map.inl"
#endif

#endif
