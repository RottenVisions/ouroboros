// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_ENTITY_REF_H
#define OURO_ENTITY_REF_H

#include "helper/debug_helper.h"
#include "common/common.h"	
#include "common/objectpool.h"

namespace Ouroboros{

class Entity;
class MemoryStream;

#define ENTITYREF_FLAG_UNKONWN							0x00000000
#define ENTITYREF_FLAG_ENTER_CLIENT_PENDING 0x00000001 // Enter the logo in the client
#define ENTITYREF_FLAG_LEAVE_CLIENT_PENDING 0x00000002 // Leave the logo in the client
#define ENTITYREF_FLAG_NORMAL 0x00000004 // Normal state

class EntityRef : public PoolObject
{
public:
	EntityRef(Entity* pEntity);
	EntityRef();

	~EntityRef();
	
	typedef KBEShared_ptr< SmartPoolObject< EntityRef > > SmartPoolObjectPtr;
	static SmartPoolObjectPtr createSmartPoolObj(const std::string& logPoint);

	static ObjectPool<EntityRef>& ObjPool();
	static EntityRef* createPoolObject(const std::string& logPoint);
	static void reclaimPoolObject(EntityRef* obj);
	static void destroyObjPool();
	void onReclaimObject();

	virtual size_t getPoolObjectBytes()
	{
		size_t bytes = sizeof(id_)
			+ sizeof(aliasID_) + sizeof(pEntity_)
			+ sizeof(flags_);

		return bytes;
	}

	void flags(uint32 v) { flags_ = v; }
	void removeflags(uint32 v) { flags_ &= ~v; }
	uint32 flags() { return flags_; }
	
	Entity* pEntity() const { return pEntity_; }
	void pEntity(Entity* e);

	ENTITY_ID id() const { return id_; }

	int aliasID() const { return aliasID_; }
	void aliasID(int id) { aliasID_ = id; }

	void addToStream(Ouroboros::MemoryStream& s);
	void createFromStream(Ouroboros::MemoryStream& s);

private:
	ENTITY_ID id_;
	int aliasID_;
	Entity* pEntity_;
	uint32 flags_;
};

}

#endif
