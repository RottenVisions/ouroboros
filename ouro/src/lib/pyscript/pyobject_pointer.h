// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_PYOBJECT_POINTER_H
#define OURO_PYOBJECT_POINTER_H

namespace Ouroboros { 

typedef SmartPointer<PyObject> PyObjectPtr;

template<>
inline void incrementReferenceCount(const PyObject& obj)
{
	Py_INCREF( const_cast<PyObject*>( &obj ) );
};

template<>
inline void decrementReferenceCount(const PyObject& obj)
{
	Py_DECREF( const_cast<PyObject*>( &obj ) );
};

}
#endif // OURO_PYOBJECT_POINTER_H
