// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com


#include "entitydef/entity_component.h"
#include "entity.h"
#include "real_entity_method.h"

namespace Ouroboros{

SCRIPT_METHOD_DECLARE_BEGIN(EntityComponent)
//SCRIPT_METHOD_DECLARE("__reduce_ex__",				reduce_ex__,			METH_VARARGS,		0)
SCRIPT_METHOD_DECLARE("addTimer",						pyAddTimer,				METH_VARARGS,		0)
SCRIPT_METHOD_DECLARE("delTimer",						pyDelTimer,				METH_VARARGS,		0)
SCRIPT_METHOD_DECLARE("clientEntity",					pyClientEntity,			METH_VARARGS,		0)
SCRIPT_METHOD_DECLARE_END()

SCRIPT_GETSET_DECLARE_BEGIN(EntityComponent)
SCRIPT_GET_DECLARE("ownerID",						pyGetOwnerID,			0,					0)
SCRIPT_GET_DECLARE("owner",							pyGetOwner,				0,					0)
SCRIPT_GET_DECLARE("name",							pyName,					0,					0)
SCRIPT_GET_DECLARE("isDestroyed",					pyIsDestroyed,			0,					0)
SCRIPT_GET_DECLARE("base",							pyGetBaseEntityCall,	0,					0)
SCRIPT_GET_DECLARE("client",						pyGetClientEntityCall,	0,					0)
SCRIPT_GET_DECLARE("allClients",					pyGetAllClients,		0,					0)
SCRIPT_GET_DECLARE("otherClients",					pyGetOtherClients,		0,					0)
SCRIPT_GETSET_DECLARE_END()
BASE_SCRIPT_INIT(EntityComponent, 0, 0, 0, 0, 0)

//-------------------------------------------------------------------------------------
PyObject* EntityComponent::onScriptGetAttribute(PyObject* attr)
{
	const char* ccattr = PyUnicode_AsUTF8AndSize(attr, NULL);

	if (ownerID_ > 0)
	{
		Entity* pOwner = static_cast<Entity*>(owner());

		if (pOwner)
		{
			// If the ghost call def method then rpc call.
			if (!pOwner->isReal())
			{
				MethodDescription* pMethodDescription = const_cast<ScriptDefModule*>(pComponentDescrs_)->findCellMethodDescription(ccattr);

				if (pMethodDescription)
				{
					return new RealEntityMethod(pPropertyDescription_, pMethodDescription, pOwner);
				}
			}
			else
			{
			}
		}
	}

	return ScriptObject::onScriptGetAttribute(attr);
}

}
