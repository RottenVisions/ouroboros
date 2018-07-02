// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_MYSQL_DB_RW_CONTEXT_H
#define OURO_MYSQL_DB_RW_CONTEXT_H

#include "common/common.h"
#include "common/memorystream.h"
#include "helper/debug_helper.h"

namespace Ouroboros {
namespace mysql {

/**
	It is used when reading, writing or deleting, and it contains various information that is fetched or written.

	Dbid: if the primary table is the dbid of the entity, the child table is the dbid of the current query.

	Dbids: There is only one dbid in the dbids on the main table, which is the id of the entity.
	If there is an array class in the entity data, there will be a sub-table. When this data structure describes a sub-table,
		Dbids is the subtable index of this array, Each dbid represents the corresponding value on this sub-table and also represents the value of the corresponding position in the array, in the same order.
		dbids = {
			123 : [xxx, xxx, ...], // 123 is a dbid on the parent table, and the array is the dbids associated with the parent table on the child table.
			...
		}

	Items: The field information of this table, if it is to write the library, there is also a corresponding value to be written in the field.

	Optable: child table structure

	Results: The data to be queried during the read operation. The arrangement of the data corresponds to the number of strkeys in the item multiplied by the number of dbids.
	readresultIdx: Because the number in results is dbids * items, when filling in some recursive reads, the fill position will be calculated based on this readresultIdx.

	parentTableDBID: dbid of the parent table
	parentTableName: the name of the parent table

	tableName: the name of the current table
 */
class DBContext
{
public:
	/**
		Store all the table item structures to operate
	*/
	struct DB_ITEM_DATA
	{
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
