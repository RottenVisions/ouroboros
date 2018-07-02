// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_OURO_TABLE_REDIS_H
#define OURO_OURO_TABLE_REDIS_H

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
class OUROEntityLogTableRedis : public OUROEntityLogTable
{
public:
	OUROEntityLogTableRedis(EntityTables* pEntityTables);
	virtual ~OUROEntityLogTableRedis(){}

	/**
		Synchronize tables into the database
	*/
	virtual bool syncToDB(DBInterface* pdbi);
	virtual bool syncIndexToDB(DBInterface* pdbi){ return true; }

	virtual bool logEntity(DBInterface * pdbi, const char* ip, uint32 port, DBID dbid,
						COMPONENT_ID componentID, ENTITY_ID entityID, ENTITY_SCRIPT_UID entityType);

	virtual bool queryEntity(DBInterface * pdbi, DBID dbid, EntityLog& entitylog, ENTITY_SCRIPT_UID entityType);

	virtual bool eraseEntityLog(DBInterface * pdbi, DBID dbid, ENTITY_SCRIPT_UID entityType);

protected:

};

class OUROServerLogTableRedis : public OUROServerLogTable
{
public:
	OUROServerLogTableRedis(EntityTables* pEntityTables);
	virtual ~OUROServerLogTableRedis(){}

	/**
		Synchronize tables into the database
	*/
	virtual bool syncToDB(DBInterface* pdbi);
	virtual bool syncIndexToDB(DBInterface* pdbi){ return true; }

	virtual bool updateServer(DBInterface * pdbi);

	virtual bool queryServer(DBInterface * pdbi, ServerLog& serverlog);

	virtual std::vector<COMPONENT_ID> queryTimeOutServers(DBInterface * pdbi);

	virtual bool clearTimeoutLogs(DBInterface * pdbi, const std::vector<COMPONENT_ID>& cids);

protected:

};

class OUROAccountTableRedis : public OUROAccountTable
{
public:
	OUROAccountTableRedis(EntityTables* pEntityTables);
	virtual ~OUROAccountTableRedis(){}

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

class OUROEmailVerificationTableRedis : public OUROEmailVerificationTable
{
public:

	OUROEmailVerificationTableRedis(EntityTables* pEntityTables);
	virtual ~OUROEmailVerificationTableRedis();

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

	int getDeadline(int8 type);

protected:
};

}

#endif // OURO_OURO_TABLE_REDIS_H
