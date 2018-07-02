// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_TRAP_TRIGGER_H
#define OURO_TRAP_TRIGGER_H

#include "range_trigger.h"

namespace Ouroboros{

class Entity;
class ProximityController;

class TrapTrigger : public RangeTrigger
{
public:
	TrapTrigger(CoordinateNode* origin, ProximityController* pProximityController, float xz = 0.0f, float y = 0.0f);
	virtual ~TrapTrigger();



	/**
		A node enters or leaves the rangeTrigger
	*/
	virtual void onEnter(CoordinateNode * pNode);
	virtual void onLeave(CoordinateNode * pNode);

protected:
	ProximityController* pProximityController_;
};

}

#ifdef CODE_INLINE
#include "trap_trigger.inl"
#endif
#endif
