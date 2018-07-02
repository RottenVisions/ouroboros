// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_SCRIPT_STRUCT_H
#define OURO_SCRIPT_STRUCT_H

#include "common/common.h"
#include "scriptobject.h"

namespace Ouroboros{ namespace script{

class PyStruct
{
public:
	/**
		proxy struct.pack
	*/
	static std::string pack(PyObject* fmt, PyObject* args);

	/**
		proxy struct.unpack
	*/
	static PyObject* unpack(PyObject* fmt, PyObject* args);

	/**
		Initialize pickler
	*/
	static bool initialize(void);
	static void finalise(void);

private:
	static PyObject* pack_;									// struct.Pack method pointer
	static PyObject* unpack_;								// struct.Unpack method pointer

	static bool	isInit;										// Has it been initialized
} ;

}
}
#endif // OURO_SCRIPT_STRUCT_H
