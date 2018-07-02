// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com


namespace Ouroboros {
namespace script{

INLINE int Sequence::length(void) const
{ 
	return (int)values_.size();
}

INLINE std::vector<PyObject*>& Sequence::getValues(void)
{ 
	return values_; 
}
	
}
}

