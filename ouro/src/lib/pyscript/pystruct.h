// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_SCRIPT_STRUCT_H
#define OURO_SCRIPT_STRUCT_H

#include "common/common.h"
#include "scriptobject.h"

namespace Ouroboros{ namespace script{

class PyStruct
{						
public:	
	/** 
		Proxy struct.pack
	*/
	static std::string pack(PyObject* fmt, PyObject* args);

	/** 
		Proxy struct.unpack
	*/
	static PyObject* unpack(PyObject* fmt, PyObject* args);

	/** 
		Initialize pickler
	*/
	static bool initialize(void);
	static void finalise(void);
	
private:
	static PyObject* pack_; // struct.pack method pointer
	static PyObject* unpack_; // struct.unpack method pointer

	static bool isInit; // whether it has been initialized
} ;

}
}
#endif // OURO_SCRIPT_STRUCT_H
