// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_TASKS_H
#define OURO_TASKS_H

#include "common/task.h"
#include "common/common.h"

namespace Ouroboros
{

/**
 *	Tasks
 */
class Tasks
{
public:
	Tasks();
	~Tasks();

	void add(Task * pTask);
	bool cancel(Task * pTask);
	void process();
private:

	typedef std::vector<Ouroboros::Task *> Container;
	Container container_;
};

}

#endif // OURO_TASKS_H
