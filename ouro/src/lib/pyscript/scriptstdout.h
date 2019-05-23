// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_SCRIPTSTDOUT_H
#define OURO_SCRIPTSTDOUT_H

#include "helper/debug_helper.h"
#include "common/common.h"
#include "scriptobject.h"

namespace Ouroboros{ namespace script{

class ScriptStdOutErr;
class ScriptStdOut: public ScriptObject
{
		/** Subclassing populates some py operations into derived classes*/
	INSTANCE_SCRIPT_HREADER(ScriptStdOut, ScriptObject)
public:	
	ScriptStdOut(ScriptStdOutErr* pScriptStdOutErr);
	virtual ~ScriptStdOut();

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
#include "scriptstdout.inl"
#endif

#endif // OURO_SCRIPTSTDOUT_H
