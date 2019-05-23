// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_READ_ENTITY_HELPER_H
#define OURO_READ_ENTITY_HELPER_H

// common include	
// #define NDEBUG
#include <sstream>
#include "common.h"
#include "sqlstatement.h"
#include "entity_sqlstatement_mapping.h"
#include "common/common.h"
#include "common/memorystream.h"
#include "helper/debug_helper.h"
#include "db_interface/db_interface.h"
#include "db_interface/entity_table.h"
#include "db_interface_mysql.h"

namespace Ouroboros{ 

class ReadEntityHelper
{
public:
	ReadEntityHelper()
	{
	}

	virtual ~ReadEntityHelper()
	{
	}

	/**
		Query data from a table
	*/
	static bool queryDB(DBInterface* pdbi, mysql::DBContext& context)
	{
		// Get the relevant data on a table according to a dbid
		SqlStatement* pSqlcmd = new SqlStatementQuery(pdbi, context.tableName, 
			context.dbids[context.dbid], 
			context.dbid, context.items);

		bool ret = pSqlcmd->query();
		context.dbid = pSqlcmd->dbid();
		delete pSqlcmd;
		
		if(!ret)
			return ret;

		// Write the result of the query to the context
		MYSQL_RES * pResult = mysql_store_result(static_cast<DBInterfaceMysql*>(pdbi)->mysql());

		if(pResult)
		{
			MYSQL_ROW arow;

			while((arow = mysql_fetch_row(pResult)) != NULL)
			{
				uint32 nfields = (uint32)mysql_num_fields(pResult);
				if(nfields <= 0)
					continue;

				unsigned long *lengths = mysql_fetch_lengths(pResult);

				// The query command guarantees that every record queried will have a dbid
				std::stringstream sval;
				sval << arow[0];

				DBID item_dbid;
				sval >> item_dbid;

				// Record the dbid into the list. If there is still a subtable reference in the current table, go to the subtable to check each record related to this dbid.
				std::vector<DBID>& itemDBIDs = context.dbids[context.dbid];
				int fidx = -100;

				// If the current dbid of this item is smaller than the dbid size of the last record in the table, you need to insert this dbid in the position specified in itemDBIDs to ensure the order from small to large.
				if (itemDBIDs.size() > 0 && itemDBIDs[itemDBIDs.size() - 1] > item_dbid)
				{
					for (fidx = itemDBIDs.size() - 1; fidx > 0; --fidx)
					{
						if (itemDBIDs[fidx] < item_dbid)
							break;
					}

					itemDBIDs.insert(itemDBIDs.begin() + fidx, item_dbid);
				}
				else
				{
					itemDBIDs.push_back(item_dbid);
				}

				// If this record has other data besides dbid, then populate the data into the result set
				if(nfields > 1)
				{
					std::vector<std::string>& itemResults = context.results[item_dbid].second;
					context.results[item_dbid].first = 0;

					OURO_ASSERT(nfields == context.items.size() + 1);

					for (uint32 i = 1; i < nfields; ++i)
					{
						OUROShared_ptr<mysql::DBContext::DB_ITEM_DATA> pSotvs = context.items[i - 1];
						std::string data;
						data.assign(arow[i], lengths[i]);

						// If the above calculation is to insert the dbid, the result set also needs to be inserted into the corresponding position.
						if (fidx != -100)
							itemResults.insert(itemResults.begin() + fidx++, data);
						else
							itemResults.push_back(data);
					}
				}
			}

			mysql_free_result(pResult);
		}
		
		std::vector<DBID>& dbids = context.dbids[context.dbid];

		// If there is no data, the query is complete.
		if(dbids.size() == 0)
			return true;

		// If the current table has a child table reference, you need to continue to query the child table
		// Every dbid needs to get the data on the child table
		// Here we let the child table query all the dbids data and populate it into the result set.

		mysql::DBContext::DB_RW_CONTEXTS::iterator iter1 = context.optable.begin();
		for(; iter1 != context.optable.end(); ++iter1)
		{
			mysql::DBContext& wbox = *iter1->second.get();
			if(!queryChildDB(pdbi, wbox, dbids))
				return false;
		}

		return ret;
	}


	/**
		Query data from child tables
	*/
	static bool queryChildDB(DBInterface* pdbi, mysql::DBContext& context, std::vector<DBID>& parentTableDBIDs)
	{
		// Get the relevant data on a table according to a dbid
		SqlStatement* pSqlcmd = new SqlStatementQuery(pdbi, context.tableName, 
			parentTableDBIDs, 
			context.dbid, context.items);

		bool ret = pSqlcmd->query();
		context.dbid = pSqlcmd->dbid();
		delete pSqlcmd;
		
		if(!ret)
			return ret;

		std::vector<DBID> t_parentTableDBIDs;

		// Write the result of the query to the context
		MYSQL_RES * pResult = mysql_store_result(static_cast<DBInterfaceMysql*>(pdbi)->mysql());

		if(pResult)
		{
			MYSQL_ROW arow;

			while((arow = mysql_fetch_row(pResult)) != NULL)
			{
				uint32 nfields = (uint32)mysql_num_fields(pResult);
				if(nfields <= 0)
					continue;

				unsigned long *lengths = mysql_fetch_lengths(pResult);

				// The query command guarantees that every record queried will have a dbid
				std::stringstream sval;
				sval << arow[0];

				DBID item_dbid;
				sval >> item_dbid;

				sval.clear();
				sval << arow[1];

				DBID parentID;
				sval >> parentID;

				// Record the dbid into the list. If there is still a subtable reference in the current table, go to the subtable to check each record related to this dbid.
				std::vector<DBID>& itemDBIDs = context.dbids[parentID];
				int fidx = -100;

				// If the current dbid of this item is smaller than the dbid size of the last record in the table, you need to insert this dbid in the position specified in itemDBIDs to ensure the order from small to large.
				if (itemDBIDs.size() > 0 && itemDBIDs[itemDBIDs.size() - 1] > item_dbid)
				{
					for (fidx = itemDBIDs.size() - 1; fidx > 0; --fidx)
					{
						if (itemDBIDs[fidx] < item_dbid)
							break;
					}

					itemDBIDs.insert(itemDBIDs.begin() + fidx, item_dbid);
					t_parentTableDBIDs.insert(t_parentTableDBIDs.begin() + t_parentTableDBIDs.size() - (itemDBIDs.size() - fidx - 1), item_dbid);
				}
				else
				{
					itemDBIDs.push_back(item_dbid);
					t_parentTableDBIDs.push_back(item_dbid);
				}

				// If this record has other data besides dbid, then populate the data into the result set
				const uint32 const_fields = 2; // id, parentID
				if(nfields > const_fields)
				{
					std::vector<std::string>& itemResults = context.results[item_dbid].second;
					context.results[item_dbid].first = 0;

					OURO_ASSERT(nfields == context.items.size() + const_fields);

					for (uint32 i = const_fields; i < nfields; ++i)
					{
						OUROShared_ptr<mysql::DBContext::DB_ITEM_DATA> pSotvs = context.items[i - const_fields];
						std::string data;
						data.assign(arow[i], lengths[i]);

						// If the dbid of the current item is greater than the dbid size of all recordsets in the table, then the dbid needs to be inserted at the location specified in itemDBIDs to ensure the order from small to large.
						if (fidx != -100)
							itemResults.insert(itemResults.begin() + fidx++, data);
						else
							itemResults.push_back(data);
					}
				}
			}

			mysql_free_result(pResult);
		}

		// If there is no data, the query is complete.
		if(t_parentTableDBIDs.size() == 0)
			return true;

		// If the current table has a child table reference, you need to continue to query the child table
		// Every dbid needs to get the data on the child table
		// Here we let the child table query all the dbids data and populate it into the result set.
		mysql::DBContext::DB_RW_CONTEXTS::iterator iter1 = context.optable.begin();
		for(; iter1 != context.optable.end(); ++iter1)
		{
			mysql::DBContext& wbox = *iter1->second.get();

			if(!queryChildDB(pdbi, wbox, t_parentTableDBIDs))
				return false;
		}

		return ret;
	}

protected:
};

}
#endif // OURO_READ_ENTITY_HELPER_H
