// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_NAVIGATEHANDLER_H
#define OURO_NAVIGATEHANDLER_H

#include "move_controller.h"	
#include "math/math.h"
#include "navigation/navigation_handle.h"

namespace Ouroboros{

class NavigateHandler : public MoveToPointHandler
{
public:
	NavigateHandler(OUROShared_ptr<Controller>& pController, const Position3D& destPos, float velocity, float distance, bool faceMovement, 
		float maxMoveDistance, VECTOR_POS3D_PTR paths_ptr,
		PyObject* userarg);

	NavigateHandler();
	virtual ~NavigateHandler();
	
	void addToStream(Ouroboros::MemoryStream& s);
	void createFromStream(Ouroboros::MemoryStream& s);

	virtual bool requestMoveOver(const Position3D& oldPos);

	virtual bool isOnGround(){ return true; }

	virtual MoveType type() const { return MOVE_TYPE_NAV; }

protected:
	int destPosIdx_;
	VECTOR_POS3D_PTR paths_;

	float maxMoveDistance_;
};
 
}
#endif // OURO_NAVIGATEHANDLER_H

