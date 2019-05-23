// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_LOADNAVMESH_THREADTASKS_H
#define OURO_LOADNAVMESH_THREADTASKS_H

#include "common/common.h"
#include "thread/threadtask.h"
#include "helper/debug_helper.h"

namespace Ouroboros{ 

class LoadNavmeshTask : public thread::TPTask
{
public:
	LoadNavmeshTask(const std::string& resPath, SPACE_ID spaceID, const std::map< int, std::string >& params):
	resPath_(resPath),
	spaceID_(spaceID),
	params_(params)
	{
	}

	virtual ~LoadNavmeshTask(){}
	virtual bool process();
	virtual thread::TPTask::TPTaskState presentMainThread();

protected:
	std::string resPath_;
	SPACE_ID spaceID_;
	std::map< int, std::string > params_;
};


}

#endif // OURO_LOADNAVMESH_THREADTASKS_H
