// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com


#ifndef OURO_PY_MEMORYSTREAM_H
#define OURO_PY_MEMORYSTREAM_H

#include "scriptobject.h"
#include "helper/debug_helper.h"
#include "common/common.h"
#include "common/memorystream.h"

namespace Ouroboros{ namespace script{

class PyMemoryStream : public ScriptObject
{
	/** Subclassing fills some py operations into derived classes */
	INSTANCE_SCRIPT_HREADER(PyMemoryStream, ScriptObject)
public:
	static PySequenceMethods seqMethods;

	PyMemoryStream(bool readonly = false);
	PyMemoryStream(std::string& strInitData, bool readonly = false);
	PyMemoryStream(PyObject* pyBytesInitData, bool readonly = false);
	PyMemoryStream(MemoryStream* streamInitData, bool readonly = false);

	PyMemoryStream(PyTypeObject* pyType, bool isInitialised = false, bool readonly = false);
	virtual ~PyMemoryStream();


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

	static PyObject* py_new();

	/**
	Initialize the fixed dictionary
	*/
	void initialize(std::string strDictInitData);
	void initialize(PyObject* pyDictInitData);
	void initialize(MemoryStream* streamInitData);

	INLINE MemoryStream& stream();

	INLINE PyObject* pyBytes();

	INLINE bool readonly() const;

	void addToStream(MemoryStream* mstream);

	void createFromStream(MemoryStream* mstream);

	/**
		Get the description of the object 
	*/
	PyObject* tp_repr();
	PyObject* tp_str();

	static PyObject* __py_append(PyObject* self, PyObject* args, PyObject* kwargs);
	static PyObject* __py_pop(PyObject* self, PyObject* args, PyObject* kwargs);

	static Py_ssize_t seq_length(PyObject* self);
	INLINE int length(void) const;

	static PyObject* __py_bytes(PyObject* self, PyObject* args, PyObject* kwargs);

	static PyObject* __py_rpos(PyObject* self, PyObject* args, PyObject* kwargs);
	static PyObject* __py_wpos(PyObject* self, PyObject* args, PyObject* kwargs);

	static PyObject* __py_fill(PyObject* self, PyObject* args, PyObject* kwargs);

protected:
	MemoryStream stream_;
	bool readonly_;
} ;

}
}

#ifdef CODE_INLINE
#include "py_memorystream.inl"
#endif

#endif // OURO_PY_MEMORYSTREAM_H
