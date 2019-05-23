// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_OURO_TABLE_MYSQL_H
#define OURO_OURO_TABLE_MYSQL_H

#include "common.h"
#include "common/common.h"
#include "common/singleton.h"
#include "helper/debug_helper.h"
#include "db_interface/entity_table.h"
#include "db_interface/ouro_tables.h"

namespace Ouroboros { 

/*
	Kbe system table
*/
class KBEEntityLogTableMysql : public KBEEntityLogTable
{
public:
	KBEEntityLogTableMysql(EntityTables* pEntityTables);
	virtual ~KBEEntityLogTableMysql(){}
	
	/**
		Synchronize tables into the database
	*/
	virtual bool syncToDB(DBInterface* pdbi);
	virtual bool syncIndexToDB(DBInterface* pdbi){ return true; }

	virtual bool logEntity(DBInterface * pdbi, const char* ip, uint32 port, DBID dbid,
						COMPONENT_ID componentID, ENTITY_ID entityID, ENTITY_SCRIPT_UID entityType);

	virtual bool queryEntity(DBInterface * pdbi, DBID dbid, EntityLog& entitylog, ENTITY_SCRIPT_UID entityType);

	virtual bool eraseEntityLog(DBInterface * pdbi, DBID dbid, ENTITY_SCRIPT_UID entityType);
	virtual bool eraseBaseappEntityLog(DBInterface * pdbi, COMPONENT_ID componentID);

protected:
	
};

class KBEServerLogTableMysql : public KBEServerLogTable
{
public:
	KBEServerLogTableMysql(EntityTables* pEntityTables);
	virtual ~KBEServerLogTableMysql(){}
	
	/**
		Synchronize tables into the database
	*/
	virtual bool syncToDB(DBInterface* pdbi);
	virtual bool syncIndexToDB(DBInterface* pdbi){ return true; }

	virtual bool updateServer(DBInterface * pdbi);

	virtual bool queryServer(DBInterface * pdbi, ServerLog& serverlog);
	virtual std::vector<COMPONENT_ID> queryServers(DBInterface * pdbi);

	virtual std::vector<COMPONENT_ID> queryTimeOutServers(DBInterface * pdbi);
	
	virtual bool clearServers(DBInterface * pdbi, const std::vector<COMPONENT_ID>& cids);

	virtual std::map<COMPONENT_ID, bool> queryAllServerShareDBState(DBInterface * pdbi);

	virtual int isShareDB(DBInterface * pdbi);
	
protected:
	
};

class KBEAccountTableMysql : public KBEAccountTable
{
public:
	KBEAccountTableMysql(EntityTables* pEntityTables);
	virtual ~KBEAccountTableMysql(){}
	
	/**
		Synchronize tables into the database
	*/
	virtual bool syncToDB(DBInterface* pdbi);
	virtual bool syncIndexToDB(DBInterface* pdbi){ return true; }

	bool queryAccount(DBInterface * pdbi, const std::string& name, ACCOUNT_INFOS& info);
	bool queryAccountAllInfos(DBInterface * pdbi, const std::string& name, ACCOUNT_INFOS& info);
	bool logAccount(DBInterface * pdbi, ACCOUNT_INFOS& info);
	bool setFlagsDeadline(DBInterface * pdbi, const std::string& name, uint32 flags, uint64 deadline);
	virtual bool updateCount(DBInterface * pdbi, const std::string& name, DBID dbid);
	virtual bool updatePassword(DBInterface * pdbi, const std::string& name, const std::string& password);
protected:
};

class KBEEmailVerificationTableMysql : public KBEEmailVerificationTable
{
public:

	KBEEmailVerificationTableMysql(EntityTables* pEntityTables);
	virtual ~KBEEmailVerificationTableMysql();

	/**
		Synchronize tables into the database
	*/
	virtual bool syncToDB(DBInterface* pdbi);
	virtual bool syncIndexToDB(DBInterface* pdbi){ return true; }

	virtual bool queryAccount(DBInterface * pdbi, int8 type, const std::string& name, ACCOUNT_INFOS& info);
	virtual bool logAccount(DBInterface * pdbi, int8 type, const std::string& name, const std::string& datas, const std::string& code);
	virtual bool delAccount(DBInterface * pdbi, int8 type, const std::string& name);
	virtual bool activateAccount(DBInterface * pdbi, const std::string& code, ACCOUNT_INFOS& info);
	virtual bool bindEMail(DBInterface * pdbi, const std::string& name, const std::string& code);
	virtual bool resetpassword(DBInterface * pdbi, const std::string& name, 
		const std::string& password, const std::string& code);

protected:
};

}

#endif // OURO_OURO_TABLE_MYSQL_H
