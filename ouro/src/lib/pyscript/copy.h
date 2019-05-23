// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_SCRIPT_COPY_H
#define OURO_SCRIPT_COPY_H

#include "common/common.h"
#include "scriptobject.h"

namespace Ouroboros{ namespace script{

class Copy
{						
public:	
		/** Proxy copy.copy*/
	static PyObject* copy(PyObject* pyobj);
	static PyObject* deepcopy(PyObject* pyobj);

		/** Initialize copy*/
	static bool initialize(void);
	static void finalise(void);

private:
	static PyObject* copyMethod_;
	static PyObject* deepcopyMethod_;
	static bool isInit; // whether it has been initialized
} ;

}
}
#endif // OURO_SCRIPT_COPY_H
