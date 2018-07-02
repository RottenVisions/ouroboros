// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_View_TRIGGER_H
#define OURO_View_TRIGGER_H

#include "range_trigger.h"

namespace Ouroboros{

class Witness;

class ViewTrigger : public RangeTrigger
{
public:
	ViewTrigger(CoordinateNode* origin, float xz = 0.0f, float y = 0.0f);
	virtual ~ViewTrigger();

	/**
		A node enters or leaves the rangeTrigger
	*/
	virtual void onEnter(CoordinateNode * pNode);
	virtual void onLeave(CoordinateNode * pNode);

	INLINE Witness* pWitness() const;

protected:
	Witness* pWitness_;
};

}

#ifdef CODE_INLINE
#include "view_trigger.inl"
#endif
#endif
