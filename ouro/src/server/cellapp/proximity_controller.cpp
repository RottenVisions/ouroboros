// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#include "trap_trigger.h"
#include "entity.h"
#include "proximity_controller.h"
#include "entity_coordinate_node.h"

namespace Ouroboros{


//-------------------------------------------------------------------------------------
ProximityController::ProximityController(Entity* pEntity, float xz, float y, int32 userarg, uint32 id):
Controller(CONTROLLER_TYPE_PROXIMITY, pEntity, userarg, id),
pTrapTrigger_(NULL),
xz_(xz),
y_(y)
{
	pTrapTrigger_ = new TrapTrigger(static_cast<EntityCoordinateNode*>(pEntity->pEntityCoordinateNode()),
								this, xz, y);

	pTrapTrigger_->install();
}

//-------------------------------------------------------------------------------------
ProximityController::ProximityController(Entity* pEntity):
Controller(pEntity),
pTrapTrigger_(NULL),
xz_(0.f),
y_(0.f)
{
}

//-------------------------------------------------------------------------------------
ProximityController::~ProximityController()
{
	pTrapTrigger_->uninstall();
	delete pTrapTrigger_;
}

//-------------------------------------------------------------------------------------
void ProximityController::addToStream(Ouroboros::MemoryStream& s)
{
	Controller::addToStream(s);
	s << xz_ << y_;
}

//-------------------------------------------------------------------------------------
void ProximityController::createFromStream(Ouroboros::MemoryStream& s)
{
	Controller::createFromStream(s);
	s >> xz_ >> y_;
}

//-------------------------------------------------------------------------------------
bool ProximityController::reinstall(CoordinateNode* pCoordinateNode)
{
	// This may happen when jumping across cellapp scenes
	// Because of using ProximityController::ProximityController(Entity* pEntity) structure
	if(pTrapTrigger_ == NULL)
	{
		pTrapTrigger_ = new TrapTrigger(static_cast<EntityCoordinateNode*>(pCoordinateNode),
								this, xz_, y_);
	}

	return pTrapTrigger_->reinstall(pCoordinateNode);
}

//-------------------------------------------------------------------------------------
void ProximityController::onEnter(Entity* pEntity, float xz, float y)
{
	pEntity_->onEnterTrap(pEntity, xz, y, id(), userarg());
}

//-------------------------------------------------------------------------------------
void ProximityController::onLeave(Entity* pEntity, float xz, float y)
{
	pEntity_->onLeaveTrap(pEntity, xz, y, id(), userarg());
}

//-------------------------------------------------------------------------------------
}
