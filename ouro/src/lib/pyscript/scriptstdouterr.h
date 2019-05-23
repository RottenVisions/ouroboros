// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_SCRIPTSTDOUTERR_H
#define OURO_SCRIPTSTDOUTERR_H

#include "helper/debug_helper.h"
#include "common/common.h"
#include "scriptobject.h"
#include "scriptstdout.h"
#include "scriptstderr.h"

namespace Ouroboros{ namespace script {

class ScriptStdOutErr
{					
public:	
	ScriptStdOutErr();
	virtual ~ScriptStdOutErr();

	/** 
		Install and uninstall this module
	*/
	bool install(void);
	bool uninstall(void);
	bool isInstall(void) const{ return isInstall_; }

	virtual void error_msg(const char* msg, uint32 msglen);
	virtual void info_msg(const char* msg, uint32 msglen);

	void pyPrint(const std::string& str);

	INLINE std::string& buffer();

protected:
	ScriptStdErr* pStderr_;
	ScriptStdOut* pStdout_;
	PyObject* pyPrint_;
	bool isInstall_;
	std::string sbuffer_;
};

}
}

#ifdef CODE_INLINE
#include "scriptstdouterr.inl"
#endif

#endif // OURO_SCRIPTSTDOUTERR_H
