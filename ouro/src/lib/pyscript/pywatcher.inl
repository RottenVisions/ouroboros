// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com


namespace Ouroboros {
namespace script{

template <class T>
INLINE T PyWatcherObject<T>::getValue()
{
	T v;

	PyObject* pyObj1 = PyObject_CallFunction(pyCallable_, const_cast<char*>(""));
	if(!pyObj1)
	{
		PyErr_Format(PyExc_Exception, "PyWatcherObject::addToStream: callFunction error! path=%s name=%s.\n",
			path(), name());
		PyErr_PrintEx(0);
	}
	else
	{
		readVal(pyObj1, v);
		SCRIPT_ERROR_CHECK();
		Py_DECREF(pyObj1);
	}

	return v;
}
	
}
}

