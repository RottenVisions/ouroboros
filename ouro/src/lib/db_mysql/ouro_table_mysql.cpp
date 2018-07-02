// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#include "entity_table_mysql.h"
#include "ouro_table_mysql.h"
#include "db_exception.h"
#include "db_interface_mysql.h"
#include "db_interface/db_interface.h"
#include "db_interface/entity_table.h"
#include "entitydef/entitydef.h"
#include "entitydef/scriptdef_module.h"
#include "server/serverconfig.h"
#include "server/common.h"

namespace Ouroboros {

//-------------------------------------------------------------------------------------
bool OUROEntityLogTableMysql::syncToDB(DBInterface* pdbi)
{
	std::string sqlstr = "CREATE TABLE IF NOT EXISTS " OURO_TABLE_PERFIX "_entitylog "
			"(entityDBID bigint(20) unsigned not null DEFAULT 0,"
			"entityType int unsigned not null DEFAULT 0,"
			"entityID int unsigned not null DEFAULT 0,"
			"ip varchar(64),"
			"port int unsigned not null DEFAULT 0,"
			"componentID bigint unsigned not null DEFAULT 0,"
			"logger bigint unsigned not null DEFAULT 0,"
			"PRIMARY KEY (entityDBID, entityType))"
		"ENGINE=" MYSQL_ENGINE_TYPE;

	if(!pdbi->query(sqlstr.c_str(), sqlstr.size(), true))
		return false;

	// Clear expired logs
	OUROServerLogTableMysql serverLogTable(NULL);

	std::vector<COMPONENT_ID> cids = serverLogTable.queryTimeOutServers(pdbi);

	if (!serverLogTable.clearTimeoutLogs(pdbi, cids))
		return false;

	cids.push_back(g_componentID);

	sqlstr = fmt::format("delete from " OURO_TABLE_PERFIX "_entitylog where logger in (");

	char tbuf[MAX_BUF];

	std::vector<COMPONENT_ID>::iterator citer = cids.begin();
	for(; citer != cids.end(); ++citer)
	{
		ouro_snprintf(tbuf, MAX_BUF, "%" PRDBID, (*citer));
		sqlstr += tbuf;
		sqlstr += ",";
	}

	if (sqlstr[sqlstr.size() - 1] == ',')
		sqlstr.erase(sqlstr.end() - 1);

	sqlstr += ")";

	if(!pdbi->query(sqlstr.c_str(), sqlstr.size(), true))
		return false;

	return true;
}

//-------------------------------------------------------------------------------------
bool OUROEntityLogTableMysql::logEntity(DBInterface * pdbi, const char* ip, uint32 port, DBID dbid,
					COMPONENT_ID componentID, ENTITY_ID entityID, ENTITY_SCRIPT_UID entityType)
{
	std::string sqlstr = "insert into " OURO_TABLE_PERFIX "_entitylog (entityDBID, entityType, entityID, ip, port, componentID, logger) values(";

	char* tbuf = new char[MAX_BUF * 3];

	ouro_snprintf(tbuf, MAX_BUF, "%" PRDBID, dbid);
	sqlstr += tbuf;
	sqlstr += ",";

	ouro_snprintf(tbuf, MAX_BUF, "%u", entityType);
	sqlstr += tbuf;
	sqlstr += ",";

	ouro_snprintf(tbuf, MAX_BUF, "%d", entityID);
	sqlstr += tbuf;
	sqlstr += ",\"";

	mysql_real_escape_string(static_cast<DBInterfaceMysql*>(pdbi)->mysql(),
		tbuf, ip, strlen(ip));

	sqlstr += tbuf;
	sqlstr += "\",";

	ouro_snprintf(tbuf, MAX_BUF, "%u", port);
	sqlstr += tbuf;
	sqlstr += ",";

	ouro_snprintf(tbuf, MAX_BUF, "%" PRDBID, componentID);
	sqlstr += tbuf;
	sqlstr += ",";

	ouro_snprintf(tbuf, MAX_BUF, "%" PRDBID, g_componentID);
	sqlstr += tbuf;
	sqlstr += ")";

	SAFE_RELEASE_ARRAY(tbuf);

	try
	{
		if(!pdbi->query(sqlstr.c_str(), sqlstr.size(), false))
		{
			// 1062 int err = pdbi->getlasterror();
			return false;
		}
	}
	catch (std::exception & e)
	{
		DBException& dbe = static_cast<DBException&>(e);
		if(dbe.isLostConnection())
		{
			if(pdbi->processException(e))
				return true;
		}

		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------
bool OUROEntityLogTableMysql::queryEntity(DBInterface * pdbi, DBID dbid, EntityLog& entitylog, ENTITY_SCRIPT_UID entityType)
{
	std::string sqlstr = "select entityID, ip, port, componentID, logger from " OURO_TABLE_PERFIX "_entitylog where entityDBID=";

	char tbuf[MAX_BUF];
	ouro_snprintf(tbuf, MAX_BUF, "%" PRDBID, dbid);
	sqlstr += tbuf;

	sqlstr += " and entityType=";
	ouro_snprintf(tbuf, MAX_BUF, "%u", entityType);
	sqlstr += tbuf;
	sqlstr += " LIMIT 1";

	if(!pdbi->query(sqlstr.c_str(), sqlstr.size(), false))
	{
		return true;
	}

	entitylog.dbid = dbid;
	entitylog.componentID = 0;
	entitylog.logger = 0;
	entitylog.entityID = 0;
	entitylog.ip[0] = '\0';
	entitylog.port = 0;

	MYSQL_RES * pResult = mysql_store_result(static_cast<DBInterfaceMysql*>(pdbi)->mysql());
	if(pResult)
	{
		MYSQL_ROW arow = mysql_fetch_row(pResult);
		if(arow != NULL)
		{
			StringConv::str2value(entitylog.entityID, arow[0]);
			ouro_snprintf(entitylog.ip, MAX_IP, "%s", arow[1]);
			StringConv::str2value(entitylog.port, arow[2]);
			StringConv::str2value(entitylog.componentID, arow[3]);
			StringConv::str2value(entitylog.logger, arow[4]);
		}

		mysql_free_result(pResult);
	}

	return entitylog.componentID > 0;
}

//-------------------------------------------------------------------------------------
bool OUROEntityLogTableMysql::eraseEntityLog(DBInterface * pdbi, DBID dbid, ENTITY_SCRIPT_UID entityType)
{
	std::string sqlstr = "delete from " OURO_TABLE_PERFIX "_entitylog where entityDBID=";

	char tbuf[MAX_BUF];

	ouro_snprintf(tbuf, MAX_BUF, "%" PRDBID, dbid);
	sqlstr += tbuf;

	sqlstr += " and entityType=";
	ouro_snprintf(tbuf, MAX_BUF, "%u", entityType);
	sqlstr += tbuf;

	if(!pdbi->query(sqlstr.c_str(), sqlstr.size(), false))
	{
		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------
OUROEntityLogTableMysql::OUROEntityLogTableMysql(EntityTables* pEntityTables):
OUROEntityLogTable(pEntityTables)
{
}

//-------------------------------------------------------------------------------------
bool OUROServerLogTableMysql::syncToDB(DBInterface* pdbi)
{
	std::string sqlstr = "";

	bool ret = false;

	sqlstr = "CREATE TABLE IF NOT EXISTS " OURO_TABLE_PERFIX "_serverlog "
			"(heartbeatTime bigint(20) unsigned not null DEFAULT 0,"
			"logger bigint unsigned not null DEFAULT 0,"
			"PRIMARY KEY (logger))"
		"ENGINE=" MYSQL_ENGINE_TYPE;

	ret = pdbi->query(sqlstr.c_str(), sqlstr.size(), true);
	OURO_ASSERT(ret);
	return ret && updateServer(pdbi);
}

//-------------------------------------------------------------------------------------
bool OUROServerLogTableMysql::updateServer(DBInterface * pdbi)
{
	std::string sqlstr = "insert into " OURO_TABLE_PERFIX "_serverlog (heartbeatTime, logger) values(";

	char* tbuf = new char[MAX_BUF * 3];

	ouro_snprintf(tbuf, MAX_BUF, "%" PRTime, time(NULL));
	sqlstr += tbuf;
	sqlstr += ",";

	ouro_snprintf(tbuf, MAX_BUF, "%" PRDBID, g_componentID);
	sqlstr += tbuf;
	sqlstr += ") ON DUPLICATE KEY UPDATE heartbeatTime=";

	ouro_snprintf(tbuf, MAX_BUF, "%" PRTime, time(NULL));
	sqlstr += tbuf;

	SAFE_RELEASE_ARRAY(tbuf);

	try
	{
		if(!pdbi->query(sqlstr.c_str(), sqlstr.size(), false))
		{
			// 1062 int err = pdbi->getlasterror();
			return false;
		}
	}
	catch (std::exception & e)
	{
		DBException& dbe = static_cast<DBException&>(e);
		if(dbe.isLostConnection())
		{
			if(pdbi->processException(e))
				return true;
		}

		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------
bool OUROServerLogTableMysql::queryServer(DBInterface * pdbi, ServerLog& serverlog)
{
	std::string sqlstr = "select heartbeatTime from " OURO_TABLE_PERFIX "_serverlog where logger=";

	char tbuf[MAX_BUF];
	ouro_snprintf(tbuf, MAX_BUF, "%" PRDBID, g_componentID);
	sqlstr += tbuf;

	sqlstr += " LIMIT 1";

	if(!pdbi->query(sqlstr.c_str(), sqlstr.size(), false))
	{
		return true;
	}

	serverlog.heartbeatTime = 0;

	bool get = false;

	MYSQL_RES * pResult = mysql_store_result(static_cast<DBInterfaceMysql*>(pdbi)->mysql());
	if(pResult)
	{
		MYSQL_ROW arow = mysql_fetch_row(pResult);
		if(arow != NULL)
		{
			StringConv::str2value(serverlog.heartbeatTime, arow[0]);
			get = true;
		}

		mysql_free_result(pResult);
	}

	return get;
}

//-------------------------------------------------------------------------------------
std::vector<COMPONENT_ID> OUROServerLogTableMysql::queryTimeOutServers(DBInterface * pdbi)
{
	std::vector<COMPONENT_ID> cids;

	std::string sqlstr = "select heartbeatTime,logger from " OURO_TABLE_PERFIX "_serverlog";

	if(!pdbi->query(sqlstr.c_str(), sqlstr.size(), false))
	{
		return cids;
	}

	MYSQL_RES * pResult = mysql_store_result(static_cast<DBInterfaceMysql*>(pdbi)->mysql());
	if(pResult)
	{
		MYSQL_ROW arow;
		while((arow = mysql_fetch_row(pResult)) != NULL)
		{
			ServerLog serverlog;
			Ouroboros::StringConv::str2value(serverlog.heartbeatTime, arow[0]);
			Ouroboros::StringConv::str2value(serverlog.logger, arow[1]);

			if(serverlog.logger == g_componentID)
				continue;

			if(time(NULL) - serverlog.heartbeatTime > OUROServerLogTable::TIMEOUT * 2)
				cids.push_back(serverlog.logger);
		}

		mysql_free_result(pResult);
	}

	return cids;
}

//-------------------------------------------------------------------------------------
bool OUROServerLogTableMysql::clearTimeoutLogs(DBInterface * pdbi, const std::vector<COMPONENT_ID>& cids)
{
	if(cids.size() == 0)
		return true;

	std::string sqlstr = "delete from " OURO_TABLE_PERFIX "_serverlog where logger in (";

	char tbuf[MAX_BUF];

	std::vector<COMPONENT_ID>::const_iterator citer = cids.begin();
	for(; citer != cids.end(); ++citer)
	{
		if((*citer) == g_componentID)
			continue;

		ouro_snprintf(tbuf, MAX_BUF, "%" PRDBID, (*citer));
		sqlstr += tbuf;
		sqlstr += ",";
	}

	if (sqlstr[sqlstr.size() - 1] == ',')
		sqlstr.erase(sqlstr.end() - 1);

	sqlstr += ")";

	if(!pdbi->query(sqlstr.c_str(), sqlstr.size(), false))
	{
		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------
OUROServerLogTableMysql::OUROServerLogTableMysql(EntityTables* pEntityTables):
OUROServerLogTable(pEntityTables)
{
}

//-------------------------------------------------------------------------------------
bool OUROAccountTableMysql::syncToDB(DBInterface* pdbi)
{
	bool ret = false;

	std::string sqlstr = fmt::format("CREATE TABLE IF NOT EXISTS " OURO_TABLE_PERFIX "_accountinfos "
		"(`accountName` varchar({}) not null, PRIMARY KEY idKey (`accountName`),"
		"`password` varchar({}),"
			"`bindata` blob,"
			"`email` varchar(191) not null, UNIQUE KEY `email` (`email`),"
			"`entityDBID` bigint(20) unsigned not null DEFAULT 0, UNIQUE KEY `entityDBID` (`entityDBID`),"
			"`flags` int unsigned not null DEFAULT 0,"
			"`deadline` bigint(20) not null DEFAULT 0,"
			"`regtime` bigint(20) not null DEFAULT 0,"
			"`lasttime` bigint(20) not null DEFAULT 0,"
			"`numlogin` int unsigned not null DEFAULT 0)"
			"ENGINE=" MYSQL_ENGINE_TYPE, ACCOUNT_NAME_MAX_LENGTH, ACCOUNT_PASSWD_MAX_LENGTH);

	ret = pdbi->query(sqlstr.c_str(), sqlstr.size(), true);
	OURO_ASSERT(ret);
	return ret;
}

//-------------------------------------------------------------------------------------
OUROAccountTableMysql::OUROAccountTableMysql(EntityTables* pEntityTables) :
OUROAccountTable(pEntityTables)
{
}

//-------------------------------------------------------------------------------------
bool OUROAccountTableMysql::setFlagsDeadline(DBInterface * pdbi, const std::string& name, uint32 flags, uint64 deadline)
{
	char* tbuf = new char[name.size() * 2 + 1];

	mysql_real_escape_string(static_cast<DBInterfaceMysql*>(pdbi)->mysql(),
		tbuf, name.c_str(), name.size());

	std::string sqlstr = fmt::format("update " OURO_TABLE_PERFIX "_accountinfos set flags={}, deadline={} where accountName=\"{}\"",
		flags, deadline, tbuf);

	SAFE_RELEASE_ARRAY(tbuf);

	// Return if the query fails, to avoid possible errors
	if(pdbi->query(sqlstr.c_str(), sqlstr.size(), false))
		return true;

	return false;
}

//-------------------------------------------------------------------------------------
bool OUROAccountTableMysql::queryAccount(DBInterface * pdbi, const std::string& name, ACCOUNT_INFOS& info)
{
	std::string sqlstr = "select entityDBID, password, flags, deadline, bindata from " OURO_TABLE_PERFIX "_accountinfos where accountName=\"";

	char* tbuf = new char[name.size() * 2 + 1];

	mysql_real_escape_string(static_cast<DBInterfaceMysql*>(pdbi)->mysql(),
		tbuf, name.c_str(), name.size());

	sqlstr += tbuf;
	sqlstr += "\" or email=\"";
	sqlstr += tbuf;
	sqlstr += "\" LIMIT 1";
	SAFE_RELEASE_ARRAY(tbuf);

	// Return if the query fails, to avoid possible errors
	if(!pdbi->query(sqlstr.c_str(), sqlstr.size(), false))
		return true;

	info.dbid = 0;
	MYSQL_RES * pResult = mysql_store_result(static_cast<DBInterfaceMysql*>(pdbi)->mysql());
	if(pResult)
	{
		MYSQL_ROW arow = mysql_fetch_row(pResult);
		if(arow != NULL)
		{
			unsigned long *lengths = mysql_fetch_lengths(pResult);

			Ouroboros::StringConv::str2value(info.dbid, arow[0]);
			info.name = name;
			info.password = arow[1];

			Ouroboros::StringConv::str2value(info.flags, arow[2]);
			Ouroboros::StringConv::str2value(info.deadline, arow[3]);

			info.datas.assign(arow[4], lengths[4]);
		}

		mysql_free_result(pResult);
	}

	return info.dbid > 0;
}

//-------------------------------------------------------------------------------------
bool OUROAccountTableMysql::queryAccountAllInfos(DBInterface * pdbi, const std::string& name, ACCOUNT_INFOS& info)
{
	std::string sqlstr = "select entityDBID, password, email, flags, deadline from " OURO_TABLE_PERFIX "_accountinfos where accountName=\"";

	char* tbuf = new char[name.size() * 2 + 1];

	mysql_real_escape_string(static_cast<DBInterfaceMysql*>(pdbi)->mysql(),
		tbuf, name.c_str(), name.size());

	sqlstr += tbuf;
	sqlstr += "\" or email=\"";
	sqlstr += tbuf;
	sqlstr += "\" LIMIT 1";
	SAFE_RELEASE_ARRAY(tbuf);

	// Return if the query fails, to avoid possible errors
	if(!pdbi->query(sqlstr.c_str(), sqlstr.size(), false))
		return true;

	info.dbid = 0;
	MYSQL_RES * pResult = mysql_store_result(static_cast<DBInterfaceMysql*>(pdbi)->mysql());
	if(pResult)
	{
		MYSQL_ROW arow = mysql_fetch_row(pResult);
		if(arow != NULL)
		{
			Ouroboros::StringConv::str2value(info.dbid, arow[0]);
			info.name = name;
			info.password = arow[1];
			info.email = arow[2];
			Ouroboros::StringConv::str2value(info.flags, arow[3]);
			Ouroboros::StringConv::str2value(info.deadline, arow[4]);
		}

		mysql_free_result(pResult);
	}

	return info.dbid > 0;
}

//-------------------------------------------------------------------------------------
bool OUROAccountTableMysql::updateCount(DBInterface * pdbi, const std::string& name, DBID dbid)
{
	// Return if the query fails, to avoid possible errors
	if(!pdbi->query(fmt::format("update " OURO_TABLE_PERFIX "_accountinfos set lasttime={}, numlogin=numlogin+1 where entityDBID={}",
		time(NULL), dbid), false))
		return false;

	return true;
}

//-------------------------------------------------------------------------------------
bool OUROAccountTableMysql::updatePassword(DBInterface * pdbi, const std::string& name, const std::string& password)
{
	char* tbuf = new char[MAX_BUF * 3];
	char* tbuf1 = new char[MAX_BUF * 3];

	mysql_real_escape_string(static_cast<DBInterfaceMysql*>(pdbi)->mysql(),
		tbuf, password.c_str(), password.size());

	mysql_real_escape_string(static_cast<DBInterfaceMysql*>(pdbi)->mysql(),
		tbuf1, name.c_str(), name.size());

	// Return if the query fails, to avoid possible errors
	if(!pdbi->query(fmt::format("update " OURO_TABLE_PERFIX "_accountinfos set password=\"{}\" where accountName like \"{}\"",
		password, tbuf1), false))
	{
		SAFE_RELEASE_ARRAY(tbuf);
		SAFE_RELEASE_ARRAY(tbuf1);
		return false;
	}

	SAFE_RELEASE_ARRAY(tbuf);
	SAFE_RELEASE_ARRAY(tbuf1);
	return true;
}

//-------------------------------------------------------------------------------------
bool OUROAccountTableMysql::logAccount(DBInterface * pdbi, ACCOUNT_INFOS& info)
{
	std::string sqlstr = "insert into " OURO_TABLE_PERFIX "_accountinfos (accountName, password, bindata, email, entityDBID, flags, deadline, regtime, lasttime) values(";

	char* tbuf = new char[MAX_BUF > info.datas.size() ? MAX_BUF * 3 : info.datas.size() * 3];

	mysql_real_escape_string(static_cast<DBInterfaceMysql*>(pdbi)->mysql(),
		tbuf, info.name.c_str(), info.name.size());

	sqlstr += "\"";
	sqlstr += tbuf;
	sqlstr += "\",";

	mysql_real_escape_string(static_cast<DBInterfaceMysql*>(pdbi)->mysql(),
		tbuf, info.password.c_str(), info.password.size());

	sqlstr += "md5(\"";
	sqlstr += tbuf;
	sqlstr += "\"),";

	mysql_real_escape_string(static_cast<DBInterfaceMysql*>(pdbi)->mysql(),
		tbuf, info.datas.data(), info.datas.size());

	sqlstr += "\"";
	sqlstr += tbuf;
	sqlstr += "\",";

	mysql_real_escape_string(static_cast<DBInterfaceMysql*>(pdbi)->mysql(),
		tbuf, info.email.c_str(), info.email.size());

	sqlstr += "\"";
	sqlstr += tbuf;
	sqlstr += "\",";

	ouro_snprintf(tbuf, MAX_BUF, "%" PRDBID, info.dbid);
	sqlstr += tbuf;
	sqlstr += ",";

	ouro_snprintf(tbuf, MAX_BUF, "%u", info.flags);
	sqlstr += tbuf;
	sqlstr += ",";

	ouro_snprintf(tbuf, MAX_BUF, "%" PRIu64, info.deadline);
	sqlstr += tbuf;
	sqlstr += ",";

	ouro_snprintf(tbuf, MAX_BUF, "%" PRTime, time(NULL));
	sqlstr += tbuf;
	sqlstr += ",";

	ouro_snprintf(tbuf, MAX_BUF, "%" PRTime, time(NULL));
	sqlstr += tbuf;
	sqlstr += ")";

	SAFE_RELEASE_ARRAY(tbuf);

	// Return if the query fails, to avoid possible errors
	if(!pdbi->query(sqlstr.c_str(), sqlstr.size(), false))
	{
		ERROR_MSG(fmt::format("OUROAccountTableMysql::logAccount({}): sql({}) is failed({})!\n",
				info.name, sqlstr, pdbi->getstrerror()));

		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------
OUROEmailVerificationTableMysql::OUROEmailVerificationTableMysql(EntityTables* pEntityTables) :
OUROEmailVerificationTable(pEntityTables)
{
}

//-------------------------------------------------------------------------------------
OUROEmailVerificationTableMysql::~OUROEmailVerificationTableMysql()
{
}

//-------------------------------------------------------------------------------------
bool OUROEmailVerificationTableMysql::queryAccount(DBInterface * pdbi, int8 type, const std::string& name, ACCOUNT_INFOS& info)
{
	std::string sqlstr = "select code, datas from " OURO_TABLE_PERFIX "_email_verification where accountName=\"";

	char* tbuf = new char[name.size() * 2 + 1];

	mysql_real_escape_string(static_cast<DBInterfaceMysql*>(pdbi)->mysql(),
		tbuf, name.c_str(), name.size());

	sqlstr += tbuf;

	sqlstr += "\" and type=";
	ouro_snprintf(tbuf, MAX_BUF, "%d", type);
	sqlstr += tbuf;
	sqlstr += " LIMIT 1";
	SAFE_RELEASE_ARRAY(tbuf);

	if(!pdbi->query(sqlstr.c_str(), sqlstr.size(), false))
	{
		ERROR_MSG(fmt::format("OUROEmailVerificationTableMysql::queryAccount({}): sql({}) is failed({})!\n",
				name, sqlstr, pdbi->getstrerror()));

		return false;
	}

	info.datas = "";
	MYSQL_RES * pResult = mysql_store_result(static_cast<DBInterfaceMysql*>(pdbi)->mysql());
	if(pResult)
	{
		MYSQL_ROW arow = mysql_fetch_row(pResult);
		if(arow != NULL)
		{
			info.name = name;
			info.datas = arow[0];
			info.password = arow[1];
		}

		mysql_free_result(pResult);
	}

	return info.datas.size() > 0;
}

//-------------------------------------------------------------------------------------
bool OUROEmailVerificationTableMysql::logAccount(DBInterface * pdbi, int8 type, const std::string& name,
												const std::string& datas, const std::string& code)
{
	std::string sqlstr = "insert into " OURO_TABLE_PERFIX "_email_verification (accountName, type, datas, code, logtime) values(";

	char* tbuf = new char[MAX_BUF > datas.size() ? MAX_BUF * 3 :
		(code.size() > datas.size() ? code.size() * 3 : datas.size() * 3)];

	mysql_real_escape_string(static_cast<DBInterfaceMysql*>(pdbi)->mysql(),
		tbuf, name.c_str(), name.size());

	sqlstr += "\"";
	sqlstr += tbuf;
	sqlstr += "\",";

	ouro_snprintf(tbuf, MAX_BUF, "%d,", type);
	sqlstr += tbuf;

	mysql_real_escape_string(static_cast<DBInterfaceMysql*>(pdbi)->mysql(),
		tbuf, datas.c_str(), datas.size());

	sqlstr += "\"";
	sqlstr += tbuf;
	sqlstr += "\",";

	mysql_real_escape_string(static_cast<DBInterfaceMysql*>(pdbi)->mysql(),
		tbuf, code.c_str(), code.size());

	sqlstr += "\"";
	sqlstr += tbuf;
	sqlstr += "\",";

	ouro_snprintf(tbuf, MAX_BUF, "%" PRTime, time(NULL));

	sqlstr += tbuf;
	sqlstr += ")";

	SAFE_RELEASE_ARRAY(tbuf);

	if(!pdbi->query(sqlstr.c_str(), sqlstr.size(), false))
	{
		ERROR_MSG(fmt::format("OUROEmailVerificationTableMysql::logAccount({}): sql({}) is failed({})!\n",
				code, sqlstr, pdbi->getstrerror()));

		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------
bool OUROEmailVerificationTableMysql::activateAccount(DBInterface * pdbi, const std::string& code, ACCOUNT_INFOS& info)
{
	std::string sqlstr = "select accountName, datas, logtime from " OURO_TABLE_PERFIX "_email_verification where code=\"";

	char* tbuf = new char[code.size() * 2 + 1];

	mysql_real_escape_string(static_cast<DBInterfaceMysql*>(pdbi)->mysql(),
		tbuf, code.c_str(), code.size());

	sqlstr += tbuf;

	sqlstr += "\" and type=";
	ouro_snprintf(tbuf, MAX_BUF, "%d", (int)OUROEmailVerificationTable::V_TYPE_CREATEACCOUNT);
	sqlstr += tbuf;
	sqlstr += " LIMIT 1";
	SAFE_RELEASE_ARRAY(tbuf);

	if(!pdbi->query(sqlstr.c_str(), sqlstr.size(), false))
	{
		ERROR_MSG(fmt::format("OUROEmailVerificationTableMysql::activateAccount({}): sql({}) is failed({})!\n",
				code, sqlstr, pdbi->getstrerror()));

		return false;
	}

	uint64 logtime = 1;

	MYSQL_RES * pResult = mysql_store_result(static_cast<DBInterfaceMysql*>(pdbi)->mysql());
	if(pResult)
	{
		MYSQL_ROW arow = mysql_fetch_row(pResult);
		if(arow != NULL)
		{
			info.name = arow[0];
			info.password = arow[1];

			Ouroboros::StringConv::str2value(logtime, arow[2]);
		}

		mysql_free_result(pResult);
	}

	if(logtime > 0 && time(NULL) - logtime > g_ouroSrvConfig.emailAtivationInfo_.deadline)
	{
		ERROR_MSG(fmt::format("OUROEmailVerificationTableMysql::activateAccount({}): is expired! {} > {}.\n",
				code, (time(NULL) - logtime), g_ouroSrvConfig.emailAtivationInfo_.deadline));

		return false;
	}

	if(info.name.size() == 0)
	{
		ERROR_MSG(fmt::format("OUROEmailVerificationTableMysql::activateAccount({}): name is NULL.\n",
				code));

		return false;
	}

	std::string password = info.password;

	// Find dblog whether this account
	OUROAccountTable* pTable = static_cast<OUROAccountTable*>(EntityTables::findByInterfaceName(pdbi->name()).findOUROTable(OURO_TABLE_PERFIX "_accountinfos"));
	OURO_ASSERT(pTable);

	info.flags = 0;
	if(!pTable->queryAccount(pdbi, info.name, info))
	{
		return false;
	}

	if((info.flags & ACCOUNT_FLAG_NOT_ACTIVATED) <= 0)
	{
		ERROR_MSG(fmt::format("OUROEmailVerificationTableMysql::activateAccount({}): Has been activated, flags={}.\n",
				code, info.flags));

		return false;
	}

	info.flags &= ~ACCOUNT_FLAG_NOT_ACTIVATED;

	if(!pTable->setFlagsDeadline(pdbi, info.name, info.flags, info.deadline))
	{
		ERROR_MSG(fmt::format("OUROEmailVerificationTableMysql::activateAccount({}): set deadline is error({})!\n",
				code, pdbi->getstrerror()));
		return false;
	}

	if(!pTable->updatePassword(pdbi, info.name, password))
	{
		ERROR_MSG(fmt::format("OUROEmailVerificationTableMysql::activateAccount({}): update password is error({})!\n",
				code, pdbi->getstrerror()));

		return false;
	}

	info.dbid = 0;

	ScriptDefModule* pModule = EntityDef::findScriptModule(DBUtil::accountScriptName());

	// To prevent multithreading problems, make a copy here.
	MemoryStream copyAccountDefMemoryStream(pTable->accountDefMemoryStream());

	info.dbid = EntityTables::findByInterfaceName(pdbi->name()).writeEntity(pdbi, 0, -1,
			&copyAccountDefMemoryStream, pModule);

	OURO_ASSERT(info.dbid > 0);

	// Return if the query fails, to avoid possible errors
	tbuf = new char[MAX_BUF * 3];

	mysql_real_escape_string(static_cast<DBInterfaceMysql*>(pdbi)->mysql(),
		tbuf, info.name.c_str(), info.name.size());

	if(!pdbi->query(fmt::format("update " OURO_TABLE_PERFIX "_accountinfos set entityDBID={} where accountName like \"{}\"",
		info.dbid, tbuf), false))
	{
		ERROR_MSG(fmt::format("OUROEmailVerificationTableMysql::activateAccount({}): update " OURO_TABLE_PERFIX "_accountinfos is error({})!\n",
				code, pdbi->getstrerror()));

		SAFE_RELEASE_ARRAY(tbuf);
		return false;
	}

	SAFE_RELEASE_ARRAY(tbuf);

	try
	{
		delAccount(pdbi, (int8)V_TYPE_CREATEACCOUNT, info.name);
	}
	catch (...)
	{
	}

	return true;
}

//-------------------------------------------------------------------------------------
bool OUROEmailVerificationTableMysql::bindEMail(DBInterface * pdbi, const std::string& name, const std::string& code)
{
	std::string sqlstr = "select accountName, datas, logtime from " OURO_TABLE_PERFIX "_email_verification where code=\"";

	char* tbuf = new char[code.size() * 2 + 1];

	mysql_real_escape_string(static_cast<DBInterfaceMysql*>(pdbi)->mysql(),
		tbuf, code.c_str(), code.size());

	sqlstr += tbuf;

	sqlstr += "\" and type=";
	ouro_snprintf(tbuf, MAX_BUF, "%d", (int)OUROEmailVerificationTable::V_TYPE_BIND_MAIL);
	sqlstr += tbuf;
	sqlstr += " LIMIT 1";
	SAFE_RELEASE_ARRAY(tbuf);

	if(!pdbi->query(sqlstr.c_str(), sqlstr.size(), false))
	{
		ERROR_MSG(fmt::format("OUROEmailVerificationTableMysql::bindEMail({}): sql({}) is failed({})!\n",
				code, sqlstr, pdbi->getstrerror()));

		return false;
	}

	uint64 logtime = 1;

	std::string qname, qemail;

	MYSQL_RES * pResult = mysql_store_result(static_cast<DBInterfaceMysql*>(pdbi)->mysql());
	if(pResult)
	{
		MYSQL_ROW arow = mysql_fetch_row(pResult);
		if(arow != NULL)
		{
			qname = arow[0];
			qemail = arow[1];

			Ouroboros::StringConv::str2value(logtime, arow[2]);
		}

		mysql_free_result(pResult);
	}

	if(logtime > 0 && time(NULL) - logtime > g_ouroSrvConfig.emailBindInfo_.deadline)
	{
		ERROR_MSG(fmt::format("OUROEmailVerificationTableMysql::bindEMail({}): is expired! {} > {}.\n",
				code, (time(NULL) - logtime), g_ouroSrvConfig.emailBindInfo_.deadline));

		return false;
	}

	if(qname.size() == 0 || qemail.size() == 0)
	{
		ERROR_MSG(fmt::format("OUROEmailVerificationTableMysql::bindEMail({}): name or email is NULL.\n",
				code));

		return false;
	}

	if(qemail != name)
	{
		WARNING_MSG(fmt::format("OUROEmailVerificationTableMysql::bindEMail: code({}) username({}:{}, {}) not match.\n"
			, code, name, qname, qemail));

		return false;
	}

	tbuf = new char[code.size() * 2 + 1];

	mysql_real_escape_string(static_cast<DBInterfaceMysql*>(pdbi)->mysql(),
		tbuf, qemail.c_str(), qemail.size());

	sqlstr = "update " OURO_TABLE_PERFIX "_accountinfos set email=\"";
	sqlstr += tbuf;
	sqlstr += "\" where accountName like \"";

	mysql_real_escape_string(static_cast<DBInterfaceMysql*>(pdbi)->mysql(),
		tbuf, qname.c_str(), qname.size());

	sqlstr += tbuf;
	sqlstr += "\"";

	SAFE_RELEASE_ARRAY(tbuf);

	if(!pdbi->query(sqlstr, false))
	{
		ERROR_MSG(fmt::format("OUROEmailVerificationTableMysql::bindEMail({}): update " OURO_TABLE_PERFIX "_accountinfos({}) error({})!\n",
				code, qname, pdbi->getstrerror()));

		return false;
	}

	try
	{
		delAccount(pdbi, (int8)V_TYPE_BIND_MAIL, name);
	}
	catch (...)
	{
	}

	return true;
}

//-------------------------------------------------------------------------------------
bool OUROEmailVerificationTableMysql::resetpassword(DBInterface * pdbi, const std::string& name,
												   const std::string& password, const std::string& code)
{
	std::string sqlstr = "select accountName, datas, logtime from " OURO_TABLE_PERFIX "_email_verification where code=\"";

	char* tbuf = new char[code.size() * 2 + 1];

	mysql_real_escape_string(static_cast<DBInterfaceMysql*>(pdbi)->mysql(),
		tbuf, code.c_str(), code.size());

	sqlstr += tbuf;

	sqlstr += "\" and type=";
	ouro_snprintf(tbuf, MAX_BUF, "%d", (int)OUROEmailVerificationTable::V_TYPE_RESETPASSWORD);
	sqlstr += tbuf;
	sqlstr += " LIMIT 1";
	SAFE_RELEASE_ARRAY(tbuf);

	if(!pdbi->query(sqlstr.c_str(), sqlstr.size(), false))
	{
		ERROR_MSG(fmt::format("OUROEmailVerificationTableMysql::resetpassword({}): sql({}) is failed({})!\n",
				code, sqlstr, pdbi->getstrerror()));

		return false;
	}

	uint64 logtime = 1;

	std::string qname, qemail;

	MYSQL_RES * pResult = mysql_store_result(static_cast<DBInterfaceMysql*>(pdbi)->mysql());
	if(pResult)
	{
		MYSQL_ROW arow = mysql_fetch_row(pResult);
		if(arow != NULL)
		{
			qname = arow[0];
			qemail = arow[1];
			Ouroboros::StringConv::str2value(logtime, arow[2]);
		}

		mysql_free_result(pResult);
	}

	if(logtime > 0 && time(NULL) - logtime > g_ouroSrvConfig.emailResetPasswordInfo_.deadline)
	{
		ERROR_MSG(fmt::format("OUROEmailVerificationTableMysql::resetpassword({}): is expired! {} > {}.\n",
				code, (time(NULL) - logtime), g_ouroSrvConfig.emailResetPasswordInfo_.deadline));

		return false;
	}

	if(qname.size() == 0 || password.size() == 0)
	{
		ERROR_MSG(fmt::format("OUROEmailVerificationTableMysql::resetpassword({}): name or password is NULL.\n",
				code));

		return false;
	}

	if(qname != name)
	{
		WARNING_MSG(fmt::format("OUROEmailVerificationTableMysql::resetpassword: code({}) username({} != {}) not match.\n"
			, code, name, qname));

		return false;
	}

	// Find dblog whether this account
	OUROAccountTable* pTable = static_cast<OUROAccountTable*>(EntityTables::findByInterfaceName(pdbi->name()).findOUROTable(OURO_TABLE_PERFIX "_accountinfos"));
	OURO_ASSERT(pTable);

	if(!pTable->updatePassword(pdbi, qname, OURO_MD5::getDigest(password.data(), password.length())))
	{
		ERROR_MSG(fmt::format("OUROEmailVerificationTableMysql::resetpassword({}): update accountName({}) password error({})!\n",
				code, qname, pdbi->getstrerror()));

		return false;
	}


	try
	{
		delAccount(pdbi, (int8)V_TYPE_RESETPASSWORD, qname);
	}
	catch (...)
	{
	}

	return true;
}

//-------------------------------------------------------------------------------------
bool OUROEmailVerificationTableMysql::delAccount(DBInterface * pdbi, int8 type, const std::string& name)
{
	std::string sqlstr = "delete from " OURO_TABLE_PERFIX "_email_verification where accountName=";

	char* tbuf = new char[MAX_BUF * 3];

	mysql_real_escape_string(static_cast<DBInterfaceMysql*>(pdbi)->mysql(),
		tbuf, name.c_str(), name.size());

	sqlstr += "\"";
	sqlstr += tbuf;
	sqlstr += "\" and type=";

	ouro_snprintf(tbuf, MAX_BUF, "%d", type);
	sqlstr += tbuf;

	SAFE_RELEASE_ARRAY(tbuf);

	if(!pdbi->query(sqlstr.c_str(), sqlstr.size(), false))
	{
		ERROR_MSG(fmt::format("OUROEmailVerificationTableMysql::delAccount({}): sql({}) is failed({})!\n",
				name, sqlstr, pdbi->getstrerror()));

		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------
bool OUROEmailVerificationTableMysql::syncToDB(DBInterface* pdbi)
{
	bool ret = false;

	std::string sqlstr = fmt::format("CREATE TABLE IF NOT EXISTS " OURO_TABLE_PERFIX "_email_verification "
			"(accountName varchar({}) not null,"
			"type tinyint not null DEFAULT 0,"
			"datas varchar(255),"
			"code varchar(128), PRIMARY KEY idKey (code),"
			"logtime bigint(20) not null DEFAULT 0)"
			"ENGINE=" MYSQL_ENGINE_TYPE, ACCOUNT_NAME_MAX_LENGTH);

	ret = pdbi->query(sqlstr.c_str(), sqlstr.size(), true);
	OURO_ASSERT(ret);

	// Delete records before xx hours
	sqlstr = fmt::format("delete from " OURO_TABLE_PERFIX "_email_verification where logtime<{} and type={}",
		Ouroboros::StringConv::val2str(time(NULL) - g_ouroSrvConfig.emailAtivationInfo_.deadline),
		((int)OUROEmailVerificationTable::V_TYPE_CREATEACCOUNT));

	ret = pdbi->query(sqlstr.c_str(), sqlstr.size(), true);
	OURO_ASSERT(ret);

	sqlstr = fmt::format("delete from " OURO_TABLE_PERFIX "_email_verification where logtime<{} and type={}",
		Ouroboros::StringConv::val2str(time(NULL) - g_ouroSrvConfig.emailResetPasswordInfo_.deadline),
		((int)OUROEmailVerificationTable::V_TYPE_RESETPASSWORD));

	ret = pdbi->query(sqlstr.c_str(), sqlstr.size(), true);
	OURO_ASSERT(ret);

	sqlstr = fmt::format("delete from " OURO_TABLE_PERFIX "_email_verification where logtime<{} and type={}",
		Ouroboros::StringConv::val2str(time(NULL) - g_ouroSrvConfig.emailBindInfo_.deadline),
		((int)OUROEmailVerificationTable::V_TYPE_BIND_MAIL));

	ret = pdbi->query(sqlstr.c_str(), sqlstr.size(), true);
	OURO_ASSERT(ret);
	return ret;
}

//-------------------------------------------------------------------------------------
}
