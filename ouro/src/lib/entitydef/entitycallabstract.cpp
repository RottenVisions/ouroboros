// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com


#include "entitycallabstract.h"
#include "pyscript/pickler.h"
#include "helper/debug_helper.h"
#include "network/packet.h"
#include "network/bundle.h"
#include "network/network_interface.h"
#include "server/components.h"
#include "entitydef/entitydef.h"
#include "client_lib/client_interface.h"

#include "../../server/baseapp/baseapp_interface.h"
#include "../../server/cellapp/cellapp_interface.h"

#ifndef CODE_INLINE
#include "entitycallabstract.inl"
#endif

namespace Ouroboros{
EntityCallAbstract::EntityCallCallHookFunc*	EntityCallAbstract::__hookCallFuncPtr = NULL;
EntityCallAbstract::FindChannelFunc EntityCallAbstract::__findChannelFunc;

SCRIPT_METHOD_DECLARE_BEGIN(EntityCallAbstract)
SCRIPT_METHOD_DECLARE("__reduce_ex__",				reduce_ex__,			METH_VARARGS,		0)
SCRIPT_METHOD_DECLARE_END()

SCRIPT_MEMBER_DECLARE_BEGIN(EntityCallAbstract)
SCRIPT_MEMBER_DECLARE_END()

SCRIPT_GETSET_DECLARE_BEGIN(EntityCallAbstract)
SCRIPT_GET_DECLARE("id",							pyGetID,				0,					0)	
SCRIPT_GETSET_DECLARE_END()
SCRIPT_INIT(EntityCallAbstract, 0, 0, 0, 0, 0)		

//-------------------------------------------------------------------------------------
EntityCallAbstract::EntityCallAbstract(PyTypeObject* scriptType, 
											const Network::Address* pAddr, 
											COMPONENT_ID componentID, 
											ENTITY_ID eid, 
											uint16 utype, 
											ENTITYCALL_TYPE type):
ScriptObject(scriptType, false),
componentID_(componentID),
addr_((pAddr == NULL) ? Network::Address::NONE : *pAddr),
type_(type),
id_(eid),
utype_(utype)
{
}

//-------------------------------------------------------------------------------------
EntityCallAbstract::~EntityCallAbstract()
{
}

//-------------------------------------------------------------------------------------
void EntityCallAbstract::newCall(Network::Bundle& bundle)
{
	newCall_(bundle);
}

//-------------------------------------------------------------------------------------
void EntityCallAbstract::newCall_(Network::Bundle& bundle)
{
	// If it is the server-side entityCall
	if(g_componentType != CLIENT_TYPE && g_componentType != BOTS_TYPE)
	{
		// If the ID is 0, then this is a client component, otherwise it is the server.
		if(componentID_ == 0)
		{
			bundle.newMessage(ClientInterface::onRemoteMethodCall);
		}
		else
		{
			Components::ComponentInfos* cinfos = Components::getSingleton().findComponent(componentID_);

			if(cinfos != NULL)
			{
				// Find the corresponding component to deliver in the past. If the entityCall still needs to be transferred, such as e.base.cell, then transfer from baseapp to cellapp.
				if(cinfos->componentType == BASEAPP_TYPE)
				{
					bundle.newMessage(BaseappInterface::onEntityCall);
				}
				else
				{
					bundle.newMessage(CellappInterface::onEntityCall);
				}
			}
			else
			{
				ERROR_MSG(fmt::format("EntityCallAbstract::newCall_: not found component({}), entityID({})!\n",
					componentID_, id_));
			}
		}

		bundle << id_;
		
		// If it is a package sent to the client, there is no need to attach such a type.
		if(componentID_ > 0)
			bundle << type_;
	}
	else
	{
		// If the entityCall on the client calls the server method only exists to call cell or base
		switch(type_)
		{
		case ENTITYCALL_TYPE_BASE:
			bundle.newMessage(BaseappInterface::onRemoteMethodCall);
			break;
		case ENTITYCALL_TYPE_CELL:
			bundle.newMessage(BaseappInterface::onRemoteCallCellMethodFromClient);
			break;
		default:
			OURO_ASSERT(false && "no support!\n");
			break;
		};

		bundle << id_;
	}
}

//-------------------------------------------------------------------------------------
bool EntityCallAbstract::sendCall(Network::Bundle* pBundle)
{
	OURO_ASSERT(Components::getSingleton().pNetworkInterface() != NULL);
	Network::Channel* pChannel = getChannel();

	if(pChannel && !pChannel->isDestroyed())
	{
		pChannel->send(pBundle);
		return true;
	}
	else
	{
		ERROR_MSG(fmt::format("EntityCallAbstract::sendCall: invalid channel({}), entityID({})!\n",
			addr_.c_str(), id_));
	}

	Network::Bundle::reclaimPoolObject(pBundle);
	return false;
}

//-------------------------------------------------------------------------------------
PyObject* EntityCallAbstract::__py_reduce_ex__(PyObject* self, PyObject* protocol)
{
	EntityCallAbstract* eentitycall = static_cast<EntityCallAbstract*>(self);
	
	PyObject* args = PyTuple_New(2);
	PyObject* unpickleMethod = script::Pickler::getUnpickleFunc("EntityCall");
	PyTuple_SET_ITEM(args, 0, unpickleMethod);
	
	PyObject* args1 = PyTuple_New(4);
	PyTuple_SET_ITEM(args1, 0, PyLong_FromLong(eentitycall->id()));
	PyTuple_SET_ITEM(args1, 1, PyLong_FromUnsignedLongLong(eentitycall->componentID()));
	PyTuple_SET_ITEM(args1, 2, PyLong_FromUnsignedLong(eentitycall->utype()));

	int16 mbType = static_cast<int16>(eentitycall->type());
	
	PyTuple_SET_ITEM(args1, 3, PyLong_FromLong(mbType));
	PyTuple_SET_ITEM(args, 1, args1);

	if(unpickleMethod == NULL){
		Py_DECREF(args);
		return NULL;
	}
	return args;
}

//-------------------------------------------------------------------------------------
PyObject* EntityCallAbstract::pyGetID()
{ 
	return PyLong_FromLong(id()); 
}

//-------------------------------------------------------------------------------------
Network::Channel* EntityCallAbstract::getChannel(void)
{
	if (__findChannelFunc == NULL)
		return NULL;

	return __findChannelFunc(*this);
}

//-------------------------------------------------------------------------------------
ScriptDefModule* EntityCallAbstract::pScriptDefModule()
{
	return EntityDef::findScriptModule(utype_);
}

//-------------------------------------------------------------------------------------

}
