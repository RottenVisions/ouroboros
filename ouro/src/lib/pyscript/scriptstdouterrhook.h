// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_SCRIPTSTDOUTERRHOOK_H
#define OURO_SCRIPTSTDOUTERRHOOK_H

#include "common/common.h"
#include "scriptobject.h"
#include "scriptstdouterr.h"

namespace Ouroboros{ namespace script{

class ScriptStdOutErrHook : public ScriptStdOutErr
{
public:
	ScriptStdOutErrHook();
	~ScriptStdOutErrHook();

	virtual void error_msg(const wchar_t* msg, uint32 msglen);
	virtual void info_msg(const wchar_t* msg, uint32 msglen);

	INLINE void setHookBuffer(std::string* buffer);

	INLINE void setPrint(bool v);

protected:
	std::string* buffer_;
	std::wstring wbuffer_;
	bool isPrint_;
} ;

}
}

#ifdef CODE_INLINE
#include "scriptstdouterrhook.inl"
#endif

#endif // OURO_SCRIPTSTDOUTERRHOOK_H
