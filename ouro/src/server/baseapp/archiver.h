// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_ARCHIVER_H
#define OURO_ARCHIVER_H

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

class Archiver
{
public:
	Archiver();
	~Archiver();
	
	void tick();
	void createArchiveTable();
	void archive(Entity& entity);

private:
	int						archiveIndex_;
	std::vector<ENTITY_ID> 	arEntityIDs_;
};


}

#endif // OURO_ARCHIVER_H
