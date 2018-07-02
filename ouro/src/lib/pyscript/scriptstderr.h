// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_SCRIPTSTDERR_H
#define OURO_SCRIPTSTDERR_H

#include "helper/debug_helper.h"
#include "common/common.h"
#include "scriptobject.h"

namespace Ouroboros{ namespace script{

class ScriptStdOutErr;
class ScriptStdErr: public ScriptObject
{
	/** Subclassing fills some py operations into derived classes */
	INSTANCE_SCRIPT_HREADER(ScriptStdErr, ScriptObject)
public:
	ScriptStdErr(ScriptStdOutErr* pScriptStdOutErr);
	virtual ~ScriptStdErr();

	/**
		Python write operation 
	*/
	static PyObject* __py_write(PyObject* self, PyObject *args);
	static PyObject* __py_flush(PyObject* self, PyObject *args);

	bool install(void);
	bool uninstall(void);
	bool isInstall(void) const{ return isInstall_; }

	INLINE ScriptStdOutErr* pScriptStdOutErr() const;

protected:
	bool softspace_;
	PyObject* old_stdobj_;
	bool isInstall_;

	ScriptStdOutErr* pScriptStdOutErr_;
} ;

}
}

#ifdef CODE_INLINE
#include "scriptstderr.inl"
#endif

#endif // OURO_SCRIPTSTDERR_H
