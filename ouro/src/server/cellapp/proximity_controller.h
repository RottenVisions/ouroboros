// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_PROXIMITYCONTROLLER_H
#define OURO_PROXIMITYCONTROLLER_H

#include "controller.h"	


namespace Ouroboros{

class TrapTrigger;
class CoordinateNode;

/*
	π‹¿Ìtrap°£
*/
class ProximityController : public Controller
{
public:
	ProximityController(Entity* pEntity, float xz, float y, int32 userarg, uint32 id = 0);
	ProximityController(Entity* pEntity);
	~ProximityController();
	
	bool reinstall(CoordinateNode* pCoordinateNode);

	void onEnter(Entity* pEntity, float xz, float y);
	void onLeave(Entity* pEntity, float xz, float y);

	void addToStream(Ouroboros::MemoryStream& s);
	void createFromStream(Ouroboros::MemoryStream& s);

protected:
	TrapTrigger* pTrapTrigger_;
	float xz_; 
	float y_;
};

}
#endif // OURO_PROXIMITYCONTROLLER_H
