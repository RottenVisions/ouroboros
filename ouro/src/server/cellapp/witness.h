// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

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

/** Observer Information Structure */
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

	int8 detailLevel;							// Current level of detail
	Entity* entity;								// Expressed entity
	float range;								// Current distance from this entity
	bool detailLevelLog[3];						// Indicates which level of detail this entity has entered into the entity, providing attribute broadcast optimization
												// When you have not entered a certain level, it will update all the attributes of this level to him, otherwise it will only update the attributes that have been changed in the recent period of time.
	std::vector<uint32> changeDefDataLogs[3];	// After the entity leaves a certain level of detail (without leaving the witness), a certain level of detail changes during this period are recorded here.
};

/**
	This module is used to monitor the entity data we are interested in, such as: view, property update, call the entity method
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
	static SmartPoolObjectPtr createSmartPoolObj();

	static ObjectPool<Witness>& ObjPool();
	static Witness* createPoolObject();
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
		Base position, if there is a mount base position may be mount etc.
	*/
	INLINE const Position3D& basePos();

	/**
	The basic orientation, if there is a mount base orientation may be a mount, etc.
	*/
	INLINE const Direction3D& baseDir();

	bool update();

	void onEnterSpace(SpaceMemory* pSpace);
	void onLeaveSpace(SpaceMemory* pSpace);

	void onEnterView(ViewTrigger* pViewTrigger, Entity* pEntity);
	void onLeaveView(ViewTrigger* pViewTrigger, Entity* pEntity);
	void _onLeaveView(EntityRef* pEntityRef);

	/**
		Get tags for this time syncing Volatile data for the entity
	*/
	uint32 getEntityVolatileDataUpdateFlags(Entity* otherEntity);


	const Network::MessageHandler& getViewEntityMessageHandler(const Network::MessageHandler& normalMsgHandler,
											   const Network::MessageHandler& optimizedMsgHandler, ENTITY_ID entityID, int& ialiasID);

	bool entityID2AliasID(ENTITY_ID id, uint8& aliasID);

	/**
		What protocol to use to update the client
	*/
	void addUpdateToStream(Network::Bundle* pForwardBundle, uint32 flags, EntityRef* pEntityRef);

	/**
		Add base location to update package
	*/
	void addBaseDataToStream(Network::Bundle* pSendBundle);

	/**
		Push a message to the witness client
	*/
	bool sendToClient(const Network::MessageHandler& msgHandler, Network::Bundle* pBundle);
	Network::Channel* pChannel();

	INLINE VIEW_ENTITIES_MAP& viewEntitiesMap();
	INLINE VIEW_ENTITIES& viewEntities();

	/** Get a reference to viewentity */
	INLINE EntityRef* getViewEntityRef(ENTITY_ID entityID);

	/** Whether the entityID is in the view */
	INLINE bool entityInView(ENTITY_ID entityID);

	INLINE ViewTrigger* pViewTrigger();
	INLINE ViewTrigger* pViewHysteresisAreaTrigger();

	void installViewTrigger();
	void uninstallViewTrigger();

	/**
		Reset the View-scoped entities to bring them back to their original unsynchronized state
	*/
	void resetViewEntities();

private:
	/**
		If the number of entities in the view is less than 256, only the index position is sent
	*/
	INLINE void _addViewEntityIDToBundle(Network::Bundle* pBundle, EntityRef* pEntityRef);

	/**
		The update of the entityRef's aliasID is required when the view is changed when the update is executed
	*/
	void updateEntitiesAliasID();

private:
	Entity*									pEntity_;

	// The current entity's view radius
	float									viewRadius_;
	// A lag range for the current entityView
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
