// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com


#include "scriptstdouterrhook.h"

#ifndef CODE_INLINE
#include "scriptstdouterrhook.inl"
#endif

namespace Ouroboros{ namespace script{
								
//-------------------------------------------------------------------------------------
ScriptStdOutErrHook::ScriptStdOutErrHook():
isPrint_(true)
{
}

//-------------------------------------------------------------------------------------
ScriptStdOutErrHook::~ScriptStdOutErrHook()
{
}

//-------------------------------------------------------------------------------------
void ScriptStdOutErrHook::info_msg(const char* msg, uint32 msglen)
{
	if(isPrint_)
		ScriptStdOutErr::info_msg(msg, msglen);

	buffer_ += msg;

	if(msg[0] == '\n')
	{
		if(pBuffer_)
		{
			(*pBuffer_) += buffer_;
			buffer_ = "";
		}
	}
}

//-------------------------------------------------------------------------------------
void ScriptStdOutErrHook::error_msg(const char* msg, uint32 msglen)
{
	if(isPrint_)
		ScriptStdOutErr::error_msg(msg, msglen);

	buffer_ += msg;

	if(msg[0] == '\n')
	{
		if(pBuffer_)
		{
			(*pBuffer_) += buffer_;
			buffer_ = "";
		}
	}
}

//-------------------------------------------------------------------------------------

}
}
