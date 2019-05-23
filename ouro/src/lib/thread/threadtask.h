// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_THREADTASK_H
#define OURO_THREADTASK_H

// common include	
// #define NDEBUG
#include "common/common.h"
#include "common/task.h"
#include "helper/debug_helper.h"

namespace Ouroboros{ namespace thread{

/*
	Thread base thread class
*/

class TPTask : public Task
{
public:
	enum TPTaskState
	{
		/// A task has been completed
		TPTASK_STATE_COMPLETED = 0,

		/// Continue to execute on the main thread
		TPTASK_STATE_CONTINUE_MAINTHREAD = 1,

		// continue to execute in the child thread
		TPTASK_STATE_CONTINUE_CHILDTHREAD = 2,
	};

	/**
		return value: thread::TPTask::TPTaskState, see TPTaskState
	*/
	virtual thread::TPTask::TPTaskState presentMainThread(){ 
		return thread::TPTask::TPTASK_STATE_COMPLETED; 
	}
};

}
}

#endif // OURO_THREADTASK_H
