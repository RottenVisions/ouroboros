// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_TURNENTITYCONTROLLERBASE_H
#define OURO_TURNENTITYCONTROLLERBASE_H

#include "controller.h"
#include "updatable.h"
#include "rotator_handler.h"
#include "pyscript/scriptobject.h"	

namespace Ouroboros{

class TurnController : public Controller
{
public:
	TurnController(Entity* pEntity, RotatorHandler* pRotatorHandler = NULL, uint32 id = 0);
	virtual ~TurnController();
	
	void pRotatorHandler(RotatorHandler* pRotatorHandler)
	{
		pRotatorHandler_ = pRotatorHandler;
	}
	
	virtual void destroy();
	virtual void addToStream(Ouroboros::MemoryStream& s);
	virtual void createFromStream(Ouroboros::MemoryStream& s);

	float velocity() const {
		return pRotatorHandler_->velocity();
	}

	void velocity(float v) {
		pRotatorHandler_->velocity(v);
	}

protected:
	RotatorHandler* pRotatorHandler_;
};

}
#endif // OURO_TURNENTITYCONTROLLERBASE_H

