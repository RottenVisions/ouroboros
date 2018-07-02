// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#include "dbmgr.h"
#include "dbtasks.h"
#include "update_dblog_handler.h"
#include "db_interface/db_interface.h"
#include "db_interface/ouro_tables.h"
#include "server/serverconfig.h"
#include "thread/threadpool.h"

namespace Ouroboros{	

//-------------------------------------------------------------------------------------
UpdateDBServerLogHandler::UpdateDBServerLogHandler() : 
pTimerHandle_(NULL)
{
	pTimerHandle_ = new TimerHandle();
	(*pTimerHandle_) = Dbmgr::getSingleton().dispatcher().addTimer(OUROServerLogTable::TIMEOUT * 1000000, this,
							NULL);
}

//-------------------------------------------------------------------------------------
UpdateDBServerLogHandler::~UpdateDBServerLogHandler()
{
	cancel();
}

//-------------------------------------------------------------------------------------
void UpdateDBServerLogHandler::cancel()
{
	if(pTimerHandle_)
	{
		pTimerHandle_->cancel();
		delete pTimerHandle_;
		pTimerHandle_ = NULL;
	}
}

//-------------------------------------------------------------------------------------
void UpdateDBServerLogHandler::handleTimeout(TimerHandle, void * arg)
{
	std::string dbInterfaceName = "default";
	
	DBUtil::pThreadPool(dbInterfaceName)->
		addTask(new DBTaskServerLog());	
}

//-------------------------------------------------------------------------------------

}
