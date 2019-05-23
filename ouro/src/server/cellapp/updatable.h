// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_UPDATABLE_H
#define OURO_UPDATABLE_H

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

/*
	Used to describe an object that will always be updated, the app will call all the tick
	Updatable to update the state, you need to implement different Updatable to complete different update features.
*/
class Updatable
{
public:
	Updatable();
	~Updatable();

	virtual bool update() = 0;

	virtual uint8 updatePriority() const {
		return 0;
	}

	std::string c_str() { return updatableName; }

	// its position in the Updatables container
	int removeIdx;

	std::string updatableName;
};

}
#endif
