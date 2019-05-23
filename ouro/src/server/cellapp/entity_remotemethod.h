// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_ENTITY_REMOTE_METHOD_H
#define OURO_ENTITY_REMOTE_METHOD_H


#include "helper/debug_helper.h"
#include "common/common.h"	
#include "entitydef/remote_entity_method.h"

namespace Ouroboros{

class EntityRemoteMethod : public RemoteEntityMethod
{
		/** Subclassing populates some py operations into derived classes*/
	INSTANCE_SCRIPT_HREADER(EntityRemoteMethod, RemoteEntityMethod)	
public:
	EntityRemoteMethod(MethodDescription* methodDescription, 
						EntityCallAbstract* entityCall);

	~EntityRemoteMethod();

	static PyObject* tp_call(PyObject* self, 
			PyObject* args, PyObject* kwds);

private:

};

}
#endif
