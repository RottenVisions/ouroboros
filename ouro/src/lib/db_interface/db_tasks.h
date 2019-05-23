// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_DB_TASKS_H
#define OURO_DB_TASKS_H

#include "common/common.h"
#include "common/timer.h"
#include "thread/threadtask.h"

namespace Ouroboros{ 

class MemoryStream;
class DBInterface;
class EntityTable;
class EntityTables;

/*
	Database thread task base class
*/

class DBTaskBase : public thread::TPTask
{
public:

	DBTaskBase():
	initTime_(timestamp())
	{
	}

	virtual ~DBTaskBase(){}
	virtual bool process();
	virtual bool db_thread_process() = 0;
	virtual DBTaskBase* tryGetNextTask(){ return NULL; }
	virtual thread::TPTask::TPTaskState presentMainThread();

	virtual void pdbi(DBInterface* ptr){ pdbi_ = ptr; }

	uint64 initTime() const{ return initTime_; }

protected:
	DBInterface* pdbi_;
	uint64 initTime_;
};

/**
	Execute a sql statement
*/
class DBTaskSyncTable : public DBTaskBase
{
public:
	DBTaskSyncTable(EntityTables* pEntityTables, KBEShared_ptr<EntityTable> pEntityTable);
	virtual ~DBTaskSyncTable();
	virtual bool db_thread_process();
	virtual thread::TPTask::TPTaskState presentMainThread();

protected:
	KBEShared_ptr<EntityTable> pEntityTable_;
	bool success_;
	EntityTables* pEntityTables_;
};


}
#endif // OURO_DB_TASKS_H
