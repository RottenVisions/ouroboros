#include "Entity.h"
#include "Ouroboros.h"
#include "OBEvent.h"
#include "DataTypes.h"
#include "Method.h"
#include "ScriptModule.h"
#include "EntityCall.h"
#include "Bundle.h"
#include "EntityDef.h"
#include "Property.h"
#include "KBDebug.h"

namespace Ouroboros
{

Entity::Entity():
	id_(0),
	className_(TEXT("")),
	isOnGround_(false),
	inWorld_(false),
	isControlled_(false),
	inited_(false),
	velocity_(0.f),
	position(),
	direction(),
	spaceID(0),
	entityLastLocalPos(),
	entityLastLocalDir()
{
}

Entity::~Entity()
{
	DEBUG_MSG("%s::~%s() %d", *className_, *className_, id());
}

void Entity::clear()
{

}

void Entity::__init__()
{

}

bool Entity::isPlayer()
{
	return id() == OuroborosApp::getSingleton().entity_id();
}

void Entity::enterWorld()
{
	inWorld_ = true;

	onEnterWorld();
	onComponentsEnterworld();

	UOBEventData_onEnterWorld* pEventData = NewObject<UOBEventData_onEnterWorld>();
	pEventData->entityID = id();
	pEventData->spaceID = OuroborosApp::getSingleton().spaceID();
	KBPos2UE4Pos(pEventData->position, position);
	pEventData->direction = direction;
	pEventData->moveSpeed = velocity_;
	pEventData->isOnGround = isOnGround_;
	pEventData->isPlayer = isPlayer();
	pEventData->entityClassName = className();
	pEventData->res = TEXT("");
	OUROBOROS_EVENT_FIRE(OBEventTypes::onEnterWorld, pEventData);
}

void Entity::onEnterWorld()
{

}

void Entity::leaveWorld()
{
	inWorld_ = false;

	onLeaveWorld();
	onComponentsLeaveworld();

	UOBEventData_onLeaveWorld* pEventData = NewObject<UOBEventData_onLeaveWorld>();
	pEventData->entityID = id();
	pEventData->spaceID = OuroborosApp::getSingleton().spaceID();
	pEventData->isPlayer = isPlayer();
	OUROBOROS_EVENT_FIRE(OBEventTypes::onLeaveWorld, pEventData);
}

void Entity::onLeaveWorld()
{

}

void Entity::enterSpace()
{
	inWorld_ = true;

	onEnterSpace();

	UOBEventData_onEnterSpace* pEventData = NewObject<UOBEventData_onEnterSpace>();
	pEventData->entityID = id();
	pEventData->spaceID = OuroborosApp::getSingleton().spaceID();
	KBPos2UE4Pos(pEventData->position, position);
	pEventData->direction = direction;
	pEventData->moveSpeed = velocity_;
	pEventData->isOnGround = isOnGround_;
	pEventData->isPlayer = isPlayer();
	pEventData->entityClassName = className();
	pEventData->res = TEXT("");
	OUROBOROS_EVENT_FIRE(OBEventTypes::onEnterSpace, pEventData);
	
	// To refresh the position of the presentation layer object immediately
	UOBEventData_set_position* pPosEventData = NewObject<UOBEventData_set_position>();
	KBPos2UE4Pos(pPosEventData->position, position);
	pPosEventData->entityID = id();
	pPosEventData->moveSpeed = velocity_;
	pPosEventData->isOnGround = isOnGround();
	OUROBOROS_EVENT_FIRE(OBEventTypes::set_position, pPosEventData);

	UOBEventData_set_direction* pDirEventData = NewObject<UOBEventData_set_direction>();
	KBDir2UE4Dir(pDirEventData->direction, direction);
	pDirEventData->entityID = id();
	OUROBOROS_EVENT_FIRE(OBEventTypes::set_direction, pDirEventData);
}

void Entity::onEnterSpace()
{

}

void Entity::leaveSpace()
{
	inWorld_ = false;

	onLeaveSpace();

	UOBEventData_onLeaveSpace* pEventData = NewObject<UOBEventData_onLeaveSpace>();
	pEventData->entityID = id();
	pEventData->spaceID = OuroborosApp::getSingleton().spaceID();
	pEventData->isPlayer = isPlayer();
	OUROBOROS_EVENT_FIRE(OBEventTypes::onLeaveSpace, pEventData);
}

void Entity::onLeaveSpace()
{

}

void Entity::onPositionChanged(const FVector& oldValue)
{
	//DEBUG_MSG("%s::onPositionChanged: (%f, %f, %f) => (%f, %f, %f)", *className, 
	//	oldValue.X, oldValue.Y, oldValue.Z, position.X, position.Y, position.Z); 
	
	if(isPlayer())
		OuroborosApp::getSingleton().entityServerPos(position);
	
	if(inWorld())
	{
		UOBEventData_set_position* pEventData = NewObject<UOBEventData_set_position>();
		KBPos2UE4Pos(pEventData->position, position);
		pEventData->entityID = id();
		pEventData->moveSpeed = velocity_;
		pEventData->isOnGround = isOnGround();
		OUROBOROS_EVENT_FIRE(OBEventTypes::set_position, pEventData);
	}
}

void Entity::onDirectionChanged(const FVector& oldValue)
{
	//DEBUG_MSG("%s::onDirectionChanged: (%f, %f, %f) => (%f, %f, %f)", *className, 
	//	oldValue.X, oldValue.Y, oldValue.Z, direction.X, direction.Y, direction.Z); 

	if (inWorld())
	{
		UOBEventData_set_direction* pEventData = NewObject<UOBEventData_set_direction>();
		KBDir2UE4Dir(pEventData->direction, direction);
		pEventData->entityID = id();
		OUROBOROS_EVENT_FIRE(OBEventTypes::set_direction, pEventData);
	}
}

}