// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_DB_THREAD_POOL_H
#define OURO_DB_THREAD_POOL_H

// common include
// #define NDEBUG
#include "db_tasks.h"
#include "common/common.h"
#include "common/memorystream.h"
#include "thread/threadtask.h"
#include "helper/debug_helper.h"
#include "thread/threadpool.h"

namespace Ouroboros{

/*
	Database thread task buffer
*/
class TPThread;

class DBThreadPool : public thread::ThreadPool
{
public:
	DBThreadPool(const std::string& dbinterfaceName);
	~DBThreadPool();

	virtual thread::TPThread* createThread(int threadWaitSecond = 0, bool threadStartsImmediately = true);

	virtual std::string name() const{ return std::string("DBThreadPool/") + dbinterfaceName_; }

protected:
	std::string dbinterfaceName_;
};

}
#endif
