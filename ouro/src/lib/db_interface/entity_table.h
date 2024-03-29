// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_ENTITY_TABLE_H
#define OURO_ENTITY_TABLE_H

#include "common/common.h"
#include "common/singleton.h"
#include "helper/debug_helper.h"
#include "entitydef/common.h"
#include "thread/threadmutex.h"

namespace Ouroboros { 

class DBUtil;
class DBInterface;
class ScriptDefModule;
class DataType;
class PropertyDescription;
class EntityTable;
class EntityTables;
class MemoryStream;

#define TABLE_ITEM_TYPE_UNKONWN		0
#define TABLE_ITEM_TYPE_FIXEDARRAY	1
#define TABLE_ITEM_TYPE_FIXEDDICT	2
#define TABLE_ITEM_TYPE_STRING		3
#define TABLE_ITEM_TYPE_DIGIT		4
#define TABLE_ITEM_TYPE_BLOB		5
#define TABLE_ITEM_TYPE_VECTOR2		6
#define TABLE_ITEM_TYPE_VECTOR3		7
#define TABLE_ITEM_TYPE_VECTOR4		8
#define TABLE_ITEM_TYPE_UNICODE		9
#define TABLE_ITEM_TYPE_ENTITYCALL	10
#define TABLE_ITEM_TYPE_PYTHON		11
#define TABLE_ITEM_TYPE_COMPONENT	12

#define OURO_TABLE_PERFIX						"ouro"
#define ENTITY_TABLE_PERFIX						"tbl"
#define TABLE_ID_CONST_STR						"id"
#define TABLE_PARENTID_CONST_STR				"parentID"
#define TABLE_ITEM_PERFIX						"sm"
#define TABLE_ARRAY_ITEM_VALUE_CONST_STR		"value"
#define TABLE_ARRAY_ITEM_VALUES_CONST_STR		"values"
#define TABLE_AUTOLOAD_CONST_STR				"autoLoad"

/**
	Db table operation
*/
enum DB_TABLE_OP
{
	TABLE_OP_INSERT					= 1,
	TABLE_OP_UPDATE					= 2,
	TABLE_OP_DELETE					= 3
};

struct ACCOUNT_INFOS
{
	ACCOUNT_INFOS():
	name(),
	password(),
	datas(),
	dbid(0),
	flags(0),
	deadline(0)
	{
	}

	std::string name, password, datas, email;
	DBID dbid;

	uint32 flags;
	uint64 deadline;
};

/**
	Maintain a field in the table of the entity in the database
*/
class EntityTableItem
{
public:
	EntityTableItem(std::string itemDBType, uint32 datalength, uint32 flags):
		itemName_(),
		tableName_(),
		utype_(0),
		pParentTable_(NULL),
		pParentTableItem_(NULL),
		pDataType_(NULL),
		pPropertyDescription_(NULL),
		itemDBType_(itemDBType),
		datalength_(datalength),
		flags_(flags),
		indexType_()
	{
	};

	virtual ~EntityTableItem() {};

	virtual bool isSameKey(std::string key) { return itemName() == key; }

	virtual uint8 type() const { return TABLE_ITEM_TYPE_UNKONWN; }

	void itemName(std::string name) { itemName_ = name; }
	const char* itemName() { return itemName_.c_str(); }

	void indexType(std::string index) { indexType_ = index; }
	const char* indexType() { return indexType_.c_str(); }
	
	const char* itemDBType() { return itemDBType_.c_str(); }

	void utype(int32/*ENTITY_PROPERTY_UID*/ utype) { utype_ = utype; }
	int32 utype() const { return utype_; }

	void flags(uint32 f) { flags_ = f; }
	uint32 flags() const { return flags_; }

	void pParentTable(EntityTable* v) { pParentTable_ = v; }
	EntityTable* pParentTable() { return pParentTable_; }

	void pParentTableItem(EntityTableItem* v) { pParentTableItem_ = v; }
	EntityTableItem* pParentTableItem() { return pParentTableItem_; }

	const DataType* pDataType() { return pDataType_; }

	uint32 datalength() const { return datalength_; }

	const PropertyDescription* pPropertyDescription() const { return pPropertyDescription_; }

	/**
		Initialize
	*/
	virtual bool initialize(const PropertyDescription* pPropertyDescription, 
		const DataType* pDataType, std::string itemName) = 0;

	void tableName(std::string name) { tableName_ = name; }
	const char* tableName() { return tableName_.c_str(); }

	/**
		Synchronize the entity table into the database
	*/
	virtual bool syncToDB(DBInterface* pdbi, void* pData = NULL) = 0;

	/**
				update data
	*/
	virtual bool writeItem(DBInterface* pdbi, DBID dbid, MemoryStream* s, ScriptDefModule* pModule) = 0;

	/**
		Get all the data into the stream
	*/
	virtual bool queryTable(DBInterface* pdbi, DBID dbid, MemoryStream* s, ScriptDefModule* pModule) = 0;

protected:
		// Field Name
	std::string itemName_;
	std::string tableName_;
	int32/*ENTITY_PROPERTY_UID*/ utype_;

	EntityTable* pParentTable_;
	EntityTableItem* pParentTableItem_;

	const DataType* pDataType_;
	const PropertyDescription* pPropertyDescription_;

	std::string itemDBType_;
	uint32 datalength_;
	uint32 flags_;

	std::string indexType_;
};

/*
	Maintain the table of the entity in the database
*/
class EntityTable
{
public:
	typedef std::map<int32/*ENTITY_PROPERTY_UID*/, OUROShared_ptr<EntityTableItem> > TABLEITEM_MAP;

	EntityTable(EntityTables* pEntityTables) :
	tableName_(),
	tableItems_(),
	tableFixedOrderItems_(),
	isChild_(false),
	sync_(false),
	pEntityTables_(pEntityTables)
	{
	};

	virtual ~EntityTable(){};
	
	void tableName(std::string name){ tableName_ = name; }
	const char* tableName(){ return tableName_.c_str(); }

	/**
		Initialize
	*/
	virtual bool initialize(ScriptDefModule* sm, std::string name) = 0;

	/**
		Synchronize the entity table into the database
	*/
	virtual bool syncToDB(DBInterface* pdbi) = 0;

	/**
		Synchronize the entity table index into the database
	*/
	virtual bool syncIndexToDB(DBInterface* pdbi) = 0;

	/** 
		Create a table item
	*/
	virtual EntityTableItem* createItem(std::string type, std::string defaultVal) = 0;

	/** 
		Get all table fields
	*/
	const EntityTable::TABLEITEM_MAP& tableItems() const { return tableItems_; }
	const std::vector<EntityTableItem*>& tableFixedOrderItems() const{ return tableFixedOrderItems_; }

	void addItem(EntityTableItem* pItem);

	bool isChild() const{ return isChild_; }
	void isChild(bool b){ isChild_ = b; }

	EntityTableItem* findItem(int32/*ENTITY_PROPERTY_UID*/ utype);

	/**
		Update table
	*/
	virtual DBID writeTable(DBInterface* pdbi, DBID dbid, int8 shouldAutoLoad, MemoryStream* s, ScriptDefModule* pModule);

	/**
		Remove entity from the database
	*/
	virtual bool removeEntity(DBInterface* pdbi, DBID dbid, ScriptDefModule* pModule);

	/**
		Get all the data into the stream
	*/
	virtual bool queryTable(DBInterface* pdbi, DBID dbid, MemoryStream* s, ScriptDefModule* pModule);

	/**
		Set whether to load automatically
	*/
	virtual void entityShouldAutoLoad(DBInterface* pdbi, DBID dbid, bool shouldAutoLoad){};

	bool hasSync() const { return sync_; }

	/**
		Query automatically loaded entities
	*/
	virtual void queryAutoLoadEntities(DBInterface* pdbi, ScriptDefModule* pModule, 
		ENTITY_ID start, ENTITY_ID end, std::vector<DBID>& outs){}

	EntityTables* pEntityTables() const { return pEntityTables_; }
	void pEntityTables(EntityTables* v){ pEntityTables_ = v; }

protected:

	// table name
	std::string tableName_;

	// all fields
	TABLEITEM_MAP tableItems_;

	// an item reference that is consistent with the ScriptDefModule
	std::vector<EntityTableItem*> tableFixedOrderItems_; 

	// Is it a child table?
	bool isChild_; 

	bool sync_;

	EntityTables* pEntityTables_;
};

class EntityTables
{
public:
	struct case_insensitive_hasher
	{
		size_t operator()(const std::string& key) const
		{
			std::string keyCopy(key);
			std::transform(keyCopy.begin(), keyCopy.end(), keyCopy.begin(), tolower);
			return std::tr1::hash<std::string>()(keyCopy);
		}
	};

	struct case_insensitive_comparer
	{
		bool operator() (const std::string& x, const std::string& y) const
		{
			return x.size() == y.size() && ouro_stricmp(x.c_str(), y.c_str()) == 0;
		}
	};

	typedef OUROUnordered_map<std::string, EntityTables> ENTITY_TABLES_MAP;
	static ENTITY_TABLES_MAP sEntityTables;
	static EntityTables& findByInterfaceName(const std::string& dbInterfaceName);

	typedef OUROUnordered_map<std::string, OUROShared_ptr<EntityTable>, case_insensitive_hasher, case_insensitive_comparer> TABLES_MAP;

	EntityTables();
	virtual ~EntityTables();
	
	bool load(DBInterface* pdbi);
	bool syncToDB(DBInterface* pdbi);

	void dbInterfaceName(const std::string& dbInterfaceName){
		dbInterfaceName_ = dbInterfaceName;
	}

	/** 
		Get all the tables
	*/
	const EntityTables::TABLES_MAP& tables() const { return tables_; }

	void addTable(EntityTable* pTable);

	EntityTable* findTable(std::string name);

	void addOUROTable(EntityTable* pTable);

	EntityTable* findOUROTable(std::string name);

	/**
		Write entity to the database
	*/
	DBID writeEntity(DBInterface* pdbi, DBID dbid, int8 shouldAutoLoad, MemoryStream* s, ScriptDefModule* pModule);

	/**
		Remove entity from the database
	*/
	bool removeEntity(DBInterface* pdbi, DBID dbid, ScriptDefModule* pModule);

	/**
		Get all the data of a table into the stream
	*/
	bool queryEntity(DBInterface* pdbi, DBID dbid, MemoryStream* s, ScriptDefModule* pModule);

	void onTableSyncSuccessfully(OUROShared_ptr<EntityTable> pEntityTable, bool error);

	/**
		Query automatically loaded entities
	*/
	void queryAutoLoadEntities(DBInterface* pdbi, ScriptDefModule* pModule, 
		ENTITY_ID start, ENTITY_ID end, std::vector<DBID>& outs);

protected:
	// all tables
	TABLES_MAP tables_;
	TABLES_MAP ouro_tables_;

	int numSyncTables_;
	bool syncTablesError_;

	std::string dbInterfaceName_;
};

}

#endif // OURO_ENTITY_TABLE_H
