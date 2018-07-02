// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

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
	/** Subclassing fills some py operations into derived classes */
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
		Destroying the scene
	*/
	DECLARE_PY_MOTHOD_ARG0(pyDestroySpace);
	void destroySpace(void);

	/**
		Trigger when the current entity's space is about to be destroyed
	*/
	void onSpaceGone();

	/**
		Determine if it is a realEntity
	*/
	INLINE bool isReal(void) const;

	/**
		Determine if you have a ghostEntity
	*/
	INLINE bool hasGhost(void) const;

	/**
		Determine if it is a realEntity
	*/
	INLINE COMPONENT_ID realCell(void) const;
	INLINE void realCell(COMPONENT_ID cellID);

	/**
		Determine if you have a ghostEntity
	*/
	INLINE COMPONENT_ID ghostCell(void) const;
	INLINE void ghostCell(COMPONENT_ID cellID);

	/**
		Defined attribute data was changed
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
		The script gets and sets the orientation of the entity
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
		Set the entity direction and location
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

	/** Network Interface
		Entity transfer
		@cellAppID: The destination of the app to send to
		@targetEntityID: the space to be delivered to this entity
		@sourceBaseAppID: It may be that the base request on some baseapp is teleport. If it is 0 then initiate for cellEntity
	*/
	void teleportFromBaseapp(Network::Channel* pChannel, COMPONENT_ID cellAppID, ENTITY_ID targetEntityID, COMPONENT_ID sourceBaseAppID);

	/**
		Cell transmission method
	*/
	DECLARE_PY_MOTHOD_ARG3(pyTeleport, PyObject_ptr, PyObject_ptr, PyObject_ptr);
	void teleport(PyObject_ptr nearbyMBRef, Position3D& pos, Direction3D& dir);
	void teleportLocal(PyObject_ptr nearbyMBRef, Position3D& pos, Direction3D& dir);
	void teleportRefEntity(Entity* entity, Position3D& pos, Direction3D& dir);
	void teleportRefEntityCall(EntityCall* nearbyMBRef, Position3D& pos, Direction3D& dir);
	void onTeleportRefEntityCall(EntityCall* nearbyMBRef, Position3D& pos, Direction3D& dir);

	/**
		Send callbacks related to success and failure
	*/
	void onTeleport();
	void onTeleportFailure();
	void onTeleportSuccess(PyObject* nearbyEntity, SPACE_ID lastSpaceID);
	void onReqTeleportOtherAck(Network::Channel* pChannel, ENTITY_ID nearbyMBRefID,
		SPACE_ID destSpaceID, COMPONENT_ID componentID);

	/**
		Enter the callback to leave the cell and so on
	*/
	void onEnteredCell();
	void onEnteringCell();
	void onLeavingCell();
	void onLeftCell();

	/**
		Enter the callback to leave space and so on
	*/
	void onEnterSpace(SpaceMemory* pSpace);
	void onLeaveSpace(SpaceMemory* pSpace);

	/**
		When cellapp unexpectedly terminates, baseapp can recover if it finds a suitable cellapp.
		Will call this method
	*/
	void onRestore();

	/**
		Script debugging view
	*/
	void debugView();
	DECLARE_PY_MOTHOD_ARG0(pyDebugView);

	/**
		The current entity sets its own View radius
	*/
	int32 setViewRadius(float radius, float hyst);
	float getViewRadius(void) const;
	float getViewHystArea(void) const;
	DECLARE_PY_MOTHOD_ARG2(pySetViewRadius, float, float);
	DECLARE_PY_MOTHOD_ARG0(pyGetViewRadius);
	DECLARE_PY_MOTHOD_ARG0(pyGetViewHystArea);

	/**
		Whether the current entity is real
	*/
	DECLARE_PY_MOTHOD_ARG0(pyIsReal);

	/**
		Send backup data to baseapp
	*/
	void backupCellData();

	/**
		Before you save to the database
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
		Entity gets a random point
	*/
	bool getRandomPoints(std::vector<Position3D>& outPoints, const Position3D& centerPos, float maxRadius, uint32 maxPoints, int8 layer);
	DECLARE_PY_MOTHOD_ARG4(pyGetRandomPoints, PyObject_ptr, float, uint32, int8);

	/**
		Entity moves to a point
	*/
	uint32 moveToPoint(const Position3D& destination, float velocity, float distance,
			PyObject* userData, bool faceMovement, bool moveVertically);

	DECLARE_PY_MOTHOD_ARG6(pyMoveToPoint, PyObject_ptr, float, float, PyObject_ptr, int32, int32);

	/**
		Entity moved to an entity
	*/
	uint32 moveToEntity(ENTITY_ID targetID, float velocity, float distance,
			PyObject* userData, bool faceMovement, bool moveVertically, const Position3D& offsetPos);

	static PyObject* __py_pyMoveToEntity(PyObject* self, PyObject* args);

	/**
	Entity move acceleration
	*/
	float accelerate(const char* type, float acceleration);
	DECLARE_PY_MOTHOD_ARG2(pyAccelerate, const_charptr, float);

	/**
		The script gets and sets the entity's maximum xz movement speed
	*/
	float topSpeed() const{ return topSpeed_; }
	INLINE void topSpeed(float speed);
	DECLARE_PY_GETSET_MOTHOD(pyGetTopSpeed, pySetTopSpeed);

	/**
		The script gets and sets the highest y-speed of the entity
	*/
	INLINE float topSpeedY() const;
	INLINE void topSpeedY(float speed);
	DECLARE_PY_GETSET_MOTHOD(pyGetTopSpeedY, pySetTopSpeedY);

	/**
		The script requests a certain range of entities of a certain type
	*/
	static PyObject* __py_pyEntitiesInRange(PyObject* self, PyObject* args);

	/**
		Script request to get View-scoped entities
	*/
	DECLARE_PY_MOTHOD_ARG0(pyEntitiesInView);

	/**
		Set whether to get automatic backup
	*/
	INLINE int8 shouldAutoBackup() const;
	INLINE void shouldAutoBackup(int8 v);
	DECLARE_PY_GETSET_MOTHOD(pyGetShouldAutoBackup, pySetShouldAutoBackup);

	/** Network Interface
		Method for remotely calling this entity
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
		Whether monitored by any proxy, if the entity does not have a client, this value is valid
	*/
	INLINE bool isWitnessed(void) const;
	DECLARE_PY_GET_MOTHOD(pyIsWitnessed);

	/**
		Whether the entity is an observer
	*/
	INLINE bool hasWitness(void) const;
	DECLARE_PY_GET_MOTHOD(pyHasWitness);

	/**
		Observed by an observer himself
	*/
	void addWitnessed(Entity* entity);

	/**
		Remove an observer who observes itself
	*/
	void delWitnessed(Entity* entity);
	void onDelWitnessed();

	/**
		 Whether the specified entity is one of the people observing oneself
	*/
	bool entityInWitnessed(ENTITY_ID entityID);

	INLINE const std::list<ENTITY_ID>&	witnesses();
	INLINE size_t witnessesSize() const;

	/** Network Interface
		The entity is bound to an observer (client)

	*/
	void setWitness(Witness* pWitness);
	void onGetWitnessFromBase(Network::Channel* pChannel);
	void onGetWitness(bool fromBase = false);

	/** Network Interface
		Entity loses an observer (client)

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
		Calling the client entity method
	*/
	DECLARE_PY_MOTHOD_ARG1(pyClientEntity, ENTITY_ID);

	/**
		Restore all range triggers
		This happens when teleport
	*/
	void restoreProximitys();

	/**
		Delete a controller
	*/
	void cancelController(uint32 id);
	static PyObject* __py_pyCancelController(PyObject* self, PyObject* args);

	/**
		An entity has entered a range of triggers for this entity
	*/
	void onEnterTrap(Entity* entity, float range_xz, float range_y,
							uint32 controllerID, int32 userarg);

	/**
		An entity leaves a range of triggers for this entity
	*/
	void onLeaveTrap(Entity* entity, float range_xz, float range_y,
							uint32 controllerID, int32 userarg);

	/**
		When the entity jumps to a new space, leaving the scope trigger event will trigger this interface
	*/
	void onLeaveTrapID(ENTITY_ID entityID,
							float range_xz, float range_y,
							uint32 controllerID, int32 userarg);

	/**
		An entity enters the View area
	*/
	void onEnteredView(Entity* entity);

	/**
		Stop any move
	*/
	bool stopMove();

	/**
		One move of the entity is complete
	*/
	void onMove(uint32 controllerId, int layer, const Position3D& oldPos, PyObject* userarg);

	/**
		Entitys move is complete
	*/
	void onMoveOver(uint32 controllerId, int layer, const Position3D& oldPos, PyObject* userarg);

	/**
		Entity failed to move
	*/
	void onMoveFailure(uint32 controllerId, PyObject* userarg);

	/**
		Entity rotation heading
	*/
	uint32 addYawRotator(float yaw, float velocity,
		PyObject* userData);

	DECLARE_PY_MOTHOD_ARG3(pyAddYawRotator, float, float, PyObject_ptr);

	/**
		Entity turn to complete
	*/
	void onTurn(uint32 controllerId, PyObject* userarg);

	/**
		Get its position in the entities of space
	*/
	INLINE SPACE_ENTITIES::size_type spaceEntityIdx() const;
	INLINE void spaceEntityIdx(SPACE_ENTITIES::size_type idx);

	/**
		Get the entity node
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
		Get the entity position orientation changed at some time
	*/
	INLINE GAME_TIME posChangedTime() const;
	INLINE GAME_TIME dirChangedTime() const;

	/**
		Real request to update attributes to ghost
	*/
	void onUpdateGhostPropertys(Ouroboros::MemoryStream& s);

	/**
		Ghost request to call def method real
	*/
	void onRemoteRealMethodCall(Ouroboros::MemoryStream& s);

	/**
		Real request to update attributes to ghost
	*/
	void onUpdateGhostVolatileData(Ouroboros::MemoryStream& s);

	/**
		Change to ghost, it must be real
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
		Get Entity Controller Manager
	*/
	INLINE Controllers*	pControllers() const;

	/**
		Set the entity persistence data is dirty, dirty will be automatically archived
	*/
	INLINE void setDirty(bool dirty = true);
	INLINE bool isDirty() const;

	/**
		VolatileInfo section
	*/
	INLINE VolatileInfo* pCustomVolatileinfo(void);
	DECLARE_PY_GETSET_MOTHOD(pyGetVolatileinfo, pySetVolatileinfo);

	/**
		Calling entity's callback function, may be cached
	*/
	bool bufferOrExeCallback(const char * funcName, PyObject * funcArgs, bool notFoundIsOK = true);
	static void bufferCallback(bool enable);

private:
	/**
		Send teleport result to base end
	*/
	void _sendBaseTeleportResult(ENTITY_ID sourceEntityID, COMPONENT_ID sourceBaseAppID,
		SPACE_ID spaceID, SPACE_ID lastSpaceID, bool fromCellTeleport);

private:
	struct BufferedScriptCall
	{
		EntityPtr		entityPtr;
		PyObject *		pyCallable;
		// Can be NULL, NULL means no parameters
		PyObject *		pyFuncArgs;
		const char*		funcName;
	};

	typedef std::list<BufferedScriptCall*>					BufferedScriptCallArray;
	static BufferedScriptCallArray							_scriptCallbacksBuffer;
	static int32											_scriptCallbacksBufferNum;
	static int32											_scriptCallbacksBufferCount;

protected:
	// The entityCall of the entity part of this entity
	EntityCall*												clientEntityCall_;

	// The entityCall of the baseapp part of this entity
	EntityCall*												baseEntityCall_;

	/** The entity's coordinates and orientation are controlled by the current client
	    Null means no client is in control (ie system control)
	    Otherwise, it points to the baseEntityCall_ of the object that controls the entity.
		Players control their own Entity.controlledBy = self.base
	*/
	EntityCall *											controlledBy_;

	// If an entity is ghost, then the entity will have a source cell pointing
	COMPONENT_ID											realCell_;

	// If an entity is real then the entity may have a ghost pointing
	COMPONENT_ID											ghostCell_;

	// The current position of the entity
	Position3D												lastpos_;
	Position3D												position_;
	script::ScriptVector3*									pPyPosition_;

	// The current direction of the entity
	Direction3D												direction_;
	script::ScriptVector3*									pPyDirection_;

	// Whether entity position has changed at some time
	// This attribute can be used, for example, to determine whether to highly synchronize the entity during a certain period of time.
	GAME_TIME												posChangedTime_;
	GAME_TIME												dirChangedTime_;

	// Whether on the ground
	bool													isOnGround_;

	// entity x,z axis maximum speed
	float													topSpeed_;

	// entity Maximum y-axis movement speed
	float													topSpeedY_;

	// Its own position in the entities of space
	SPACE_ENTITIES::size_type								spaceEntityIdx_;

	// Is it monitored by any observer
	std::list<ENTITY_ID>									witnesses_;
	size_t													witnesses_count_;

	// Observer object
	Witness*												pWitness_;

	AllClients*												allClients_;
	AllClients*												otherClients_;

	// Entity node
	EntityCoordinateNode*									pEntityCoordinateNode_;

	// Controller Manager
	Controllers*											pControllers_;
	OUROShared_ptr<Controller>								pMoveController_;
	OUROShared_ptr<Controller>								pTurnController_;

	script::ScriptVector3::PYVector3ChangedCallback			pyPositionChangedCallback_;
	script::ScriptVector3::PYVector3ChangedCallback			pyDirectionChangedCallback_;

	// The entity layer can be expressed arbitrarily. Tile-based games can be represented by layers such as land, sea and air. Pure 3D can also represent various layers.
	// When searching in the script layer, you can search by layer.
	int8													layer_;

	// Whether the data that needs to be persisted is dirty or not persistent if it is not dirty
	bool													isDirty_;

	// If the user has set up Volatileinfo, Volatileinfo is created here, otherwise it is NULL. Use Volatileinfo of ScriptDefModule
	VolatileInfo*											pCustomVolatileinfo_;
};

}

#ifdef CODE_INLINE
#include "entity.inl"
#endif
#endif // OURO_ENTITY_H
