// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#include "trap_trigger.h"
#include "entity.h"
#include "entity_coordinate_node.h"
#include "proximity_controller.h"	

#ifndef CODE_INLINE
#include "trap_trigger.inl"
#endif

namespace Ouroboros{	


//-------------------------------------------------------------------------------------
TrapTrigger::TrapTrigger(CoordinateNode* origin, ProximityController* pProximityController, float xz, float y):
RangeTrigger(origin, xz, y),
pProximityController_(pProximityController)
{
}

//-------------------------------------------------------------------------------------
TrapTrigger::~TrapTrigger()
{
}

//-------------------------------------------------------------------------------------
void TrapTrigger::onEnter(CoordinateNode * pNode)
{
	if((pNode->flags() & COORDINATE_NODE_FLAG_ENTITY) <= 0)
		return;

	pProximityController_->onEnter(static_cast<EntityCoordinateNode*>(pNode)->pEntity(), range_xz_, range_y_);
}

//-------------------------------------------------------------------------------------
void TrapTrigger::onLeave(CoordinateNode * pNode)
{
	if((pNode->flags() & COORDINATE_NODE_FLAG_ENTITY) <= 0)
		return;

	pProximityController_->onLeave(static_cast<EntityCoordinateNode*>(pNode)->pEntity(), range_xz_, range_y_);
}

//-------------------------------------------------------------------------------------
}
