// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_OURO_TABLES_H
#define OURO_OURO_TABLES_H

#include "entity_table.h"
#include "common/common.h"
#include "common/memorystream.h"
#include "helper/debug_helper.h"

namespace Ouroboros {

class OUROTable : public EntityTable
{
public:
	OUROTable(EntityTables* pEntityTables) :
	EntityTable(pEntityTables)
	{
	}

	virtual ~OUROTable()
	{
	}

	/**
		Synchronize the entity table into the database
	*/
	virtual bool syncToDB(DBInterface* pdbi) = 0;

	/**
		initialization
	*/
	virtual bool initialize(ScriptDefModule* sm, std::string name) { return true; };

	virtual EntityTableItem* createItem(std::string type, std::string defaultVal) { return NULL; }

protected:

};

/*
	Kbe system table
*/
class OUROEntityLogTable : public OUROTable
{
public:
	struct EntityLog
	{
		DBID dbid;
		ENTITY_ID entityID;
		char ip[MAX_IP];
		uint16 port;
		COMPONENT_ID componentID;

		// Who records
		COMPONENT_ID logger;
	};

	OUROEntityLogTable(EntityTables* pEntityTables) :
	OUROTable(pEntityTables)
	{
		tableName(OURO_TABLE_PERFIX "_entitylog");
	}

	virtual ~OUROEntityLogTable()
	{
	}

	virtual bool logEntity(DBInterface * pdbi, const char* ip, uint32 port, DBID dbid,
						COMPONENT_ID componentID, ENTITY_ID entityID, ENTITY_SCRIPT_UID entityType) = 0;

	virtual bool queryEntity(DBInterface * pdbi, DBID dbid, EntityLog& entitylog, ENTITY_SCRIPT_UID entityType) = 0;

	virtual bool eraseEntityLog(DBInterface * pdbi, DBID dbid, ENTITY_SCRIPT_UID entityType) = 0;

protected:

};

/*
	Kbe system table
*/
class OUROServerLogTable : public OUROTable
{
public:
	const static uint32 TIMEOUT = 3600;

	struct ServerLog
	{
		uint64 heartbeatTime;

		// Who records
		COMPONENT_ID logger;
	};

	OUROServerLogTable(EntityTables* pEntityTables) :
	OUROTable(pEntityTables)
	{
		tableName(OURO_TABLE_PERFIX "_serverlog");
	}

	virtual ~OUROServerLogTable()
	{
	}

	virtual bool updateServer(DBInterface * pdbi) = 0;

	virtual bool queryServer(DBInterface * pdbi, ServerLog& serverlog) = 0;

	virtual std::vector<COMPONENT_ID> queryTimeOutServers(DBInterface * pdbi) = 0;

	virtual bool clearTimeoutLogs(DBInterface * pdbi, const std::vector<COMPONENT_ID>& cids) = 0;

protected:

};

class OUROAccountTable : public OUROTable
{
public:
	OUROAccountTable(EntityTables* pEntityTables) :
	OUROTable(pEntityTables),
	accountDefMemoryStream_()
	{
		tableName(OURO_TABLE_PERFIX "_accountinfos");
	}

	virtual ~OUROAccountTable()
	{
	}

	virtual bool queryAccount(DBInterface * pdbi, const std::string& name, ACCOUNT_INFOS& info) = 0;
	virtual bool logAccount(DBInterface * pdbi, ACCOUNT_INFOS& info) = 0;
	virtual bool setFlagsDeadline(DBInterface * pdbi, const std::string& name, uint32 flags, uint64 deadline) = 0;
	virtual bool updateCount(DBInterface * pdbi, const std::string& name, DBID dbid) = 0;
	virtual bool queryAccountAllInfos(DBInterface * pdbi, const std::string& name, ACCOUNT_INFOS& info) = 0;
	virtual bool updatePassword(DBInterface * pdbi, const std::string& name, const std::string& password) = 0;

	MemoryStream& accountDefMemoryStream()
	{
		return accountDefMemoryStream_;
	}

	void accountDefMemoryStream(MemoryStream& s)
	{
		accountDefMemoryStream_.clear(false);
		accountDefMemoryStream_.append(s.data() + s.rpos(), s.length());
	}

protected:
	MemoryStream accountDefMemoryStream_;
};

class OUROEmailVerificationTable : public OUROTable
{
public:
	enum V_TYPE
	{
		V_TYPE_CREATEACCOUNT = 1,
		V_TYPE_RESETPASSWORD = 2,
		V_TYPE_BIND_MAIL = 3
	};

	OUROEmailVerificationTable(EntityTables* pEntityTables) :
	OUROTable(pEntityTables)
	{
		tableName(OURO_TABLE_PERFIX "_email_verification");
	}

	virtual ~OUROEmailVerificationTable()
	{
	}

	virtual bool queryAccount(DBInterface * pdbi, int8 type, const std::string& name, ACCOUNT_INFOS& info) = 0;
	virtual bool logAccount(DBInterface * pdbi, int8 type, const std::string& name, const std::string& datas, const std::string& code) = 0;
	virtual bool delAccount(DBInterface * pdbi, int8 type, const std::string& name) = 0;
	virtual bool activateAccount(DBInterface * pdbi, const std::string& code, ACCOUNT_INFOS& info) = 0;
	virtual bool bindEMail(DBInterface * pdbi, const std::string& name, const std::string& code) = 0;
	virtual bool resetpassword(DBInterface * pdbi, const std::string& name, const std::string& password, const std::string& code) = 0;

protected:
};

}

#endif // OURO_OURO_TABLES_H
