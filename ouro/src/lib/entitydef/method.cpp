// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#include "method.h"
#include "entitydef.h"
#include "network/bundle.h"

#ifndef CODE_INLINE
#include "method.inl"
#endif

namespace Ouroboros{

uint32	MethodDescription::methodDescriptionCount_ = 0;

//-------------------------------------------------------------------------------------
MethodDescription::MethodDescription(ENTITY_METHOD_UID utype, COMPONENT_ID domain,
									 std::string name,
									 bool isExposed):
methodDomain_(domain),
name_(name),
utype_(utype),
argTypes_(),
isExposed_(isExposed),
aliasID_(-1)
{
	MethodDescription::methodDescriptionCount_++;

	EntityDef::md5().append((void*)name_.c_str(), (int)name_.size());
	EntityDef::md5().append((void*)&utype_, sizeof(ENTITY_METHOD_UID));
	EntityDef::md5().append((void*)&isExposed_, sizeof(bool));
}

//-------------------------------------------------------------------------------------
MethodDescription::~MethodDescription()
{
	std::vector<DataType*>::iterator iter = argTypes_.begin();
	for(; iter != argTypes_.end(); ++iter)
		(*iter)->decRef();

	argTypes_.clear();
}

//-------------------------------------------------------------------------------------
void MethodDescription::setExposed(void)
{
	isExposed_ = true;
	EntityDef::md5().append((void*)&isExposed_, sizeof(bool));
}

//-------------------------------------------------------------------------------------
bool MethodDescription::pushArgType(DataType* dataType)
{
	if(dataType == NULL)
	{
		ERROR_MSG("MethodDescription::pushArgType: dataType is NULL!\n");
		return false;
	}

	dataType->incRef();
	argTypes_.push_back(dataType);

	DATATYPE_UID uid = dataType->id();
	EntityDef::md5().append((void*)&uid, sizeof(DATATYPE_UID));
	EntityDef::md5().append((void*)&isExposed_, sizeof(bool));
	return true;
}

//-------------------------------------------------------------------------------------
bool MethodDescription::checkArgs(PyObject* args)
{
	if (args == NULL || !PyTuple_Check(args))
	{
		PyErr_Format(PyExc_AssertionError, "Method::checkArgs: method[%s] args is not a tuple.\n",
			getName());

		PyErr_PrintEx(0);
		return false;
	}

	int offset = (isExposed() == true && g_componentType == CELLAPP_TYPE && isCell()) ? 1 : 0;
	uint8 argsSize = (uint8)argTypes_.size();
	uint8 giveArgsSize = (uint8)PyTuple_Size(args);

	if (giveArgsSize != argsSize + offset)
	{
		PyErr_Format(PyExc_AssertionError, "Method::checkArgs: method[%s] requires exactly %d argument%s%s; %d given",
				getName(),
				argsSize,
				(offset > 0) ? " + exposed(1)" : "",
				(argsSize == 1) ? "" : "s",
				PyTuple_Size(args));

		PyErr_PrintEx(0);
		return false;
	}


	// Check if it is an exposed method
	if(offset > 0)
	{
		PyObject* pyExposed = PyTuple_GetItem(args, 0);
		if (!PyLong_Check(pyExposed))
		{
			PyObject* pyeid = PyObject_GetAttrString(pyExposed, "id");
			if (pyeid == NULL || !PyLong_Check(pyeid))
			{
				Py_XDECREF(pyeid);
				PyErr_Format( PyExc_TypeError,
					"Method::checkArgs: method[%s] requires None, an id, or an object with an "
					"id as its first agument", getName());

				PyErr_PrintEx(0);
				return false;
			}

			Py_DECREF(pyeid);
		}
	}

	for(uint8 i=0; i <argsSize; ++i)
	{
		PyObject* pyArg = PyTuple_GetItem(args, i + offset);
		if (!argTypes_[i]->isSameType(pyArg))
		{
			PyObject* pExample = argTypes_[i]->parseDefaultStr("");
			PyErr_Format(PyExc_AssertionError,
				"Method::checkArgs: method[%s] argument %d: Expected %s, %s found",
				getName(),
				i+1,
				pExample->ob_type->tp_name,
				pyArg != NULL ? pyArg->ob_type->tp_name : "NULL");

			PyErr_PrintEx(0);
			Py_DECREF(pExample);
			return false;
		}
	}

	return true;
}

//-------------------------------------------------------------------------------------
void MethodDescription::addToStream(MemoryStream* mstream, PyObject* args)
{
	uint8 argsSize = argTypes_.size();
	int offset = 0;

	// Put utype in to facilitate peer identification of this method
	// Here if aliasID_ is greater than 0 then an optimized method is used, using 1 byte transfer
	// Note: The client method is set when the def is loaded, the aliasID is set, so the aliasID is not used inside the server.
	if(aliasID_ <= 0)
	{
		(*mstream) << utype_;
	}
	else
	{
		uint8 utype = (uint8)aliasID_;
		(*mstream) << utype;
	}

	// If the exposed method is first packaged into the entityID
	if(isExposed() && g_componentType == CELLAPP_TYPE && isCell())
	{
		offset = 1;
	}

	// Add each parameter to the stream
	for(uint8 i=0; i <argsSize; ++i)
	{
		PyObject* pyArg = PyTuple_GetItem(args, i + offset);
		argTypes_[i]->addToStream(mstream, pyArg);
	}
}

//-------------------------------------------------------------------------------------
PyObject* MethodDescription::createFromStream(MemoryStream* mstream)
{
	size_t argSize = getArgSize();
	PyObject* pyArgsTuple = NULL;
	int offset = 0;

	if(isExposed() && g_componentType == CELLAPP_TYPE && isCell())
	{
		offset = 1;
		pyArgsTuple = PyTuple_New(argSize + offset);

		// Set a caller ID to the script to determine if the source is correct
		OURO_ASSERT(EntityDef::context().currEntityID > 0);
		PyTuple_SET_ITEM(pyArgsTuple, 0, PyLong_FromLong(EntityDef::context().currEntityID));
	}
	else
		pyArgsTuple = PyTuple_New(argSize);

	for(size_t index=0; index<argSize; ++index)
	{
		PyObject* pyitem = argTypes_[index]->createFromStream(mstream);

		if(pyitem == NULL)
		{
			WARNING_MSG(fmt::format("MethodDescription::createFromStream: {} arg[{}][{}] is NULL.\n",
				this->getName(), index, argTypes_[index]->getName()));
		}

		PyTuple_SET_ITEM(pyArgsTuple, index + offset, pyitem);
	}

	return pyArgsTuple;
}

//-------------------------------------------------------------------------------------
size_t MethodDescription::getArgSize(void)
{
	return argTypes_.size();
}

//-------------------------------------------------------------------------------------
PyObject* MethodDescription::call(PyObject* func, PyObject* args)
{
	PyObject* pyResult = NULL;
	if (!PyCallable_Check(func))
	{
		PyErr_Format(PyExc_TypeError, "MethodDescription::call: method[%s] call attempted on a error object!",
			getName());
	}
	else
	{
		if(args == NULL)
		{
			pyResult = PyObject_CallObject(func, NULL);
		}
		else
		{
			if(checkArgs(args))
				pyResult = PyObject_CallObject(func, args);
		}
	}

	if (PyErr_Occurred())
	{
		if (isExposed() && PyErr_ExceptionMatches(PyExc_TypeError))
		{
			WARNING_MSG(fmt::format("MethodDescription::call: {} is exposed of method, if there is a missing arguments error, "
				"try adding callerEntityID, For example: \ndef func(msg): => def func(callerEntityID, msg):\n",
				this->getName()));
		}

		PyErr_PrintEx(0);
	}

	return pyResult;
}

//-------------------------------------------------------------------------------------

}
