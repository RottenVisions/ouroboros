// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#include "entity_sqlstatement_mapping.h"
#include "sqlstatement.h"

namespace Ouroboros{ 
OURO_SINGLETON_INIT(EntitySqlStatementMapping);

EntitySqlStatementMapping _g_entitySqlStatementMapping;

//-------------------------------------------------------------------------------------
void EntitySqlStatementMapping::addQuerySqlStatement(const std::string& tableName, SqlStatement* pSqlStatement)
{
	query_sqlStatements_[tableName].reset(pSqlStatement);
}

//-------------------------------------------------------------------------------------
void EntitySqlStatementMapping::addInsertSqlStatement(const std::string& tableName, SqlStatement* pSqlStatement)
{
	insert_sqlStatements_[tableName].reset(pSqlStatement);
}

//-------------------------------------------------------------------------------------
void EntitySqlStatementMapping::addUpdateSqlStatement(const std::string& tableName, SqlStatement* pSqlStatement)
{
	update_sqlStatements_[tableName].reset(pSqlStatement);
}

//-------------------------------------------------------------------------------------
SqlStatement* EntitySqlStatementMapping::findQuerySqlStatement(const std::string& tableName)
{
	OUROUnordered_map< std::string, OUROShared_ptr< SqlStatement > >::iterator iter = 
		query_sqlStatements_.find(tableName);

	if(iter != query_sqlStatements_.end())
		return iter->second.get();

	return NULL;
}

//-------------------------------------------------------------------------------------
SqlStatement* EntitySqlStatementMapping::findInsertSqlStatement(const std::string& tableName)
{
	OUROUnordered_map< std::string, OUROShared_ptr< SqlStatement > >::iterator iter = 
		insert_sqlStatements_.find(tableName);

	if(iter != insert_sqlStatements_.end())
		return iter->second.get();

	return NULL;
}

//-------------------------------------------------------------------------------------
SqlStatement* EntitySqlStatementMapping::findUpdateSqlStatement(const std::string& tableName)
{
	OUROUnordered_map< std::string, OUROShared_ptr< SqlStatement > >::iterator iter = 
		update_sqlStatements_.find(tableName);

	if(iter != update_sqlStatements_.end())
		return iter->second.get();

	return NULL;
}

//-------------------------------------------------------------------------------------
}

