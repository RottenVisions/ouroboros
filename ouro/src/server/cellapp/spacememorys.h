// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_SPACEMEMORYS_H
#define OURO_SPACEMEMORYS_H

#include "helper/debug_helper.h"
#include "common/common.h"
#include "common/singleton.h"
#include "updatable.h"
#include "spacememory.h"

namespace Ouroboros{

class SpaceMemorys
{
public:
	SpaceMemorys();
	~SpaceMemorys();
	
	static void finalise();

	typedef std::map<SPACE_ID, OUROShared_ptr<SpaceMemory> > SPACEMEMORYS;

	/** 
		Create a new space
	*/
	static SpaceMemory* createNewSpace(SPACE_ID spaceID, const std::string& scriptModuleName);
	
	/**
		Destroy a space
	*/
	static bool destroySpace(SPACE_ID spaceID, ENTITY_ID entityID);

	/** 
		Looking for a specified space
	*/
	static SpaceMemory* findSpace(SPACE_ID spaceID);
	
	/** 
		Update all spaces
	*/
	static void update();

	static size_t size(){ return spaces_.size(); }

protected:
	static SPACEMEMORYS spaces_;
};

}
#endif
