// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#include "spacememorys.h"	
namespace Ouroboros{	
SpaceMemorys::SPACEMEMORYS SpaceMemorys::spaces_;

//-------------------------------------------------------------------------------------
SpaceMemorys::SpaceMemorys()
{
}

//-------------------------------------------------------------------------------------
SpaceMemorys::~SpaceMemorys()
{
}

//-------------------------------------------------------------------------------------
void SpaceMemorys::finalise()
{
	SpaceMemorys::SPACEMEMORYS spaces = spaces_;
	while (spaces.size() > 0)
	{
		SPACEMEMORYS::iterator iter = spaces.begin();
		KBEShared_ptr<SpaceMemory> pSpace = iter->second;
		spaces.erase(iter++);
		pSpace->destroy(0, false);
	}

	spaces_.clear();
}

//-------------------------------------------------------------------------------------
SpaceMemory* SpaceMemorys::createNewSpace(SPACE_ID spaceID, const std::string& scriptModuleName)
{
	SPACEMEMORYS::iterator iter = spaces_.find(spaceID);
	if(iter != spaces_.end())
	{
		ERROR_MSG(fmt::format("Spaces::createNewSpace: space {} is exist! scriptModuleName={}\n", spaceID, scriptModuleName));
		return NULL;
	}
	
	SpaceMemory* space = new SpaceMemory(spaceID, scriptModuleName);
	spaces_[spaceID].reset(space);
	
	DEBUG_MSG(fmt::format("Spaces::createNewSpace: new space({}) {}.\n", scriptModuleName, spaceID));
	return space;
}

//-------------------------------------------------------------------------------------
bool SpaceMemorys::destroySpace(SPACE_ID spaceID, ENTITY_ID entityID)
{
	INFO_MSG(fmt::format("Spaces::destroySpace: {}.\n", spaceID));

	SpaceMemory* pSpace = SpaceMemorys::findSpace(spaceID);
	if(!pSpace)
		return true;
	
	if(pSpace->isDestroyed())
		return true;

	if(!pSpace->destroy(entityID))
	{
		//WARNING_MSG("Spaces::destroySpace: destroying!\n");
		return false;
	}

	// Delay after a period of time to destroy
	//spaces_.erase(spaceID);
	return true;
}

//-------------------------------------------------------------------------------------
SpaceMemory* SpaceMemorys::findSpace(SPACE_ID spaceID)
{
	SPACEMEMORYS::iterator iter = spaces_.find(spaceID);
	if(iter != spaces_.end())
		return iter->second.get();
	
	return NULL;
}

//-------------------------------------------------------------------------------------
void SpaceMemorys::update()
{
	SPACEMEMORYS::iterator iter = spaces_.begin();

	for(; iter != spaces_.end(); )
	{
		if(!iter->second->update())
		{
			spaces_.erase(iter++);
		}
		else
		{
			++iter;
		}
	}
}

//-------------------------------------------------------------------------------------
}
