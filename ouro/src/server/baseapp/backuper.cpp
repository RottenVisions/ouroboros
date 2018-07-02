// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#include "baseapp.h"
#include "backuper.h"
#include "server/serverconfig.h"

namespace Ouroboros{
float backupPeriod = 0.0;

//-------------------------------------------------------------------------------------
Backuper::Backuper():
backupEntityIDs_(),
backupRemainder_(0.f)
{
}

//-------------------------------------------------------------------------------------
Backuper::~Backuper()
{
	backupEntityIDs_.clear();
}

//-------------------------------------------------------------------------------------
void Backuper::tick()
{
	int32 periodInTicks = (int32)secondsToTicks(ServerConfig::getSingleton().getBaseApp().backupPeriod, 0);
	if (periodInTicks == 0)
		return;

	// Here is a batch operation for the backup entity
	// Probably the algorithm is to convert the backup period filled in the configuration to the number of ticks, part of each tick backup entity
	float numToBackUpFloat = float(Baseapp::getSingleton().pEntities()->size()) / periodInTicks + backupRemainder_;

	// The number of backups
	int numToBackUp = int(numToBackUpFloat);

	// Calculate the amount of loss due to accuracy
	backupRemainder_ = numToBackUpFloat - numToBackUp;

	// If there is no content in the backup table, regenerate a new one
	if (backupEntityIDs_.empty())
	{
		this->createBackupTable();
	}

	MemoryStream* s = MemoryStream::createPoolObject();

	while((numToBackUp > 0) && !backupEntityIDs_.empty())
	{
		Entity * pEntity = Baseapp::getSingleton().findEntity(backupEntityIDs_.back());
		backupEntityIDs_.pop_back();

		if (pEntity && backup(*pEntity, *s))
		{
			--numToBackUp;
		}

		s->clear(false);
	}

	MemoryStream::reclaimPoolObject(s);
}

//-------------------------------------------------------------------------------------
bool Backuper::backup(Entity& entity, MemoryStream& s)
{
	// This begins to write the data that needs to be backed up to the stream
	entity.writeBackupData(&s);

	if(entity.shouldAutoBackup() == OURO_NEXT_ONLY)
		entity.shouldAutoBackup(0);

	return true;
}

//-------------------------------------------------------------------------------------
void Backuper::createBackupTable()
{
	backupEntityIDs_.clear();

	Entities<Entity>::ENTITYS_MAP::const_iterator iter = Baseapp::getSingleton().pEntities()->getEntities().begin();

	for(; iter != Baseapp::getSingleton().pEntities()->getEntities().end(); ++iter)
	{
		Entity* pEntity = static_cast<Entity*>(iter->second.get());

		if(pEntity->shouldAutoBackup() > 0)
			backupEntityIDs_.push_back(iter->first);
	}

	// Random sequence
	std::random_shuffle(backupEntityIDs_.begin(), backupEntityIDs_.end());
}

}
