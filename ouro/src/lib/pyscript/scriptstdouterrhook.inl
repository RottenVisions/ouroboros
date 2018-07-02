// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com


namespace Ouroboros {
namespace script{

INLINE void ScriptStdOutErrHook::setHookBuffer(std::string* buffer){ 
	buffer_ = buffer; 
	wbuffer_ = L""; 
};

INLINE void ScriptStdOutErrHook::setPrint(bool v)
{
	isPrint_ = v;
}

}
}

