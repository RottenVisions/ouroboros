// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_BACKUPER_H
#define OURO_BACKUPER_H

// common include
#include "helper/debug_helper.h"
#include "common/common.h"
// #define NDEBUG
// windows include
#if OURO_PLATFORM == PLATFORM_WIN32
#else
// linux include
#endif

namespace Ouroboros{

class Backuper
{
public:
	Backuper();
	~Backuper();

	void tick();

	void createBackupTable();

	bool backup(Entity& entity, MemoryStream& s);

private:
	// The entities in this list will be backed up
	std::vector<ENTITY_ID>		backupEntityIDs_;

	float						backupRemainder_;
};


}

#endif // OURO_BACKUPER_H
