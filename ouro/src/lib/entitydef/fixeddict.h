// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_FIXED_DICT_H
#define OURO_FIXED_DICT_H

#include <string>
#include "datatype.h"
#include "helper/debug_helper.h"
#include "common/common.h"
#include "pyscript/map.h"
#include "pyscript/pickler.h"

namespace Ouroboros{

class FixedDict : public script::Map
{
	/** Subclassing fills some py operations into derived classes */
	INSTANCE_SCRIPT_HREADER(FixedDict, Map)
public:
	static PyMappingMethods mappingMethods;
	static PySequenceMethods mappingSequenceMethods;

	FixedDict(DataType* dataType);
	FixedDict(DataType* dataType, bool isPersistentsStream);

	virtual ~FixedDict();

	DataType* getDataType(void){ return _dataType; }

	/**
		Supports pickler method
	*/
	static PyObject* __py_reduce_ex__(PyObject* self, PyObject* protocol);

	/**
		Unpickle method
	*/
	static PyObject* __unpickle__(PyObject* self, PyObject* args);

	/**
		Script is called when it is installed
	*/
	static void onInstallScript(PyObject* mod);

	/**
		Map operation function related
	*/
	static PyObject* mp_subscript(PyObject* self, PyObject* key);

	static int mp_ass_subscript(PyObject* self, PyObject* key,
		PyObject* value);

	static int mp_length(PyObject* self);

	/**
		Initialize the fixed dictionary
	*/
	void initialize(std::string strDictInitData);
	void initialize(PyObject* pyDictInitData);
	void initialize(MemoryStream* streamInitData, bool isPersistentsStream);

	/**
		Check data changes
	*/
	bool checkDataChanged(const char* keyName,
		PyObject* value,
		bool isDelete = false);

	/**
		Update dictionary data to your own data
	*/
	PyObject* update(PyObject* args);

	/**
		Get the description of the object 
	*/
	PyObject* tp_repr();
	PyObject* tp_str();

protected:
	FixedDictType* _dataType;
} ;

}
#endif // OURO_FIXED_DICT_H
