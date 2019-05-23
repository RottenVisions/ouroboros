// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_WRITE_ENTITY_HELPER_H
#define OURO_WRITE_ENTITY_HELPER_H

// common include	
// #define NDEBUG
#include <sstream>
#include "common.h"
#include "sqlstatement.h"
#include "common/common.h"
#include "common/memorystream.h"
#include "helper/debug_helper.h"
#include "db_interface/db_interface.h"
#include "db_interface/entity_table.h"
#include "db_interface_mysql.h"

namespace Ouroboros{ 

class WriteEntityHelper
{
public:
	WriteEntityHelper()
	{
	}

	virtual ~WriteEntityHelper()
	{
	}

	static SqlStatement* createSql(DBInterface* pdbi, DB_TABLE_OP opType, 
		std::string tableName, DBID parentDBID, 
		DBID dbid, mysql::DBContext::DB_ITEM_DATAS& tableVal)
	{
		SqlStatement* pSqlcmd = NULL;

		switch(opType)
		{
		case TABLE_OP_UPDATE:
			if(dbid > 0)
				pSqlcmd = new SqlStatementUpdate(pdbi, tableName, parentDBID, dbid, tableVal);
			else
				pSqlcmd = new SqlStatementInsert(pdbi, tableName, parentDBID, dbid, tableVal);
			break;
		case TABLE_OP_INSERT:
			pSqlcmd = new SqlStatementInsert(pdbi, tableName, parentDBID, dbid, tableVal);
			break;
		case TABLE_OP_DELETE:
			break;
		default:
			OURO_ASSERT(false && "no support!\n");
		};

		return pSqlcmd;
	}

	/**
		Update data to the table
	*/
	static bool writeDB(DB_TABLE_OP optype, DBInterface* pdbi, mysql::DBContext& context)
	{
		bool ret = true;

		if(!context.isEmpty)
		{
			SqlStatement* pSqlcmd = createSql(pdbi, optype, context.tableName, 
				context.parentTableDBID, 
				context.dbid, context.items);

			ret = pSqlcmd->query();
			context.dbid = pSqlcmd->dbid();
			delete pSqlcmd;
		}

		if(optype == TABLE_OP_INSERT)
		{
			// Start updating all child tables
			mysql::DBContext::DB_RW_CONTEXTS::iterator iter1 = context.optable.begin();
			for(; iter1 != context.optable.end(); ++iter1)
			{
				mysql::DBContext& wbox = *iter1->second.get();
				
				// Bind table relationship
				wbox.parentTableDBID = context.dbid;

				// update the child table
				writeDB(optype, pdbi, wbox);
			}
		}
		else
		{
			// If there is a parent ID, first get the number of entries in the property database with the same parent id, and take the id of each data.
			// Then update the data in memory to the database sequentially, if there are existing entries in the database, the order will overwrite the existing entries, if the number of data
			// Insert the remaining data if it is larger than the existing one in the database. If the data is less than the entry in the database, delete the entry in the database.
			// select id from tbl_SpawnPoint_xxx_values where parentID = 7;
			KBEUnordered_map< std::string, std::vector<DBID> > childTableDBIDs;

			if(context.dbid > 0)
			{
				mysql::DBContext::DB_RW_CONTEXTS::iterator iter1 = context.optable.begin();
				for(; iter1 != context.optable.end(); ++iter1)
				{
					mysql::DBContext& wbox = *iter1->second.get();

					KBEUnordered_map<std::string, std::vector<DBID> >::iterator iter = 
						childTableDBIDs.find(context.tableName);

					if(iter == childTableDBIDs.end())
					{
						std::vector<DBID> v;
						childTableDBIDs.insert(std::pair< std::string, std::vector<DBID> >(wbox.tableName, v));
					}
				}
				
				if(childTableDBIDs.size() > 1)
				{
					std::string sqlstr_getids;
					KBEUnordered_map< std::string, std::vector<DBID> >::iterator tabiter = childTableDBIDs.begin();
					for(; tabiter != childTableDBIDs.end();)
					{
						char sqlstr[MAX_BUF * 10];
						ouro_snprintf(sqlstr, MAX_BUF * 10, "select count(id) from " ENTITY_TABLE_PERFIX "_%s where " TABLE_PARENTID_CONST_STR "=%" PRDBID " union all ", 
							tabiter->first.c_str(),
							context.dbid);
						
						sqlstr_getids += sqlstr;

						ouro_snprintf(sqlstr, MAX_BUF * 10, "select id from " ENTITY_TABLE_PERFIX "_%s where " TABLE_PARENTID_CONST_STR "=%" PRDBID, 
							tabiter->first.c_str(),
							context.dbid);

						sqlstr_getids += sqlstr;
						if(++tabiter != childTableDBIDs.end())
							sqlstr_getids += " union all ";
					}
					
					if(pdbi->query(sqlstr_getids.c_str(), sqlstr_getids.size(), false))
					{
						MYSQL_RES * pResult = mysql_store_result(static_cast<DBInterfaceMysql*>(pdbi)->mysql());
						if(pResult)
						{
							MYSQL_ROW arow;
							int32 count = 0;
							tabiter = childTableDBIDs.begin();
							bool first = true;

							while((arow = mysql_fetch_row(pResult)) != NULL)
							{
								if(count == 0)
								{
									StringConv::str2value(count, arow[0]);
									if(!first || count <= 0)
										tabiter++;
									continue;
								}

								DBID old_dbid;
								StringConv::str2value(old_dbid, arow[0]);
								tabiter->second.push_back(old_dbid);
								count--;
								first = false;
							}

							mysql_free_result(pResult);
						}
					}
				}
				else if(childTableDBIDs.size() == 1)
				{
					KBEUnordered_map< std::string, std::vector<DBID> >::iterator tabiter = childTableDBIDs.begin();
						char sqlstr[MAX_BUF * 10];
						ouro_snprintf(sqlstr, MAX_BUF * 10, "select id from " ENTITY_TABLE_PERFIX "_%s where " TABLE_PARENTID_CONST_STR "=%" PRDBID, 
							tabiter->first.c_str(),
							context.dbid);

						if(pdbi->query(sqlstr, strlen(sqlstr), false))
						{
							MYSQL_RES * pResult = mysql_store_result(static_cast<DBInterfaceMysql*>(pdbi)->mysql());
							if(pResult)
							{
								MYSQL_ROW arow;
								while((arow = mysql_fetch_row(pResult)) != NULL)
								{
									DBID old_dbid;
									StringConv::str2value(old_dbid, arow[0]);
									tabiter->second.push_back(old_dbid);
								}

								mysql_free_result(pResult);
							}
						}
				}
			}

			// If you want to clear this table, loop through the dreads that have been found N times, so that the child tables in the child table can also be deleted effectively.
			if(!context.isEmpty)
			{
				// Start updating all child tables
				mysql::DBContext::DB_RW_CONTEXTS::iterator iter1 = context.optable.begin();
				for(; iter1 != context.optable.end(); ++iter1)
				{
					mysql::DBContext& wbox = *iter1->second.get();
					
					if(wbox.isEmpty)
						continue;

					// Bind table relationship
					wbox.parentTableDBID = context.dbid;

					KBEUnordered_map<std::string, std::vector<DBID> >::iterator iter = 
						childTableDBIDs.find(wbox.tableName);
					
					if(iter != childTableDBIDs.end())
					{
						if(iter->second.size() > 0)
						{
							wbox.dbid = iter->second.front();
							iter->second.erase(iter->second.begin());
						}

						if(iter->second.size() <= 0)
						{
							childTableDBIDs.erase(wbox.tableName);
						}
					}

					// update the child table
					writeDB(optype, pdbi, wbox);
				}
			}
			
			// delete obsolete data items
			KBEUnordered_map< std::string, std::vector<DBID> >::iterator tabiter = childTableDBIDs.begin();
			for(; tabiter != childTableDBIDs.end(); ++tabiter)
			{
				if(tabiter->second.size() == 0)
					continue;

				// First delete the records in the database
				std::string sqlstr = "delete from " ENTITY_TABLE_PERFIX "_";
				sqlstr += tabiter->first;
				sqlstr += " where " TABLE_ID_CONST_STR " in (";

				std::vector<DBID>::iterator iter = tabiter->second.begin();
				for(; iter != tabiter->second.end(); ++iter)
				{
					DBID dbid = (*iter);

					char sqlstr1[MAX_BUF];
					ouro_snprintf(sqlstr1, MAX_BUF, "%" PRDBID, dbid);
					sqlstr += sqlstr1;
					sqlstr += ",";
				}
				
				sqlstr.erase(sqlstr.size() - 1);
				sqlstr += ")";
				bool ret = pdbi->query(sqlstr.c_str(), sqlstr.size(), false);
				OURO_ASSERT(ret);

				mysql::DBContext::DB_RW_CONTEXTS::iterator iter1 = context.optable.begin();
				for(; iter1 != context.optable.end(); ++iter1)
				{
					mysql::DBContext& wbox = *iter1->second.get();
					if(wbox.tableName == tabiter->first)
					{
						std::vector<DBID>::iterator iter = tabiter->second.begin();
						for(; iter != tabiter->second.end(); ++iter)
						{
							DBID dbid = (*iter);
							
							wbox.parentTableDBID = context.dbid;
							wbox.dbid = dbid;
							wbox.isEmpty = true;

							// update the child table
							writeDB(optype, pdbi, wbox);
						}
					}
				}
			}
		}
		return ret;
	}

protected:

};

}
#endif // OURO_WRITE_ENTITY_HELPER_H

