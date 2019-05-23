// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_TASK_H
#define OURO_TASK_H

#include "common/common.h"

namespace Ouroboros
{

/**
 ?* Abstract a task
 */
class Task
{
public:
	virtual ~Task() {}
	virtual bool process() = 0;
};


}

#endif // OURO_TASK_H
