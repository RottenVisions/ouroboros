// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_DB_INTERFACE_REDIS_H
#define OURO_DB_INTERFACE_REDIS_H

#include "common.h"
#include "common/common.h"
#include "common/singleton.h"
#include "common/memorystream.h"
#include "helper/debug_helper.h"
#include "db_interface/db_interface.h"

#include "hiredis.h"
#if OURO_PLATFORM == PLATFORM_WIN32
#ifdef _DEBUG
#pragma comment (lib, "hiredis_d.lib")
#else
#pragma comment (lib, "hiredis.lib")
#endif
#endif

namespace Ouroboros {

/*
	Database interface
	tbl_Account_Auto_increment = uint64(1)
	tbl_Account:1 = hashes(name, password, xxx)
	tbl_Account:2 = hashes(name, password, xxx)
	tbl_Account:3 = hashes(name, password, xxx(array))

	// array of type
	tbl_Account_xxx_values:3:size = uint64(3)
	tbl_Account_xxx_values:3:1 = val
	tbl_Account_xxx_values:3:2 = val
	tbl_Account_xxx_values:3:3 = val
*/
class DBInterfaceRedis : public DBInterface
{
public:
	DBInterfaceRedis(const char* name);
	virtual ~DBInterfaceRedis();

	static bool initInterface(DBInterface* pdbi);

	bool ping(redisContext* pRedisContext = NULL);

	void inTransaction(bool value)
	{
		OURO_ASSERT(inTransaction_ != value);
		inTransaction_ = value;
	}

	redisContext* context()				{ return pRedisContext_; }

	bool hasLostConnection() const		{ return hasLostConnection_; }
	void hasLostConnection( bool v )	{ hasLostConnection_ = v; }

	/**
		Check the environment
	*/
	virtual bool checkEnvironment();

	/**
		Check for errors, correct the contents of the errors
		If the correction fails to return failure
	*/
	virtual bool checkErrors();

	/**
		Associated with a database
	*/
	bool reattach();
	virtual bool attach(const char* databaseName = NULL);
	virtual bool detach();

	/**
		Get all database table names
	*/
	virtual bool getTableNames( std::vector<std::string>& tableNames, const char * pattern);

	/**
		Get all the field names of a table in the database
	*/
	virtual bool getTableItemNames(const char* tableName, std::vector<std::string>& itemNames);

	/**
		Query table
	*/
	virtual bool query(const char* cmd, uint32 size, bool printlog = true, MemoryStream * result = NULL);
	bool query(const std::string& cmd, redisReply** pRedisReply, bool printlog = true);
	bool query(bool printlog, const char* format, ...);
	bool queryAppend(bool printlog, const char* format, ...);
	bool getQueryReply(redisReply **pRedisReply);

	void write_query_result(redisReply* pRedisReply, MemoryStream * result);
	void write_query_result_element(redisReply* pRedisReply, MemoryStream * result);

	/**
		Return the description of this interface
	*/
	virtual const char* c_str();

	/**
		Get error
	*/
	virtual const char* getstrerror();

	/**
		Get error number
	*/
	virtual int getlasterror();

	/**
		Create an entity storage table
	*/
	virtual EntityTable* createEntityTable(EntityTables* pEntityTables);

	/**
		Delete the entity table from the database
	*/
	virtual bool dropEntityTableFromDB(const char* tableName);

	/**
		Delete the entity table field from the database
	*/
	virtual bool dropEntityTableItemFromDB(const char* tableName, const char* tableItemName);

	/**
		Lock the interface operation
	*/
	virtual bool lock();
	virtual bool unlock();

	void throwError();

	/**
		Handling exceptions
	*/
	virtual bool processException(std::exception & e);

protected:
	redisContext* pRedisContext_;
	bool hasLostConnection_;
	bool inTransaction_;
};


}

#endif // OURO_DB_INTERFACE_REDIS_H
