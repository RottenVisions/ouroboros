// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#include "cellapp.h"
#include "entity.h"
#include "move_controller.h"	
#include "moveto_point_handler.h"	
#include "moveto_entity_handler.h"	
#include "navigate_handler.h"	

namespace Ouroboros{	


//-------------------------------------------------------------------------------------
MoveController::MoveController(Entity* pEntity, MoveToPointHandler* pMoveToPointHandler, uint32 id):
Controller(Controller::CONTROLLER_TYPE_MOVE, pEntity, 0, id),
pMoveToPointHandler_(pMoveToPointHandler)
{
}

//-------------------------------------------------------------------------------------
MoveController::~MoveController()
{
	// DEBUG_MSG(fmt::format("MoveController::~MoveController(): {:p}\n", (void*)this);
	if (pMoveToPointHandler_)
	{
		pMoveToPointHandler_->destroy();
		pMoveToPointHandler_ = NULL;
	}
}

//-------------------------------------------------------------------------------------
void MoveController::addToStream(Ouroboros::MemoryStream& s)
{
	Controller::addToStream(s);

	uint8 utype = pMoveToPointHandler_->type();
	s << utype;

	pMoveToPointHandler_->addToStream(s);
}

//-------------------------------------------------------------------------------------
void MoveController::createFromStream(Ouroboros::MemoryStream& s)
{
	Controller::createFromStream(s);
	OURO_ASSERT(pMoveToPointHandler_ == NULL);

	uint8 utype;
	s >> utype;

	if(utype == MoveToPointHandler::MOVE_TYPE_NAV)
		pMoveToPointHandler_ = new NavigateHandler();
	else if(utype == MoveToPointHandler::MOVE_TYPE_ENTITY)
		pMoveToPointHandler_ = new MoveToEntityHandler();
	else if(utype == MoveToPointHandler::MOVE_TYPE_POINT)
		pMoveToPointHandler_ = new MoveToPointHandler();
	else
		OURO_ASSERT(false);

	pMoveToPointHandler_->createFromStream(s);
}

//-------------------------------------------------------------------------------------
void MoveController::destroy()
{
	Controller::destroy();

	// Since you have to destroy it, the updatable associated with yourself should also stop.
	if (pMoveToPointHandler_)
	{
		pMoveToPointHandler_->destroy();
		pMoveToPointHandler_ = NULL;
	}
}

//-------------------------------------------------------------------------------------
}

