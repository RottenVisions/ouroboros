// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

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
		/** Subclassing populates some py operations into derived classes*/
	BASE_SCRIPT_HREADER(Entity, ScriptObject)	
	ENTITY_HEADER(Entity)
		
public:
	Entity(ENTITY_ID id, const ScriptDefModule* pScriptModule, EntityCall* base, EntityCall* cell);
	~Entity();
	
	/** 
		Defining attribute data has been changed
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
		The script gets and sets the direction of the entity
	*/
	INLINE Direction3D& direction();
	INLINE void direction(const Direction3D& dir);
	void onDirectionChanged();
	DECLARE_PY_GETSET_MOTHOD(pyGetDirection, pySetDirection);
	
	/**
		The location and orientation of the physical client
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
		Entity moves to a certain point
	*/
	uint32 moveToPoint(const Position3D& destination, float velocity, float distance,
			PyObject* userData, bool faceMovement, bool moveVertically);
	
	DECLARE_PY_MOTHOD_ARG6(pyMoveToPoint, PyObject_ptr, float, float, PyObject_ptr, int32, int32);

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
		Method of remotely calling this entity
	*/
	void onRemoteMethodCall(Network::Channel* pChannel, MemoryStream& s);

	/**
		Server update entity attribute
	*/
	void onUpdatePropertys(MemoryStream& s);
	
	/**
	    When setting the data for Entity for the first time, decide whether to call back the set_* method of the script layer.
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

	bool isPlayer();
	DECLARE_PY_MOTHOD_ARG0(pyIsPlayer);

protected:
	EntityCall* cellEntityCall_; // cell-entityCall of this entity
	EntityCall* baseEntityCall_; // base-entityCall of this entity

	Position3D position_, serverPosition_; //The current position of the entity
	Direction3D direction_; //The current direction of the entity

	Position3D clientPos_; // client location, if the entity is controlled by the client to synchronize the location with the server
	Direction3D clientDir_; // Client orientation, if the entity is controlled by the client for synchronization to the server

	ClientObjectBase*						pClientApp_;

	EntityAspect							aspect_;

	float									velocity_;

	bool enterworld_; // Is it already enterworld, useful when restore
	
	bool									isOnGround_;

	ScriptID								pMoveHandlerID_;
	
	bool inited_; // set to true after __init__ call

    bool isControlled_; // is it controlled?
};																										

}
}

#ifdef CODE_INLINE
#include "entity.inl"
#endif
#endif
