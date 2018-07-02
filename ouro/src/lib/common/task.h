// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_TASK_H
#define OURO_TASK_H

#include "common/common.h"

namespace Ouroboros
{

/**
 *	Abstract one task
 */
class Task
{
public:
	virtual ~Task() {}
	virtual bool process() = 0;
};


}

#endif // OURO_TASK_H
