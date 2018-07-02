// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_ENTITY_TABLE_REDIS_H
#define OURO_ENTITY_TABLE_REDIS_H
#include "db_interface_redis.h"
#include "common.h"
#include "common/common.h"
#include "common/singleton.h"
#include "helper/debug_helper.h"
#include "db_interface/entity_table.h"

namespace Ouroboros {

class ScriptDefModule;
class EntityTableRedis;

/*
	Maintaining a field in the database table in the entity table
*/
class EntityTableItemRedisBase : public EntityTableItem
{
public:
	EntityTableItemRedisBase(std::string itemDBType, uint32 datalength, uint32 flags):
	  EntityTableItem(itemDBType, datalength, flags)
	{
		memset(db_item_name_, 0, MAX_BUF);
	};

	virtual ~EntityTableItemRedisBase()
	{
	};

	uint8 type() const{ return TABLE_ITEM_TYPE_UNKONWN; }

	/**
		initialization
	*/
	virtual bool initialize(const PropertyDescription* pPropertyDescription,
		const DataType* pDataType, std::string name);

	/**
		Synchronize the entity table into the database
	*/
	virtual bool syncToDB(DBInterface* pdbi, void* pData = NULL) = 0;

	/**
		update data
	*/
	virtual bool writeItem(DBInterface* pdbi, DBID dbid, MemoryStream* s, ScriptDefModule* pModule){ return true; }

	/**
		Query table
	*/
	virtual bool queryTable(DBInterface* pdbi, DBID dbid, MemoryStream* s, ScriptDefModule* pModule){ return true; }

	/**
		Get all the data of a table into the stream
	*/
	virtual void addToStream(MemoryStream* s, redis::DBContext& context, DBID resultDBID){};

	/**
		Get the name of the table that needs to be stored, the field name, and the string value converted to sql when stored
	*/
	virtual void getWriteSqlItem(DBInterface* pdbi, MemoryStream* s, redis::DBContext& context) = 0;
	virtual void getReadSqlItem(redis::DBContext& context) = 0;

	virtual void init_db_item_name(const char* exstrFlag = "");
	const char* db_item_name(){ return db_item_name_; }

	virtual bool isSameKey(std::string key){ return key == db_item_name(); }
protected:
	char db_item_name_[MAX_BUF];
};

class EntityTableItemRedis_DIGIT : public EntityTableItemRedisBase
{
public:
	EntityTableItemRedis_DIGIT(std::string dataSType, std::string itemDBType,
		uint32 datalength, uint32 flags):
	  EntityTableItemRedisBase(itemDBType, datalength, flags),
		  dataSType_(dataSType)
	{
	};

	virtual ~EntityTableItemRedis_DIGIT(){};

	uint8 type() const{ return TABLE_ITEM_TYPE_DIGIT; }

	/**
		Synchronize the entity table into the database
	*/
	virtual bool syncToDB(DBInterface* pdbi, void* pData = NULL);

	/**
		Get all the data of a table into the stream
	*/
	void addToStream(MemoryStream* s, redis::DBContext& context, DBID resultDBID);

	/**
		Get the name of the table that needs to be stored, the field name, and the string value converted to sql when stored
	*/
	virtual void getWriteSqlItem(DBInterface* pdbi, MemoryStream* s, redis::DBContext& context);
	virtual void getReadSqlItem(redis::DBContext& context);
protected:
	std::string dataSType_;
};

class EntityTableItemRedis_STRING : public EntityTableItemRedisBase
{
public:
	EntityTableItemRedis_STRING(std::string itemDBType,
		uint32 datalength, uint32 flags):
	  EntityTableItemRedisBase(itemDBType, datalength, flags)
	  {
	  }

	virtual ~EntityTableItemRedis_STRING(){};

	uint8 type() const{ return TABLE_ITEM_TYPE_STRING; }

	/**
		Synchronize the entity table into the database
	*/
	virtual bool syncToDB(DBInterface* pdbi, void* pData = NULL);

	/**
		Get all the data of a table into the stream
	*/
	void addToStream(MemoryStream* s, redis::DBContext& context, DBID resultDBID);

	/**
		Get the name of the table that needs to be stored, the field name, and the string value converted to sql when stored
	*/
	virtual void getWriteSqlItem(DBInterface* pdbi, MemoryStream* s, redis::DBContext& context);
	virtual void getReadSqlItem(redis::DBContext& context);
};

class EntityTableItemRedis_UNICODE : public EntityTableItemRedisBase
{
public:
	EntityTableItemRedis_UNICODE(std::string itemDBType,
		uint32 datalength, uint32 flags):
	  EntityTableItemRedisBase(itemDBType, datalength, flags)
	  {
	  }

	virtual ~EntityTableItemRedis_UNICODE(){};

	uint8 type() const{ return TABLE_ITEM_TYPE_UNICODE; }

	/**
		Synchronize the entity table into the database
	*/
	virtual bool syncToDB(DBInterface* pdbi, void* pData = NULL);

	/**
		Get all the data of a table into the stream
	*/
	void addToStream(MemoryStream* s, redis::DBContext& context, DBID resultDBID);

	/**
		Get the name of the table that needs to be stored, the field name, and the string value converted to sql when stored
	*/
	virtual void getWriteSqlItem(DBInterface* pdbi, MemoryStream* s, redis::DBContext& context);
	virtual void getReadSqlItem(redis::DBContext& context);
};

class EntityTableItemRedis_PYTHON : public EntityTableItemRedisBase
{
public:
	EntityTableItemRedis_PYTHON(std::string itemDBType,
		uint32 datalength, uint32 flags):
	  EntityTableItemRedisBase(itemDBType, datalength, flags)
	  {
	  }

	virtual ~EntityTableItemRedis_PYTHON(){};

	uint8 type() const{ return TABLE_ITEM_TYPE_PYTHON; }

	/**
		Synchronize the entity table into the database
	*/
	virtual bool syncToDB(DBInterface* pdbi, void* pData = NULL);

	/**
		Get all the data of a table into the stream
	*/
	void addToStream(MemoryStream* s, redis::DBContext& context, DBID resultDBID);

	/**
		Get the name of the table that needs to be stored, the field name, and the string value converted to sql when stored
	*/
	virtual void getWriteSqlItem(DBInterface* pdbi, MemoryStream* s, redis::DBContext& context);
	virtual void getReadSqlItem(redis::DBContext& context);
};

class EntityTableItemRedis_BLOB : public EntityTableItemRedisBase
{
public:
	EntityTableItemRedis_BLOB(std::string itemDBType,
		uint32 datalength, uint32 flags):
	  EntityTableItemRedisBase(itemDBType, datalength, flags)
	  {
	  }

	virtual ~EntityTableItemRedis_BLOB(){};

	uint8 type() const{ return TABLE_ITEM_TYPE_BLOB; }

	/**
		Synchronize the entity table into the database
	*/
	virtual bool syncToDB(DBInterface* pdbi, void* pData = NULL);

	/**
		Get all the data of a table into the stream
	*/
	void addToStream(MemoryStream* s, redis::DBContext& context, DBID resultDBID);

	/**
		Get the name of the table that needs to be stored, the field name, and the string value converted to sql when stored
	*/
	virtual void getWriteSqlItem(DBInterface* pdbi, MemoryStream* s, redis::DBContext& context);
	virtual void getReadSqlItem(redis::DBContext& context);
};

class EntityTableItemRedis_VECTOR2 : public EntityTableItemRedisBase
{
public:
	EntityTableItemRedis_VECTOR2(std::string itemDBType,
		uint32 datalength, uint32 flags):
	  EntityTableItemRedisBase(itemDBType, datalength, flags)
	  {
	  }

	virtual ~EntityTableItemRedis_VECTOR2(){};

	uint8 type() const{ return TABLE_ITEM_TYPE_VECTOR2; }

	virtual bool isSameKey(std::string key);

	/**
		Synchronize the entity table into the database
	*/
	virtual bool syncToDB(DBInterface* pdbi, void* pData = NULL);

	/**
		Get all the data of a table into the stream
	*/
	void addToStream(MemoryStream* s, redis::DBContext& context, DBID resultDBID);

	/**
		Get the name of the table that needs to be stored, the field name, and the string value converted to sql when stored
	*/
	virtual void getWriteSqlItem(DBInterface* pdbi, MemoryStream* s, redis::DBContext& context);
	virtual void getReadSqlItem(redis::DBContext& context);

	virtual void init_db_item_name(const char* exstrFlag = "")
	{
		for(int i=0; i<2; ++i)
			ouro_snprintf(db_item_names_[i], MAX_BUF, TABLE_ITEM_PERFIX"_%d_%s%s", i, exstrFlag, itemName());
	}

protected:
	char db_item_names_[2][MAX_BUF];
};

class EntityTableItemRedis_VECTOR3 : public EntityTableItemRedisBase
{
public:
	EntityTableItemRedis_VECTOR3(std::string itemDBType,
		uint32 datalength, uint32 flags):
	  EntityTableItemRedisBase(itemDBType, datalength, flags)
	  {
	  }

	virtual ~EntityTableItemRedis_VECTOR3(){};

	uint8 type() const{ return TABLE_ITEM_TYPE_VECTOR3; }

	virtual bool isSameKey(std::string key);

	/**
		Synchronize the entity table into the database
	*/
	virtual bool syncToDB(DBInterface* pdbi, void* pData = NULL);

	/**
		Get all the data of a table into the stream
	*/
	void addToStream(MemoryStream* s, redis::DBContext& context, DBID resultDBID);

	/**
		Get the name of the table that needs to be stored, the field name, and the string value converted to sql when stored
	*/
	virtual void getWriteSqlItem(DBInterface* pdbi, MemoryStream* s, redis::DBContext& context);
	virtual void getReadSqlItem(redis::DBContext& context);

	virtual void init_db_item_name(const char* exstrFlag = "")
	{
		for(int i=0; i<3; ++i)
			ouro_snprintf(db_item_names_[i], MAX_BUF, TABLE_ITEM_PERFIX"_%d_%s%s", i, exstrFlag, itemName());
	}

protected:
	char db_item_names_[3][MAX_BUF];
};

class EntityTableItemRedis_VECTOR4 : public EntityTableItemRedisBase
{
public:
	EntityTableItemRedis_VECTOR4(std::string itemDBType,
		uint32 datalength, uint32 flags):
	  EntityTableItemRedisBase(itemDBType, datalength, flags)
	  {
	  }

	virtual ~EntityTableItemRedis_VECTOR4(){};

	uint8 type() const{ return TABLE_ITEM_TYPE_VECTOR4; }

	virtual bool isSameKey(std::string key);

	/**
		Synchronize the entity table into the database
	*/
	virtual bool syncToDB(DBInterface* pdbi, void* pData = NULL);

	/**
		Get all the data of a table into the stream
	*/
	void addToStream(MemoryStream* s, redis::DBContext& context, DBID resultDBID);

	/**
		Get the name of the table that needs to be stored, the field name, and the string value converted to sql when stored
	*/
	virtual void getWriteSqlItem(DBInterface* pdbi, MemoryStream* s, redis::DBContext& context);
	virtual void getReadSqlItem(redis::DBContext& context);

	virtual void init_db_item_name(const char* exstrFlag = "")
	{
		for(int i=0; i<4; ++i)
			ouro_snprintf(db_item_names_[i], MAX_BUF, TABLE_ITEM_PERFIX"_%d_%s%s", i, exstrFlag, itemName());
	}

protected:
	char db_item_names_[4][MAX_BUF];
};

class EntityTableItemRedis_ENTITYCALL : public EntityTableItemRedisBase
{
public:
	EntityTableItemRedis_ENTITYCALL(std::string itemDBType,
		uint32 datalength, uint32 flags):
	  EntityTableItemRedisBase(itemDBType, datalength, flags)
	  {
	  }

	virtual ~EntityTableItemRedis_ENTITYCALL(){};

	uint8 type() const{ return TABLE_ITEM_TYPE_ENTITYCALL; }

	/**
		Synchronize the entity table into the database
	*/
	virtual bool syncToDB(DBInterface* pdbi, void* pData = NULL);

	/**
		Get all the data of a table into the stream
	*/
	void addToStream(MemoryStream* s, redis::DBContext& context, DBID resultDBID);

	/**
		Get the name of the table that needs to be stored, the field name, and the string value converted to sql when stored
	*/
	virtual void getWriteSqlItem(DBInterface* pdbi, MemoryStream* s, redis::DBContext& context);
	virtual void getReadSqlItem(redis::DBContext& context);
};

class EntityTableItemRedis_ARRAY : public EntityTableItemRedisBase
{
public:
	EntityTableItemRedis_ARRAY(std::string itemDBType,
		uint32 datalength, uint32 flags):
	  EntityTableItemRedisBase(itemDBType, datalength, flags),
	  pChildTable_(NULL)
	  {
	  }

	virtual ~EntityTableItemRedis_ARRAY(){};

	virtual bool isSameKey(std::string key);

	/**
		initialization
	*/
	virtual bool initialize(const PropertyDescription* pPropertyDescription,
		const DataType* pDataType, std::string name);

	uint8 type() const{ return TABLE_ITEM_TYPE_FIXEDARRAY; }

	/**
		Synchronize the entity table into the database
	*/
	virtual bool syncToDB(DBInterface* pdbi, void* pData = NULL);

	/**
		Get all the data of a table into the stream
	*/
	void addToStream(MemoryStream* s, redis::DBContext& context, DBID resultDBID);

	/**
		Get the name of the table that needs to be stored, the field name, and the string value converted to sql when stored
	*/
	virtual void getWriteSqlItem(DBInterface* pdbi, MemoryStream* s, redis::DBContext& context);
	virtual void getReadSqlItem(redis::DBContext& context);

	virtual void init_db_item_name(const char* exstrFlag = "");

protected:
	EntityTable* pChildTable_;
};

class EntityTableItemRedis_FIXED_DICT : public EntityTableItemRedisBase
{
public:
	EntityTableItemRedis_FIXED_DICT(std::string itemDBType,
		uint32 datalength, uint32 flags):
	  EntityTableItemRedisBase(itemDBType, datalength, flags)
	  {
	  }

	virtual ~EntityTableItemRedis_FIXED_DICT(){};

	typedef std::vector< std::pair< std::string, OUROShared_ptr<EntityTableItem> > > FIXEDDICT_KEYTYPES;

	uint8 type() const{ return TABLE_ITEM_TYPE_FIXEDDICT; }

	virtual bool isSameKey(std::string key);

	/**
		initialization
	*/
	virtual bool initialize(const PropertyDescription* pPropertyDescription,
		const DataType* pDataType, std::string name);

	/**
		Synchronize the entity table into the database
	*/
	virtual bool syncToDB(DBInterface* pdbi, void* pData = NULL);

	/**
		Get all the data of a table into the stream
	*/
	void addToStream(MemoryStream* s, redis::DBContext& context, DBID resultDBID);

	/**
		Get the name of the table that needs to be stored, the field name, and the string value converted to sql when stored
	*/
	virtual void getWriteSqlItem(DBInterface* pdbi, MemoryStream* s, redis::DBContext& context);
	virtual void getReadSqlItem(redis::DBContext& context);

	virtual void init_db_item_name(const char* exstrFlag = "");

protected:
	EntityTableItemRedis_FIXED_DICT::FIXEDDICT_KEYTYPES			keyTypes_;		// The type of each key in this fixed dictionary
};


/*
	Maintain the table of the entity in the database
*/
class EntityTableRedis : public EntityTable
{
public:
	EntityTableRedis(EntityTables* pEntityTables);
	virtual ~EntityTableRedis();

	/**
		initialization
	*/
	virtual bool initialize(ScriptDefModule* sm, std::string name);

	/**
		Synchronize the entity table into the database
	*/
	virtual bool syncToDB(DBInterface* pdbi);

	/**
		Synchronization table index
	*/
	virtual bool syncIndexToDB(DBInterface* pdbi);

	/**
		Create a table item
	*/
	virtual EntityTableItem* createItem(std::string type, std::string defaultVal);

	DBID writeTable(DBInterface* pdbi, DBID dbid, int8 shouldAutoLoad, MemoryStream* s, ScriptDefModule* pModule);

	/**
		Delete entity from database
	*/
	bool removeEntity(DBInterface* pdbi, DBID dbid, ScriptDefModule* pModule);

	/**
		Get all the data into the stream
	*/
	virtual bool queryTable(DBInterface* pdbi, DBID dbid, MemoryStream* s, ScriptDefModule* pModule);

	/**
		Set whether to automatically load
	*/
	virtual void entityShouldAutoLoad(DBInterface* pdbi, DBID dbid, bool shouldAutoLoad);

	/**
		Query the automatically loaded entity
	*/
	virtual void queryAutoLoadEntities(DBInterface* pdbi, ScriptDefModule* pModule,
		ENTITY_ID start, ENTITY_ID end, std::vector<DBID>& outs);

	/**
		Get all the data of a table into the stream
	*/
	void addToStream(MemoryStream* s, redis::DBContext& context, DBID resultDBID);

	/**
		Get the name of the table that needs to be stored, the field name, and the string value converted to sql when stored
	*/
	virtual void getWriteSqlItem(DBInterface* pdbi, MemoryStream* s, redis::DBContext& context);
	virtual void getReadSqlItem(redis::DBContext& context);

	void init_db_item_name();

protected:

};


}

#ifdef CODE_INLINE
#include "entity_table_redis.inl"
#endif
#endif // OURO_ENTITY_TABLE_REDIS_H
