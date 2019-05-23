// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_MOVETOENTITYHANDLER_H
#define OURO_MOVETOENTITYHANDLER_H

#include "moveto_point_handler.h"	


namespace Ouroboros{

class MoveToEntityHandler : public MoveToPointHandler
{
public:
	MoveToEntityHandler(KBEShared_ptr<Controller>& pController, ENTITY_ID pTargetID, float velocity, float range, bool faceMovement, 
		bool moveVertically, PyObject* userarg, const Position3D& offsetPos);

	MoveToEntityHandler();
	virtual ~MoveToEntityHandler();
	
	void addToStream(Ouroboros::MemoryStream& s);
	void createFromStream(Ouroboros::MemoryStream& s);

	virtual bool update();

	virtual const Position3D& destPos();

	virtual MoveType type() const{ return MOVE_TYPE_ENTITY; }

protected:
	ENTITY_ID pTargetID_;
	Position3D offsetPos_;
	Position3D destPos_;
};
 
}
#endif // OURO_MOVETOENTITYHANDLER_H

