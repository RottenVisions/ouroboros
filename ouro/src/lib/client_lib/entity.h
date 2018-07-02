// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_CLIENTAPP_ENTITY_H
#define OURO_CLIENTAPP_ENTITY_H

#include "entity_aspect.h"
#include "client_lib/profile.h"
#include "common/timer.h"
#include "common/common.h"
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
class EntityCall;
class ClientObjectBase;
class EntityComponent;

namespace Network
{
class Channel;
}

namespace client
{

class Entity : public script::ScriptObject
{
	/** Subclassing fills some py operations into derived classes */
	BASE_SCRIPT_HREADER(Entity, ScriptObject)
	ENTITY_HEADER(Entity)

public:
	Entity(ENTITY_ID id, const ScriptDefModule* pScriptModule, EntityCall* base, EntityCall* cell);
	~Entity();

	/**
		Defined attribute data was changed
	*/
	void onDefDataChanged(EntityComponent* pEntityComponent, const PropertyDescription* propertyDescription,
			PyObject* pyData);

	/**
		entityCall section
	*/
	INLINE EntityCall* baseEntityCall() const;
	DECLARE_PY_GET_MOTHOD(pyGetBaseEntityCall);
	INLINE void baseEntityCall(EntityCall* entityCall);

	INLINE EntityCall* cellEntityCall() const;
	DECLARE_PY_GET_MOTHOD(pyGetCellEntityCall);
	INLINE void cellEntityCall(EntityCall* entityCall);

	/**
		The script gets and sets the position of the entity
	*/
	INLINE Position3D& position();
	INLINE Position3D& serverPosition();
	INLINE void position(const Position3D& pos);
	INLINE void serverPosition(const Position3D& pos);
	void onPositionChanged();
	DECLARE_PY_GETSET_MOTHOD(pyGetPosition, pySetPosition);

	/**
		The script gets and sets the orientation of the entity
	*/
	INLINE Direction3D& direction();
	INLINE void direction(const Direction3D& dir);
	void onDirectionChanged();
	DECLARE_PY_GETSET_MOTHOD(pyGetDirection, pySetDirection);

	/**
		Entity client location and orientation
	*/
	INLINE Position3D& clientPos();
	INLINE void clientPos(const Position3D& pos);
	INLINE void clientPos(float x, float y, float z);

	INLINE Direction3D& clientDir();
	INLINE void clientDir(const Direction3D& dir);
	INLINE void clientDir(float roll, float pitch, float yaw);

	/**
		Moving speed
	*/
	INLINE void moveSpeed(float speed);
	INLINE float moveSpeed() const;
	void onMoveSpeedChanged();
	DECLARE_PY_GETSET_MOTHOD(pyGetMoveSpeed, pySetMoveSpeed);

	/**
		pClientApp section
	*/
	DECLARE_PY_GET_MOTHOD(pyGetClientApp);
	void pClientApp(ClientObjectBase* p);
	INLINE ClientObjectBase* pClientApp() const;

	const EntityAspect* getAspect() const{ return &aspect_; }

	/**
		Entity moves to a point
	*/
	uint32 moveToPoint(const Position3D& destination, float velocity, float distance,
			PyObject* userData, bool faceMovement, bool moveVertically);

	DECLARE_PY_MOTHOD_ARG6(pyMoveToPoint, PyObject_ptr, float, float, PyObject_ptr, int32, int32);

	/**
		Stop any move
	*/
	bool stopMove();

	/**
		One move of the entity is complete
	*/
	void onMove(uint32 controllerId, int layer, const Position3D& oldPos, PyObject* userarg);

	/**
		Entity's move is complete
	*/
	void onMoveOver(uint32 controllerId, int layer, const Position3D& oldPos, PyObject* userarg);

	/**
		Entity failed to move
	*/
	void onMoveFailure(uint32 controllerId, PyObject* userarg);

	/**
		Delete a controller
	*/
	void cancelController(uint32 id);
	static PyObject* __py_pyCancelController(PyObject* self, PyObject* args);

	/**
		Destroy this entity
	*/
	void onDestroy(bool callScript){};

	void onEnterWorld();
	void onLeaveWorld();

	void onEnterSpace();
	void onLeaveSpace();

	/**
		Method for remotely calling this entity
	*/
	void onRemoteMethodCall(Network::Channel* pChannel, MemoryStream& s);

	/**
		The server updates the entity attribute
	*/
	void onUpdatePropertys(MemoryStream& s);

	/**
	    When the data for the Entity is first set, decide whether to call back the set_* method of the script layer
	*/
	void callPropertysSetMethods();

	bool inWorld() const{ return enterworld_; }

	void onBecomePlayer();
	void onBecomeNonPlayer();

	bool isOnGround() const { return isOnGround_;}
	void isOnGround(bool v) { isOnGround_ = v;}

	INLINE bool isInited();
	INLINE void isInited(bool status);

    bool isControlled() { return isControlled_; }
    void onControlled(bool p_controlled);

protected:
	EntityCall*								cellEntityCall_;					// cell-entityCall for this entity
	EntityCall*								baseEntityCall_;					// The base-entityCall of this entity

	Position3D								position_, serverPosition_;			// The current position of the entity
	Direction3D								direction_;							// The current direction of the entity

	Position3D								clientPos_;							// The client position, if the entity is the client of the control to the server for the synchronization position
	Direction3D								clientDir_;							// Client orientation, if the entity is the client of the control to the server for synchronization toward the

	ClientObjectBase*						pClientApp_;

	EntityAspect							aspect_;

	float									velocity_;

	bool									enterworld_;						// Whether it is already enterworld, useful when restoring

	bool									isOnGround_;

	ScriptID								pMoveHandlerID_;

	bool									inited_;							// Set to true after __init__ call

    bool                                    isControlled_;                      // Is it controlled
};

}
}

#ifdef CODE_INLINE
#include "entity.inl"
#endif
#endif
