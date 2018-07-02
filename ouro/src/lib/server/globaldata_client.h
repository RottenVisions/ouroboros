// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com
#ifndef OURO_GLOBAL_DATA_CLIENT_H
#define OURO_GLOBAL_DATA_CLIENT_H

#include "globaldata_server.h"
#include "common/common.h"
#include "helper/debug_helper.h"
#include "pyscript/map.h"

namespace Ouroboros{

class GlobalDataClient : public script::Map
{
	/** Subclassing fills some py operations into derived classes */
	INSTANCE_SCRIPT_HREADER(GlobalDataClient, script::Map)

public:
	GlobalDataClient(COMPONENT_TYPE componentType, GlobalDataServer::DATA_TYPE dataType);
	~GlobalDataClient();

	/** Write data */
	bool write(PyObject* pyKey, PyObject* pyValue);

	/** delete data */
	bool del(PyObject* pyKey);

	/** Data change notification */
	void onDataChanged(PyObject* key, PyObject* value, bool isDelete = false);

	/** Set the server component type of this global data client */
	void setServerComponentType(COMPONENT_TYPE ct){ serverComponentType_ = ct; }

private:
	COMPONENT_TYPE					serverComponentType_;				// The type of server component where GlobalDataServer is located
	GlobalDataServer::DATA_TYPE 	dataType_;
} ;

}

#endif // OURO_GLOBAL_DATA_CLIENT_H
