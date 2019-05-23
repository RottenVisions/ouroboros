// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_CLIENT_ENTITY_ASPECT_H
#define OURO_CLIENT_ENTITY_ASPECT_H

#include "common/common.h"


namespace Ouroboros{


class EntityAspect
{
public:
	EntityAspect(const EntityAspect& entityAspect);
	EntityAspect(ENTITY_ID aspectID);
	EntityAspect();
	virtual ~EntityAspect();
	
	void modelres(const std::string& modelres){ modelres_ = modelres; }
	const std::string& modelres(){ return modelres_; }

	ENTITY_ID aspectID() const{ return aspectID_; }

	float modelScale() const{ return modelScale_; }
protected:
	ENTITY_ID aspectID_;
	std::string modelres_;
	float modelScale_;	
};

}
#endif
