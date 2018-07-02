// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_SCRIPT_COPY_H
#define OURO_SCRIPT_COPY_H

#include "common/common.h"
#include "scriptobject.h"

namespace Ouroboros{ namespace script{

class Copy
{
public:
	/** proxy copy.copy */
	static PyObject* copy(PyObject* pyobj);
	static PyObject* deepcopy(PyObject* pyobj);

	/** Initialize pickler */
	static bool initialize(void);
	static void finalise(void);

private:
	static PyObject* copyMethod_;							// cPicket.Dumps method pointer
	static PyObject* deepcopyMethod_;						// cPicket.The method method pointer
	static bool	isInit;										// Has it been initialized
} ;

}
}
#endif // OURO_SCRIPT_COPY_H
