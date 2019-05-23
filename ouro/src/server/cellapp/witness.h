// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_WITNESS_H
#define OURO_WITNESS_H

// common include
#include "updatable.h"
#include "entityref.h"
#include "helper/debug_helper.h"
#include "common/common.h"
#include "common/objectpool.h"
#include "math/math.h"

// #define NDEBUG
// windows include	
#if OURO_PLATFORM == PLATFORM_WIN32	
#else
// linux include
#endif

namespace Ouroboros{

namespace Network
{
	class Bundle;
	class MessageHandler;
}

class Entity;
class MemoryStream;
class ViewTrigger;
class SpaceMemory;

/** Observer information structure*/
struct WitnessInfo
{
	WitnessInfo(const int8& lv, Entity* e, const float& r):
	detailLevel(lv),
	entity(e),
	range(r)
	{
		for(int i=0; i<3; ++i)
			if(lv == i)
				detailLevelLog[i] = true;
			else
				detailLevelLog[i] = false;
	}
	
	int8 detailLevel; // current level of detail
	Entity* entity; // the entity expressed
	float range; // current distance from this entity
	bool detailLevelLog[3]; // Indicates which level of detail the entity has entered the entity, providing attribute broadcast optimization.
												// When you have not entered a certain level, all the attributes of this level will be updated to him, otherwise only the attributes that have changed in the recent time will be updated.
	std::vector<uint32> changeDefDataLogs[3]; //After the entity leaves a level of detail (without leaving the witness), there is a level of detail change attribute recorded here.
};

/**
	This module is used to monitor the entity data we are interested in, such as: view, property update, method of calling entity
	And transmit it to the watcher.
*/
class Witness : public PoolObject, public Updatable
{
public:
	typedef std::list<EntityRef*> VIEW_ENTITIES;
	typedef std::map<ENTITY_ID, EntityRef*> VIEW_ENTITIES_MAP;

	Witness();
	~Witness();
	
	virtual uint8 updatePriority() const {
		return 1;
	}

	void addToStream(Ouroboros::MemoryStream& s);
	void createFromStream(Ouroboros::MemoryStream& s);

	typedef OUROShared_ptr< SmartPoolObject< Witness > > SmartPoolObjectPtr;
	static SmartPoolObjectPtr createSmartPoolObj(const std::string& logPoint);

	static ObjectPool<Witness>& ObjPool();
	static Witness* createPoolObject(const std::string& logPoint);
	static void reclaimPoolObject(Witness* obj);
	static void destroyObjPool();
	void onReclaimObject();

	virtual size_t getPoolObjectBytes()
	{
		size_t bytes = sizeof(pEntity_)
		 + sizeof(viewRadius_) + sizeof(viewHysteresisArea_)
		 + sizeof(pViewTrigger_) + sizeof(pViewHysteresisAreaTrigger_) + sizeof(clientViewSize_)
		 + sizeof(lastBasePos_) + (sizeof(EntityRef*) * viewEntities_map_.size());

		return bytes;
	}

	INLINE void pEntity(Entity* pEntity);
	INLINE Entity* pEntity();

	void attach(Entity* pEntity);
	void detach(Entity* pEntity);
	void clear(Entity* pEntity);
	void onAttach(Entity* pEntity);

	void setViewRadius(float radius, float hyst = 5.0f);
	
	INLINE float viewRadius() const;
	INLINE float viewHysteresisArea() const;

	typedef std::vector<Network::Bundle*> Bundles;
	bool pushBundle(Network::Bundle* pBundle);

	/**
		Basic position, if there is a mount base position, it may be a mount, etc.
	*/
	INLINE const Position3D& basePos();

	/**
	Foundation orientation, if there is a mount base orientation, it may be a mount, etc.
	*/
	INLINE const Direction3D& baseDir();

	bool update();
	
	void onEnterSpace(SpaceMemory* pSpace);
	void onLeaveSpace(SpaceMemory* pSpace);

	void onEnterView(ViewTrigger* pViewTrigger, Entity* pEntity);
	void onLeaveView(ViewTrigger* pViewTrigger, Entity* pEntity);
	void _onLeaveView(EntityRef* pEntityRef);

	/**
		Get the token of the entity's current synchronized Volatile data
	*/
	uint32 getEntityVolatileDataUpdateFlags(Entity* otherEntity);
	

	const Network::MessageHandler& getViewEntityMessageHandler(const Network::MessageHandler& normalMsgHandler, 
											   const Network::MessageHandler& optimizedMsgHandler, ENTITY_ID entityID, int& ialiasID);

	bool entityID2AliasID(ENTITY_ID id, uint8& aliasID);

	/**
		Which protocol is used to update the client
	*/
	void addUpdateToStream(Network::Bundle* pForwardBundle, uint32 flags, EntityRef* pEntityRef);

	/**
		Add the base location to the update package
	*/
	void addBaseDataToStream(Network::Bundle* pSendBundle);

	/**
		Push a message to the witness client
	*/
	bool sendToClient(const Network::MessageHandler& msgHandler, Network::Bundle* pBundle);
	Network::Channel* pChannel();
		
	INLINE VIEW_ENTITIES_MAP& viewEntitiesMap();
	INLINE VIEW_ENTITIES& viewEntities();

		/** Get a reference to viewentity*/
	INLINE EntityRef* getViewEntityRef(ENTITY_ID entityID);

		/** Whether the entityID is in the view*/
	INLINE bool entityInView(ENTITY_ID entityID);

	INLINE ViewTrigger* pViewTrigger();
	INLINE ViewTrigger* pViewHysteresisAreaTrigger();
	
	void installViewTrigger();
	void uninstallViewTrigger();

	/**
		Reset the entities in the View range to restore their sync state to the original unsynchronized state
	*/
	void resetViewEntities();

private:
	/**
		if the number of entities in the view is less than 256, only the index position is sent.
	*/
	INLINE void _addViewEntityIDToBundle(Network::Bundle* pBundle, EntityRef* pEntityRef);
	
	/**
		Need to update the aliasID of the entityRef when the view list has changed when the update is executed.
	*/
	void updateEntitiesAliasID();
		
private:
	Entity*									pEntity_;

	// The view radius of the current entity
	float									viewRadius_;
	// A lag range of the current entityView
	float									viewHysteresisArea_;

	ViewTrigger*							pViewTrigger_;
	ViewTrigger*							pViewHysteresisAreaTrigger_;

	VIEW_ENTITIES							viewEntities_;
	VIEW_ENTITIES_MAP						viewEntities_map_;

	Position3D								lastBasePos_;
	Direction3D								lastBaseDir_;

	uint16									clientViewSize_;
};

}

#ifdef CODE_INLINE
#include "witness.inl"
#endif
#endif
