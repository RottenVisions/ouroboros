// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_MYSQL_DB_RW_CONTEXT_H
#define OURO_MYSQL_DB_RW_CONTEXT_H

#include "common/common.h"
#include "common/memorystream.h"
#include "helper/debug_helper.h"

namespace Ouroboros { 
namespace mysql {

/**
	It is used when reading and writing delete operations, and contains various information to be fetched or to be written.

	Dbid: If the primary table is the dbid of the entity, the child table is the dbid of the current query.

	Dbids: There is only one dbid in the dbids on the main table, which is the id of the entity. If the entity data exists in the array class, there will be a child table. When the data structure describes a child table,
		Dbids is the sub-table index of this array. Each dbid represents the corresponding value on this sub-table and also represents the value of the corresponding position in the array in the order of arrangement.
		dbids = {
			123 : [xxx, xxx, ...], // 123 is a dbid on the parent table, and the array is the dbids associated with the parent table on the child table.
			...
		}

	Items: There is field information of this table. If it is a write library, there is also a corresponding value to be written in the field.

	Optable: subtable structure

	Results: The data that is queried during the read operation. The arrangement of the data corresponds to the number of strkeys in the items multiplied by the number of dbids.
	readresultIdx: Because the number in results is dbids * items, when filling data in some recursive reads, the padding position is calculated based on this readresultIdx.

	parentTableDBID: the dbid of the parent table
	parentTableName: the name of the parent table

	tableName: the name of the current table
 */
class DBContext
{
public:
	/**
		Store all the table item structures to be operated
	*/
	struct DB_ITEM_DATA
	{
		DB_ITEM_DATA()
		{
			sqlkey = NULL;
		}

		char sqlval[MAX_BUF];
		const char* sqlkey;
		std::string extraDatas;
	};

	typedef std::vector< std::pair< std::string/*tableName*/, OUROShared_ptr< DBContext > > > DB_RW_CONTEXTS;
	typedef std::vector< OUROShared_ptr<DB_ITEM_DATA>  > DB_ITEM_DATAS;

	DBContext()
	{
	}

	~DBContext()
	{
	}
	
	DB_ITEM_DATAS items;
	
	std::string tableName;
	std::string parentTableName;
	
	DBID parentTableDBID;
	DBID dbid;
	
	DB_RW_CONTEXTS optable;
	
	bool isEmpty;
	
	std::map<DBID, std::vector<DBID> > dbids;
	std::map<DBID, std::pair< std::vector<std::string>::size_type, std::vector<std::string> > > results;

private:

};

}
}
#endif // OURO_MYSQL_DB_RW_CONTEXT_H

