// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_MOVETOPOINTCONTROLLERBASE_H
#define OURO_MOVETOPOINTCONTROLLERBASE_H

#include "controller.h"
#include "updatable.h"
#include "moveto_point_handler.h"
#include "pyscript/scriptobject.h"	

namespace Ouroboros{

class MoveController : public Controller
{
public:
	MoveController(Entity* pEntity, MoveToPointHandler* pMoveToPointHandler = NULL, uint32 id = 0);
	virtual ~MoveController();
	
	void pMoveToPointHandler(MoveToPointHandler* pMoveToPointHandler)
		{ pMoveToPointHandler_ = pMoveToPointHandler; }

	virtual void destroy();
	virtual void addToStream(Ouroboros::MemoryStream& s);
	virtual void createFromStream(Ouroboros::MemoryStream& s);

	float velocity() const {
		return pMoveToPointHandler_->velocity();
	}

	void velocity(float v) {
		pMoveToPointHandler_->velocity(v);
	}

protected:
	MoveToPointHandler* pMoveToPointHandler_;
};
 
}
#endif // OURO_MOVETOPOINTCONTROLLERBASE_H

