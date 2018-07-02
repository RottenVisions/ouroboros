// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_PY_FILE_DESCRIPTOR_H
#define OURO_PY_FILE_DESCRIPTOR_H

#include "common/common.h"
#include "pyscript/scriptobject.h"
#include "common/smartpointer.h"

namespace Ouroboros{
typedef SmartPointer<PyObject> PyObjectPtr;

class PyFileDescriptor : public Network::InputNotificationHandler, public Network::OutputNotificationHandler
{
public:
	PyFileDescriptor(int fd, PyObject* pyCallback, bool write);
	virtual ~PyFileDescriptor();

	/**
		Descriptor
	*/
	static PyObject* __py_registerReadFileDescriptor(PyObject* self, PyObject* args);
	static PyObject* __py_registerWriteFileDescriptor(PyObject* self, PyObject* args);
	static PyObject* __py_deregisterReadFileDescriptor(PyObject* self, PyObject* args);
	static PyObject* __py_deregisterWriteFileDescriptor(PyObject* self, PyObject* args);
protected:

	virtual int handleInputNotification( int fd );
	virtual int handleOutputNotification( int fd );

	void callback();

	int fd_;
	PyObjectPtr pyCallback_;

	bool write_;
};

}

#endif // OURO_PY_FILE_DESCRIPTOR_H
