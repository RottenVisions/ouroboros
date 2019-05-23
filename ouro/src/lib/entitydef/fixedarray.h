// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com


#ifndef _FIXED_ARRAY_TYPE_H
#define _FIXED_ARRAY_TYPE_H
#include <string>
#include "datatype.h"
#include "pyscript/sequence.h"
#include "pyscript/pickler.h"

namespace Ouroboros{

class FixedArray : public script::Sequence
{		
		/** Subclassing populates some py operations into derived classes*/
	INSTANCE_SCRIPT_HREADER(FixedArray, Sequence)

public:	
	FixedArray(DataType* dataType);
	virtual ~FixedArray();

	const DataType* getDataType(void){ return _dataType; }
	
	/** 
		Initialize a fixed array
	*/
	void initialize(std::string strInitData);
	void initialize(PyObject* pyObjInitData);

	/** 
		Support for the pickler method
	*/
	static PyObject* __py_reduce_ex__(PyObject* self, PyObject* protocol);

	/** 
		Unpick method
	*/
	static PyObject* __unpickle__(PyObject* self, PyObject* args);
	
	/** 
		Called when the script is installed
	*/
	static void onInstallScript(PyObject* mod);
	
	/** 
		The operation interface required for a list
	*/
	static PyObject* __py_append(PyObject* self, PyObject* args, PyObject* kwargs);	
	static PyObject* __py_count(PyObject* self, PyObject* args, PyObject* kwargs);
	static PyObject* __py_extend(PyObject* self, PyObject* args, PyObject* kwargs);	
	static PyObject* __py_index(PyObject* self, PyObject* args, PyObject* kwargs);
	static PyObject* __py_insert(PyObject* self, PyObject* args, PyObject* kwargs);	
	static PyObject* __py_pop(PyObject* self, PyObject* args, PyObject* kwargs);
	static PyObject* __py_remove(PyObject* self, PyObject* args, PyObject* kwargs);
	static PyObject* __py_clear(PyObject* self, PyObject* args, PyObject* kwargs);
	
	bool isSameType(PyObject* pyValue);
	bool isSameItemType(PyObject* pyValue);

	virtual PyObject* createNewItemFromObj(PyObject* pyItem);

	/** 
		Get the description of the object
	*/
	PyObject* tp_repr();
	PyObject* tp_str();

protected:
	FixedArrayType* _dataType;
} ;

}
#endif
