// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com


namespace Ouroboros {
namespace script{

INLINE void ScriptStdOutErrHook::setHookBuffer(std::string* buffer){ 
	pBuffer_ = buffer; 
	buffer_ = ""; 
};

INLINE void ScriptStdOutErrHook::setPrint(bool v)
{
	isPrint_ = v;
}

}
}

