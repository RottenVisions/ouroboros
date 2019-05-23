// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_ENTITY_SQL_STATEMENT_MAPPING_H
#define OURO_ENTITY_SQL_STATEMENT_MAPPING_H

// common include	
// #define NDEBUG
#include "common/common.h"
#include "common/singleton.h"
#include "helper/debug_helper.h"

namespace Ouroboros{ 

class SqlStatement;
class EntitySqlStatementMapping : public Singleton<EntitySqlStatementMapping>
{
public:
	EntitySqlStatementMapping()
	{
	}

	virtual ~EntitySqlStatementMapping()
	{
	}

	void addQuerySqlStatement(const std::string& tableName, SqlStatement* pSqlStatement);
	void addInsertSqlStatement(const std::string& tableName, SqlStatement* pSqlStatement);
	void addUpdateSqlStatement(const std::string& tableName, SqlStatement* pSqlStatement);

	SqlStatement* findQuerySqlStatement(const std::string& tableName);
	SqlStatement* findInsertSqlStatement(const std::string& tableName);
	SqlStatement* findUpdateSqlStatement(const std::string& tableName);
protected:
	KBEUnordered_map< std::string, KBEShared_ptr< SqlStatement > > query_sqlStatements_;
	KBEUnordered_map< std::string, KBEShared_ptr< SqlStatement > > update_sqlStatements_;
	KBEUnordered_map< std::string, KBEShared_ptr< SqlStatement > > insert_sqlStatements_;
};



}
#endif
