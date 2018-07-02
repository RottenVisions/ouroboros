// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#include "baseapp.h"
#include "archiver.h"
#include "entity.h"

namespace Ouroboros{

//-------------------------------------------------------------------------------------
Archiver::Archiver():
	archiveIndex_(INT_MAX),
	arEntityIDs_()
{
}

//-------------------------------------------------------------------------------------
Archiver::~Archiver()
{
}

//-------------------------------------------------------------------------------------
void Archiver::tick()
{
	int32 periodInTicks = (int32)secondsToTicks(ServerConfig::getSingleton().getBaseApp().archivePeriod, 0);
	if (periodInTicks == 0)
		return;

	if (archiveIndex_ >= periodInTicks)
	{
		this->createArchiveTable();
	}

	// The algorithm is as follows:
	// The number of baseEntity * idx / tick period = one section at a time in the vector
	// This section is processed in each gametick, just finished the task in periodInTicks smoothly.
	// If archiveIndex_ &gt;= periodInTicks then regenerate a random sequence
	int size = (int)arEntityIDs_.size();
	int startIndex = size * archiveIndex_ / periodInTicks;

	++archiveIndex_;

	int endIndex   = size * archiveIndex_ / periodInTicks;

	for (int i = startIndex; i < endIndex; ++i)
	{
		Entity* pEntity = Baseapp::getSingleton().findEntity(arEntityIDs_[i]);

		if(pEntity && pEntity->hasDB())
		{
			this->archive(*pEntity);
		}
	}
}

//-------------------------------------------------------------------------------------
void Archiver::archive(Entity& entity)
{
	entity.writeToDB(NULL, NULL, NULL);

	if(entity.shouldAutoArchive() == OURO_NEXT_ONLY)
		entity.shouldAutoArchive(0);
}

//-------------------------------------------------------------------------------------
void Archiver::createArchiveTable()
{
	archiveIndex_ = 0;
	arEntityIDs_.clear();

	Entities<Entity>::ENTITYS_MAP::const_iterator iter = Baseapp::getSingleton().pEntities()->getEntities().begin();

	for(; iter != Baseapp::getSingleton().pEntities()->getEntities().end(); ++iter)
	{
		Entity* pEntity = static_cast<Entity*>(iter->second.get());

		if(pEntity->hasDB() && pEntity->shouldAutoArchive() > 0)
		{
			arEntityIDs_.push_back(iter->first);
		}
	}

	// Random sequence
	std::random_shuffle(arEntityIDs_.begin(), arEntityIDs_.end());
}

}
