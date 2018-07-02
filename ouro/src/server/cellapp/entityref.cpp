// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com
#include "entityref.h"
#include "entity.h"
#include "cellapp.h"
#include "common/memorystream.h"

namespace Ouroboros{	

//-------------------------------------------------------------------------------------
EntityRef::EntityRef(Entity* pEntity):
id_(0),
aliasID_(0),
pEntity_(pEntity),
flags_(ENTITYREF_FLAG_UNKONWN)
{
	id_ = pEntity->id();
}

//-------------------------------------------------------------------------------------
EntityRef::EntityRef():
id_(0),
aliasID_(0),
pEntity_(NULL),
flags_(ENTITYREF_FLAG_UNKONWN)
{
}

//-------------------------------------------------------------------------------------
EntityRef::~EntityRef()
{
}

//-------------------------------------------------------------------------------------
static ObjectPool<EntityRef> _g_objPool("EntityRef");
ObjectPool<EntityRef>& EntityRef::ObjPool()
{
	return _g_objPool;
}

//-------------------------------------------------------------------------------------
EntityRef* EntityRef::createPoolObject()
{
	return _g_objPool.createObject();
}

//-------------------------------------------------------------------------------------
void EntityRef::reclaimPoolObject(EntityRef* obj)
{
	_g_objPool.reclaimObject(obj);
}

//-------------------------------------------------------------------------------------
void EntityRef::destroyObjPool()
{
	DEBUG_MSG(fmt::format("EntityRef::destroyObjPool(): size {}.\n",
		_g_objPool.size()));

	_g_objPool.destroy();
}

//-------------------------------------------------------------------------------------
EntityRef::SmartPoolObjectPtr EntityRef::createSmartPoolObj()
{
	return SmartPoolObjectPtr(new SmartPoolObject<EntityRef>(ObjPool().createObject(), _g_objPool));
}

//-------------------------------------------------------------------------------------
void EntityRef::onReclaimObject()
{
	id_ = 0;
	aliasID_ =  0;
	pEntity_ = NULL;
	flags_ = ENTITYREF_FLAG_UNKONWN;
}

//-------------------------------------------------------------------------------------
void EntityRef::pEntity(Entity* e)
{
	pEntity_ = e; 

	if(e)
		id_ = e->id(); 
}

//-------------------------------------------------------------------------------------
void EntityRef::addToStream(Ouroboros::MemoryStream& s)
{
	ENTITY_ID eid = 0;
	if(pEntity_)
		eid = pEntity_->id();

	s << id_ << aliasID_ << flags_ << eid;
}

//-------------------------------------------------------------------------------------
void EntityRef::createFromStream(Ouroboros::MemoryStream& s)
{
	ENTITY_ID eid = 0;
	s >> id_ >> aliasID_ >> flags_ >> eid;

	if(eid > 0)
	{
		pEntity_ = Cellapp::getSingleton().findEntity(eid);
	}
}

//-------------------------------------------------------------------------------------
}
