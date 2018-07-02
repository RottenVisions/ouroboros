// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com


namespace Ouroboros {
namespace script{

INLINE MemoryStream& PyMemoryStream::stream()
{ 
	return stream_;
}

INLINE PyObject* PyMemoryStream::pyBytes()
{
	if(stream_.size() == 0)
	{
		return PyBytes_FromString("");
	}
	
	return PyBytes_FromStringAndSize((char*)stream_.data(), stream_.size());
}

INLINE int PyMemoryStream::length(void) const
{ 
	return (int)stream_.size();
}

INLINE bool PyMemoryStream::readonly() const
{
	return readonly_; 
}

}
}

