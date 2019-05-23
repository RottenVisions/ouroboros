// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_DB_INTERFACE_H
#define OURO_DB_INTERFACE_H

#include "common/common.h"
#include "common/singleton.h"
#include "helper/debug_helper.h"
#include "db_interface/entity_table.h"
#include "server/serverconfig.h"

namespace Ouroboros { 

namespace thread
{
class ThreadPool;
}

class DBUtil;

/*
	Database interface
*/
class DBInterface
{
public:
	enum DB_OP_STATE
	{
		DB_OP_READ,
		DB_OP_WRITE,
	};

	friend class DBUtil;

	DBInterface(const char* name) :
	db_port_(3306),
	db_numConnections_(1),
	lastquery_()
	{
		strncpy(name_, name, MAX_NAME);
		int dbIndex = g_ouroSrvConfig.dbInterfaceName2dbInterfaceIndex(this->name());
		OURO_ASSERT(dbIndex >= 0);
		dbIndex_ = dbIndex;
	};

	virtual ~DBInterface()
	{
	};

	/**
		Check the environment
	*/
	virtual bool checkEnvironment() = 0;
	
	/**
		Check for errors and correct the contents of the error
		if the correction fails, the return fails.
	*/
	virtual bool checkErrors() = 0;

	/**
		Associated with a database
	*/
	virtual bool attach(const char* databaseName = NULL) = 0;
	virtual bool detach() = 0;

	/**
		Get all the table names of the database
	*/
	virtual bool getTableNames( std::vector<std::string>& tableNames, const char * pattern) = 0;

	/**
		Get all the field names of a table in the database
	*/
	virtual bool getTableItemNames(const char* tableName, std::vector<std::string>& itemNames) = 0;

	/**
		Query list
	*/
	virtual bool query(const char* cmd, uint32 size, bool printlog = true, MemoryStream * result = NULL) = 0;
	virtual bool query(const std::string& cmd, bool printlog = true, MemoryStream * result = NULL)
	{
		return query(cmd.c_str(), (uint32)cmd.size(), printlog, result);
	}

	/**
		return the name of this interface
	*/
	const char* name() const { return name_; }

	/**
		returns the index of this interface
	*/
	uint16 dbIndex() const { return dbIndex_; }

	/**
		return a description of this interface
	*/
	virtual const char* c_str() = 0;

	/** 
		Get error
	*/
	virtual const char* getstrerror() = 0;

	/** 
		Get the error number
	*/
	virtual int getlasterror() = 0;

	/**
		Create an entity storage table
	*/
	virtual EntityTable* createEntityTable(EntityTables* pEntityTables) = 0;

	/** 
		Remove the entity table from the database
	*/
	virtual bool dropEntityTableFromDB(const char* tableName) = 0;

	/** 
		Remove the entity table field from the database
	*/
	virtual bool dropEntityTableItemFromDB(const char* tableName, const char* tableItemName) = 0;

	/**
		Lock interface operation
	*/
	virtual bool lock() = 0;
	virtual bool unlock() = 0;

	/**
		Handling exceptions
	*/
	virtual bool processException(std::exception & e) = 0;

	/**
		Get the sql statement of the last query
	*/
	virtual const std::string& lastquery() const{ return lastquery_; }

protected:
	char name_[MAX_BUF]; // name of the database interface
	char db_type_[MAX_BUF]; // category of the database
	uint32 db_port_; // port of the database
	char db_ip_[MAX_IP]; // ip address of the database
	char db_username_[MAX_BUF]; // username of the database
	char db_password_[MAX_BUF * 10]; // password for the database
	char db_name_[MAX_BUF]; // database name
	uint16 db_numConnections_; // database maximum connection
	std::string lastquery_; // last query description
	uint16 dbIndex_; // corresponding database interface index
};

/*
	Database operating unit
*/
class DBUtil : public Singleton<DBUtil>
{
public:
	DBUtil();
	~DBUtil();
	
	static bool initialize();
	static void finalise();
	static bool initializeWatcher();

	static bool initThread(const std::string& dbinterfaceName);
	static bool finiThread(const std::string& dbinterfaceName);

	static DBInterface* createInterface(const std::string& name, bool showinfo = true);
	static const char* accountScriptName();
	static bool initInterface(DBInterface* pdbi);

	static void handleMainTick();

	typedef OUROUnordered_map<std::string, thread::ThreadPool*> DBThreadPoolMap;
	static thread::ThreadPool* pThreadPool(const std::string& name)
	{ 
		DBThreadPoolMap::iterator iter = pThreadPoolMaps_.find(name);
		if (iter != pThreadPoolMaps_.end())
			return iter->second;

		return NULL;
	}

private:
	static DBThreadPoolMap pThreadPoolMaps_;
};

}

#endif // OURO_DB_INTERFACE_H
