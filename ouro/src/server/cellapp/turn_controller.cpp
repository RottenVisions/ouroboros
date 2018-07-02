// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#include "cellapp.h"
#include "entity.h"
#include "turn_controller.h"

namespace Ouroboros{


//-------------------------------------------------------------------------------------
TurnController::TurnController(Entity* pEntity, RotatorHandler* pRotatorHandler, uint32 id) :
Controller(Controller::CONTROLLER_TYPE_ROTATE, pEntity, 0, id),
pRotatorHandler_(pRotatorHandler)
{
}

//-------------------------------------------------------------------------------------
TurnController::~TurnController()
{
	// DEBUG_MSG(fmt::format("TurnController::~TurnController(): {:p}\n", (void*)this);
	if (pRotatorHandler_)
		pRotatorHandler_->pController(OUROShared_ptr<Controller>());

	pRotatorHandler_ = NULL;
}

//-------------------------------------------------------------------------------------
void TurnController::addToStream(Ouroboros::MemoryStream& s)
{
	Controller::addToStream(s);
	pRotatorHandler_->addToStream(s);
}

//-------------------------------------------------------------------------------------
void TurnController::createFromStream(Ouroboros::MemoryStream& s)
{
	Controller::createFromStream(s);
	OURO_ASSERT(pRotatorHandler_ == NULL);

	pRotatorHandler_ = new RotatorHandler();
	pRotatorHandler_->createFromStream(s);
}

//-------------------------------------------------------------------------------------
void TurnController::destroy()
{
	Controller::destroy();

	// Since they are going to be destroyed, the updatable associated with them should also be stopped.
	if (pRotatorHandler_)
	{
		pRotatorHandler_->pController(OUROShared_ptr<Controller>());
		pRotatorHandler_ = NULL;
	}
}

//-------------------------------------------------------------------------------------
}
