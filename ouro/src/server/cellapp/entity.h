// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_ENTITY_H
#define OURO_ENTITY_H
	
#include "profile.h"
#include "common/timer.h"
#include "common/common.h"
#include "common/smartpointer.h"
#include "helper/debug_helper.h"
#include "entitydef/entity_call.h"
#include "entitydef/entity_component.h"
#include "pyscript/math.h"
#include "pyscript/scriptobject.h"
#include "entitydef/datatypes.h"	
#include "entitydef/entitydef.h"	
#include "entitydef/scriptdef_module.h"
#include "entitydef/entity_macro.h"	
#include "server/script_timers.h"	
	
namespace Ouroboros{

class Chunk;
class Entity;
class EntityCall;
class EntityComponent;
class Cellapp;
class Witness;
class AllClients;
class CoordinateSystem;
class EntityCoordinateNode;
class Controller;
class Controllers;
class SpaceMemory;
class VolatileInfo;

namespace Network
{
class Channel;
class Bundle;
}

typedef SmartPointer<Entity> EntityPtr;
typedef std::vector<EntityPtr> SPACE_ENTITIES;

class Entity : public script::ScriptObject
{
		/** Subclassing populates some py operations into derived classes*/
	BASE_SCRIPT_HREADER(Entity, ScriptObject)	
	ENTITY_HEADER(Entity)

public:
	Entity(ENTITY_ID id, const ScriptDefModule* pScriptModule,
		PyTypeObject* pyType = getScriptType(), bool isInitialised = true);
	~Entity();
	
	/** 
		Destroy this entity
	*/
	void onDestroy(bool callScript);
	
	/**
		Destroy the scene
	*/
	DECLARE_PY_MOTHOD_ARG0(pyDestroySpace);
	void destroySpace(void);

	/** 
		Triggered when the space where the current entity is located is about to be destroyed
	*/
	void onSpaceGone();
	
	/** 
		Determine if it is a realEntity
	*/
	INLINE bool isReal(void) const;

	/** 
		Determine if you have ghostEntity
	*/
	INLINE bool hasGhost(void) const;

	/** 
		Determine if it is a realEntity
	*/
	INLINE COMPONENT_ID realCell(void) const;
	INLINE void realCell(COMPONENT_ID cellID);

	/** 
		Determine if you have ghostEntity
	*/
	INLINE COMPONENT_ID ghostCell(void) const;
	INLINE void ghostCell(COMPONENT_ID cellID);

	/** 
		Defining attribute data has been changed
	*/
	void onDefDataChanged(EntityComponent* pEntityComponent, const PropertyDescription* propertyDescription,
			PyObject* pyData);
	
	/** 
		The entity communication channel
	*/
	INLINE void pChannel(Network::Channel* pchannel);
	INLINE Network::Channel* pChannel(void) const ;

public:
	/** 
		entityCall section
	*/
	INLINE EntityCall* baseEntityCall() const;
	DECLARE_PY_GET_MOTHOD(pyGetBaseEntityCall);
	INLINE void baseEntityCall(EntityCall* entityCall);
	
	INLINE EntityCall* clientEntityCall() const;
	DECLARE_PY_GET_MOTHOD(pyGetClientEntityCall);
	INLINE void clientEntityCall(EntityCall* entityCall);

	/**
		all_clients
	*/
	INLINE AllClients* allClients() const;
	DECLARE_PY_GET_MOTHOD(pyGetAllClients);
	INLINE void allClients(AllClients* clients);

	/**
		other_clients
	*/
	INLINE AllClients* otherClients() const;
	DECLARE_PY_GET_MOTHOD(pyGetOtherClients);
	INLINE void otherClients(AllClients* clients);

	/**
		Script gets controlledBy property
	*/
	INLINE bool isControlledNotSelfClient() const;
	INLINE EntityCall* controlledBy() const;
	INLINE void controlledBy(EntityCall* baseEntityCall);
	DECLARE_PY_GETSET_MOTHOD(pyGetControlledBy, pySetControlledBy);
	bool setControlledBy(EntityCall* baseEntityCall);
	void sendControlledByStatusMessage(EntityCall* baseEntityCall, int8 isControlled);

	/** 
		The script gets and sets the position of the entity
	*/
	INLINE Position3D& position();
	INLINE void position(const Position3D& pos);
	DECLARE_PY_GETSET_MOTHOD(pyGetPosition, pySetPosition);

	/** 
		The script gets and sets the direction of the entity
	*/
	INLINE Direction3D& direction();
	INLINE void direction(const Direction3D& dir);
	DECLARE_PY_GETSET_MOTHOD(pyGetDirection, pySetDirection);

	/**
		Whether on the ground
	*/
	INLINE void isOnGround(bool v);
	INLINE bool isOnGround() const;
	DECLARE_PY_GET_MOTHOD(pyGetIsOnGround);

	/** 
		Set the entity direction and position
	*/
	void setPositionAndDirection(const Position3D& pos, 
		const Direction3D& dir);
	
	void onPositionChanged();
	void onDirectionChanged();
	
	void onPyPositionChanged();
	void onPyDirectionChanged();
	
	void updateLastPos();

	bool checkMoveForTopSpeed(const Position3D& position);

		/** Network Interface
		Client set new location
	*/
	void setPosition_XZ_int(Network::Channel* pChannel, int32 x, int32 z);

		/** Network Interface
		Client set new location
	*/
	void setPosition_XYZ_int(Network::Channel* pChannel, int32 x, int32 y, int32 z);

		/** Network Interface
		Client setting location
	*/
	void setPosition_XZ_float(Network::Channel* pChannel, float x, float z);

		/** Network Interface
		Client setting location
	*/
	void setPosition_XYZ_float(Network::Channel* pChannel, float x, float y, float z);

	/**
		Transfer method on cell
	*/
	DECLARE_PY_MOTHOD_ARG3(pyTeleport, PyObject_ptr, PyObject_ptr, PyObject_ptr);
	void teleport(PyObject_ptr nearbyMBRef, Position3D& pos, Direction3D& dir);
	void teleportLocal(PyObject_ptr nearbyMBRef, Position3D& pos, Direction3D& dir);
	void teleportRefEntity(Entity* entity, Position3D& pos, Direction3D& dir);
	void teleportRefEntityCall(EntityCall* nearbyMBRef, Position3D& pos, Direction3D& dir);
	void onTeleportRefEntityCall(EntityCall* nearbyMBRef, Position3D& pos, Direction3D& dir);

	/**
		Transfer success and failure related callbacks
	*/
	void onTeleport();
	void onTeleportFailure();
	void onTeleportSuccess(PyObject* nearbyEntity, SPACE_ID lastSpaceID);
	void onReqTeleportOtherAck(Network::Channel* pChannel, ENTITY_ID nearbyMBRefID, 
		SPACE_ID destSpaceID, COMPONENT_ID componentID);

	/**
		Enter and leave the cell and other callbacks
	*/
	void onEnteredCell();
	void onEnteringCell();
	void onLeavingCell();
	void onLeftCell();
	
	/**
		Enter and leave the space and other callbacks
	*/
	void onEnterSpace(SpaceMemory* pSpace);
	void onLeaveSpace(SpaceMemory* pSpace);

	/** 
		When the cellapp terminates unexpectedly, the baseapp will restore it if it finds the appropriate cellapp.
		Will call this method
	*/
	void onRestore();

	/**
		Script debug view
	*/
	void debugView();
	DECLARE_PY_MOTHOD_ARG0(pyDebugView);

	/** 
		The current entity sets its own View radius.
	*/
	int32 setViewRadius(float radius, float hyst);
	float getViewRadius(void) const;
	float getViewHystArea(void) const;
	DECLARE_PY_MOTHOD_ARG2(pySetViewRadius, float, float);
	DECLARE_PY_MOTHOD_ARG0(pyGetViewRadius);
	DECLARE_PY_MOTHOD_ARG0(pyGetViewHystArea);

	/**
		return to observe all observers of this entity
	*/
	DECLARE_PY_MOTHOD_ARG0(pyGetWitnesses);

	/** 
		Whether the current entity is real
	*/
	DECLARE_PY_MOTHOD_ARG0(pyIsReal);
	
	/** 
		Send backup data to baseapp
	*/
	void backupCellData();

	/** 
		Notification before saving to the database
	*/
	void onWriteToDB();

	/** 
		The script gets and sets the position of the entity
	*/
	INLINE int8 layer() const;
	DECLARE_PY_GETSET_MOTHOD(pyGetLayer, pySetLayer);

	/** 
		Entity mobile navigation
	*/
	bool canNavigate();
	uint32 navigate(const Position3D& destination, float velocity, float distance,
					float maxMoveDistance, float maxSearchDistance,
					bool faceMovement, int8 layer, PyObject* userData);
	bool navigatePathPoints(std::vector<Position3D>& outPaths, const Position3D& destination, float maxSearchDistance, int8 layer);

	DECLARE_PY_MOTHOD_ARG0(pycanNavigate);
	DECLARE_PY_MOTHOD_ARG3(pyNavigatePathPoints, PyObject_ptr, float, int8);
	DECLARE_PY_MOTHOD_ARG8(pyNavigate, PyObject_ptr, float, float, float, float, int8, int8, PyObject_ptr);

	/** 
		Entity gets random points
	*/
	bool getRandomPoints(std::vector<Position3D>& outPoints, const Position3D& centerPos, float maxRadius, uint32 maxPoints, int8 layer);
	DECLARE_PY_MOTHOD_ARG4(pyGetRandomPoints, PyObject_ptr, float, uint32, int8);

	/** 
		Entity moves to a certain point
	*/
	uint32 moveToPoint(const Position3D& destination, float velocity, float distance,
			PyObject* userData, bool faceMovement, bool moveVertically);
	
	DECLARE_PY_MOTHOD_ARG6(pyMoveToPoint, PyObject_ptr, float, float, PyObject_ptr, int32, int32);

	/** 
		Entity moves to an entity
	*/
	uint32 moveToEntity(ENTITY_ID targetID, float velocity, float distance,
			PyObject* userData, bool faceMovement, bool moveVertically, const Position3D& offsetPos);
	
	static PyObject* __py_pyMoveToEntity(PyObject* self, PyObject* args);

	/**
	Entity mobile acceleration
	*/
	float accelerate(const char* type, float acceleration);
	DECLARE_PY_MOTHOD_ARG2(pyAccelerate, const_charptr, float);

	/** 
		The script gets and sets the maximum xz movement speed of the entity
	*/
	float topSpeed() const{ return topSpeed_; }
	INLINE void topSpeed(float speed);
	DECLARE_PY_GETSET_MOTHOD(pyGetTopSpeed, pySetTopSpeed);
	
	/** 
		The script gets and sets the highest y movement speed of the entity
	*/
	INLINE float topSpeedY() const;
	INLINE void topSpeedY(float speed);
	DECLARE_PY_GETSET_MOTHOD(pyGetTopSpeedY, pySetTopSpeedY);
	
	/** 
		The script requests a certain type of entities within a certain range
	*/
	static PyObject* __py_pyEntitiesInRange(PyObject* self, PyObject* args);

	/** 
		The script requests to get the entities in the scope of the View
	*/
	static PyObject* __py_pyEntitiesInView(PyObject* self, PyObject* args);
	PyObject* entitiesInView(bool pending);

	/**
		Set whether to get automatic backup
	*/
	INLINE int8 shouldAutoBackup() const;
	INLINE void shouldAutoBackup(int8 v);
	DECLARE_PY_GETSET_MOTHOD(pyGetShouldAutoBackup, pySetShouldAutoBackup);

		/** Network Interface
		Method of remotely calling this entity
	*/
	void onRemoteMethodCall(Network::Channel* pChannel, MemoryStream& s);
	void onRemoteCallMethodFromClient(Network::Channel* pChannel, ENTITY_ID srcEntityID, MemoryStream& s);
	void onRemoteMethodCall_(PropertyDescription* pComponentPropertyDescription, 
		MethodDescription* pMethodDescription, ENTITY_ID srcEntityID, MemoryStream& s);

	/**
		Observer
	*/
	INLINE Witness* pWitness() const;
	INLINE void pWitness(Witness* w);

	/** 
		Whether it is monitored by any proxy, if the entity has no client, then this value is valid.
	*/
	INLINE bool isWitnessed(void) const;
	DECLARE_PY_GET_MOTHOD(pyIsWitnessed);

	/** 
		Whether the entity is an observer
	*/
	INLINE bool hasWitness(void) const;
	DECLARE_PY_GET_MOTHOD(pyHasWitness);

	/** 
		I was observed by an observer myself.
	*/
	void addWitnessed(Entity* entity);

	/** 
		Remove an observer who observes itself
	*/
	void delWitnessed(Entity* entity);
	void onDelWitnessed();

	/**
		 Whether the specified entity is one of the people who observe themselves
	*/
	bool entityInWitnessed(ENTITY_ID entityID);

	INLINE const std::list<ENTITY_ID>&	witnesses();
	INLINE size_t witnessesSize() const;

		/** Network Interface
		Entity is bound to an observer (client)

	*/
	void setWitness(Witness* pWitness);
	void onGetWitnessFromBase(Network::Channel* pChannel);
	void onGetWitness(bool fromBase = false);

		/** Network Interface
		Entity lost an observer (client)

	*/
	void onLoseWitness(Network::Channel* pChannel);

	/** 
		Client update data
	*/
	void onUpdateDataFromClient(Ouroboros::MemoryStream& s);

	/** 
		Add a range trigger
	*/
	uint32 addProximity(float range_xz, float range_y, int32 userarg);
	DECLARE_PY_MOTHOD_ARG3(pyAddProximity, float, float, int32);

	/** 
		Method of calling a client entity
	*/
	DECLARE_PY_MOTHOD_ARG1(pyClientEntity, ENTITY_ID);

	/** 
		Restore all range triggers
		This happens when you are in teleport
	*/
	void restoreProximitys();

	/** 
		Delete a controller
	*/
	void cancelController(uint32 id);
	static PyObject* __py_pyCancelController(PyObject* self, PyObject* args);

	/** 
		An entity enters a range trigger of this entity
	*/
	void onEnterTrap(Entity* entity, float range_xz, float range_y, 
							uint32 controllerID, int32 userarg);

	/** 
		An entity leaves a range trigger of this entity
	*/
	void onLeaveTrap(Entity* entity, float range_xz, float range_y, 
							uint32 controllerID, int32 userarg);

	/** 
		When the entity jumps to a new space, the exit range trigger event will trigger the interface.
	*/
	void onLeaveTrapID(ENTITY_ID entityID, 
							float range_xz, float range_y, 
							uint32 controllerID, int32 userarg);

	/** 
		An entity enters the View area
	*/
	void onEnteredView(Entity* entity);

	/** 
		Stop any mobile behavior
	*/
	bool stopMove();

	/** 
		Entity's one move is completed
	*/
	void onMove(uint32 controllerId, int layer, const Position3D& oldPos, PyObject* userarg);

	/** 
		Entity's move is complete
	*/
	void onMoveOver(uint32 controllerId, int layer, const Position3D& oldPos, PyObject* userarg);

	/** 
		Entity move failed
	*/
	void onMoveFailure(uint32 controllerId, PyObject* userarg);

	/**
		Entity turning direction
	*/
	uint32 addYawRotator(float yaw, float velocity,
		PyObject* userData);

	DECLARE_PY_MOTHOD_ARG3(pyAddYawRotator, float, float, PyObject_ptr);
	
	/**
		Entity turn to completion
	*/
	void onTurn(uint32 controllerId, PyObject* userarg);
	
	/**
		Get your position in the entities of the space
	*/
	INLINE SPACE_ENTITIES::size_type spaceEntityIdx() const;
	INLINE void spaceEntityIdx(SPACE_ENTITIES::size_type idx);

	/**
		Get the node where the entity is located
	*/
	INLINE EntityCoordinateNode* pEntityCoordinateNode() const;
	INLINE void pEntityCoordinateNode(EntityCoordinateNode* pNode);

	/**
		Install the uninstall node
	*/
	void installCoordinateNodes(CoordinateSystem* pCoordinateSystem);
	void uninstallCoordinateNodes(CoordinateSystem* pCoordinateSystem);
	void onCoordinateNodesDestroy(EntityCoordinateNode* pEntityCoordinateNode);

	/**
		Get the orientation of the entity position changed at a certain time
	*/
	INLINE GAME_TIME posChangedTime() const;
	INLINE GAME_TIME dirChangedTime() const;

	/** 
		Real request to update the attribute to ghost
	*/
	void onUpdateGhostPropertys(Ouroboros::MemoryStream& s);
	
	/** 
		Ghost request to call def method real
	*/
	void onRemoteRealMethodCall(Ouroboros::MemoryStream& s);

	/** 
		Real request to update the attribute to ghost
	*/
	void onUpdateGhostVolatileData(Ouroboros::MemoryStream& s);

	/** 
		Change to ghost, you must be real
	*/
	void changeToGhost(COMPONENT_ID realCell, Ouroboros::MemoryStream& s);

	/** 
		Change to real, it must be ghost
	*/
	void changeToReal(COMPONENT_ID ghostCell, Ouroboros::MemoryStream& s);

	void addToStream(Ouroboros::MemoryStream& s);
	void createFromStream(Ouroboros::MemoryStream& s);

	void addTimersToStream(Ouroboros::MemoryStream& s);
	void createTimersFromStream(Ouroboros::MemoryStream& s);

	void addControllersToStream(Ouroboros::MemoryStream& s);
	void createControllersFromStream(Ouroboros::MemoryStream& s);

	void addWitnessToStream(Ouroboros::MemoryStream& s);
	void createWitnessFromStream(Ouroboros::MemoryStream& s);

	void addMovementHandlerToStream(Ouroboros::MemoryStream& s);
	void createMovementHandlerFromStream(Ouroboros::MemoryStream& s);
	
	void addEventsToStream(Ouroboros::MemoryStream& s);
	void createEventsFromStream(Ouroboros::MemoryStream& s);

	/** 
		Obtain the entity controller manager
	*/
	INLINE Controllers*	pControllers() const;

	/** 
		Set whether the entity persistence data is dirty, dirty will automatically archive
	*/
	INLINE void setDirty(uint32* digest = NULL);
	INLINE bool isDirty() const;
	
	/**
		VolatileInfo section
	*/
	INLINE VolatileInfo* pCustomVolatileinfo(void);
	DECLARE_PY_GETSET_MOTHOD(pyGetVolatileinfo, pySetVolatileinfo);

	/**
		Calling the entity's callback function, it may be cached
	*/
	bool bufferOrExeCallback(const char * funcName, PyObject * funcArgs, bool notFoundIsOK = true);
	static void bufferCallback(bool enable);

private:
	/** 
		Send teleport results to the base
	*/
	void _sendBaseTeleportResult(ENTITY_ID sourceEntityID, COMPONENT_ID sourceBaseAppID, 
		SPACE_ID spaceID, SPACE_ID lastSpaceID, bool fromCellTeleport);

private:
	struct BufferedScriptCall
	{
		EntityPtr		entityPtr;
		PyObject *		pyCallable;
		// can be NULL, NULL indicates no parameters
		PyObject *		pyFuncArgs;
		const char*		funcName;
	};

	typedef std::list<BufferedScriptCall*>					BufferedScriptCallArray;
	static BufferedScriptCallArray							_scriptCallbacksBuffer;
	static int32											_scriptCallbacksBufferNum;
	static int32											_scriptCallbacksBufferCount;

protected:
	// The entityCall of the client part of this entity
	EntityCall*												clientEntityCall_;

	// the entityCall of the baseapp part of the entity
	EntityCall*												baseEntityCall_;

	/** The coordinates and orientation of this entity are currently controlled by the client
	    Null means no client is in control (ie system control),
	    Otherwise, point to the baseEntityCall_ of the object that controls this entity,
		The player controls himself by Entity.controlledBy = self.base
	*/
	EntityCall *											controlledBy_;

	// If an entity is ghost, then the entity will have a source cell pointing
	COMPONENT_ID											realCell_;

	// If an entity is real, the entity may have a ghost pointing
	COMPONENT_ID											ghostCell_;	

	// The current location of the entity
	Position3D												lastpos_;
	Position3D												position_;
	script::ScriptVector3*									pPyPosition_;

	// The current direction of the entity
	Direction3D												direction_;	
	script::ScriptVector3*									pPyDirection_;

	// The entity position orientation has changed at a certain time
	// This property can be used, for example: to decide if you want to synchronize the entity highly during a certain period
	GAME_TIME												posChangedTime_;
	GAME_TIME												dirChangedTime_;

	// Is it on the ground?
	bool													isOnGround_;

	// entity x, z axis maximum moving speed
	float													topSpeed_;

	// entity y axis maximum moving speed
	float													topSpeedY_;

	// position in the entities of the space
	SPACE_ENTITIES::size_type								spaceEntityIdx_;

	// Is it monitored by any observer?
	std::list<ENTITY_ID>									witnesses_;
	size_t													witnesses_count_;

	// Observer object
	Witness*												pWitness_;

	AllClients*												allClients_;
	AllClients*												otherClients_;

	//entity node
	EntityCoordinateNode*									pEntityCoordinateNode_;	

	// Controller Manager
	Controllers*											pControllers_;
	KBEShared_ptr<Controller>								pMoveController_;
	KBEShared_ptr<Controller>								pTurnController_;
	
	script::ScriptVector3::PYVector3ChangedCallback			pyPositionChangedCallback_;
	script::ScriptVector3::PYVector3ChangedCallback			pyDirectionChangedCallback_;
	
	// Entity layer, you can do any representation, tile-based games can be expressed as sea, land and air, etc., pure 3d can also represent various layers
	// You can search by layer when doing search in the script layer.
	int8													layer_;
	
	// Whether the data that needs to be persisted is dirty (memory sha1), no persistence if it is not dirty
	uint32													persistentDigest_[5];

	// If the user has set Volatileinfo, create Volatileinfo here, otherwise use NULL to use Volatileinfo of ScriptDefModule
	VolatileInfo*											pCustomVolatileinfo_;
};

}

#ifdef CODE_INLINE
#include "entity.inl"
#endif
#endif // OURO_ENTITY_H
