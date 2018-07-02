// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_DB_INTERFACE_MYSQL_H
#define OURO_DB_INTERFACE_MYSQL_H

#include "common.h"
#include "db_transaction.h"
#include "common/common.h"
#include "common/singleton.h"
#include "common/memorystream.h"
#include "helper/debug_helper.h"
#include "db_interface/db_interface.h"

#include "mysql/mysql.h"
#if OURO_PLATFORM == PLATFORM_WIN32
#ifdef X64
// added for VS2015
#if _MSC_VER >= 1900
#pragma comment (lib, "libmysql64_vs140.lib")
#pragma comment (lib, "mysqlclient64_vs140.lib")
#else
#pragma comment (lib, "libmysql64.lib")
#pragma comment (lib, "mysqlclient64.lib")
#endif
#else
// added for VS2015
#if _MSC_VER >= 1900
#pragma comment (lib, "libmysql32_vs140.lib")
#pragma comment (lib, "mysqlclient32_vs140.lib")
#else
#pragma comment (lib, "libmysql32.lib")
#pragma comment (lib, "mysqlclient32.lib")
#endif
#endif
#endif

namespace Ouroboros {

struct MYSQL_TABLE_FIELD
{
	std::string name;
	int32 length;
	uint64 maxlength;
	unsigned int flags;
	enum_field_types type;
};

/*
	Database interface
*/
class DBInterfaceMysql : public DBInterface
{
public:
	DBInterfaceMysql(const char* name, std::string characterSet, std::string collation);
	virtual ~DBInterfaceMysql();

	static bool initInterface(DBInterface* pdbi);

	/**
		Associated with a database
	*/
	bool reattach();
	virtual bool attach(const char* databaseName = NULL);
	virtual bool detach();

	bool ping(){
		return mysql_ping(pMysql_) == 0;
	}

	void inTransaction(bool value)
	{
		OURO_ASSERT(inTransaction_ != value);
		inTransaction_ = value;
	}

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

	virtual bool query(const char* strCommand, uint32 size, bool printlog = true, MemoryStream * result = NULL);

	bool write_query_result(MemoryStream * result);

	/**
		Get all database table names
	*/
	virtual bool getTableNames( std::vector<std::string>& tableNames, const char * pattern);

	/**
		Get all the field names of a table in the database
	*/
	virtual bool getTableItemNames(const char* tableName, std::vector<std::string>& itemNames);

	/**
		Delete the entity table field from the database
	*/
	virtual bool dropEntityTableItemFromDB(const char* tableName, const char* tableItemName);

	MYSQL* mysql(){ return pMysql_; }

	void throwError();

	my_ulonglong insertID()		{ return mysql_insert_id( pMysql_ ); }

	my_ulonglong affectedRows()	{ return mysql_affected_rows( pMysql_ ); }

	const char* info()			{ return mysql_info( pMysql_ ); }

	const char* getLastError()
	{
		if(pMysql_ == NULL)
			return "pMysql is NULL";

		return mysql_error( pMysql_ );
	}

	unsigned int getLastErrorNum() { return mysql_errno( pMysql_ ); }

	typedef OUROUnordered_map<std::string, MYSQL_TABLE_FIELD> TABLE_FIELDS;
	void getFields(TABLE_FIELDS& outs, const char* tableName);

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
		Create a database if the database does not exist
	*/
	virtual bool createDatabaseIfNotExist();

	/**
		Create an entity storage table
	*/
	virtual EntityTable* createEntityTable(EntityTables* pEntityTables);

	/**
		Delete the entity table from the database
	*/
	virtual bool dropEntityTableFromDB(const char* tableName);

	/**
		Lock the interface operation
	*/
	virtual bool lock();
	virtual bool unlock();

	/**
		Handling exceptions
	*/
	bool processException(std::exception & e);

	/**
		The maximum size of the SQL command
	*/
	static size_t sql_max_allowed_packet(){ return sql_max_allowed_packet_; }

protected:
	MYSQL* pMysql_;

	bool hasLostConnection_;

	bool inTransaction_;

	mysql::DBTransaction lock_;

	std::string characterSet_;
	std::string collation_;

	static size_t sql_max_allowed_packet_;
};


}

#endif // OURO_DB_INTERFACE_MYSQL_H
