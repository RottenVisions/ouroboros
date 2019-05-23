// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

//#include "entity_table_redis.h"
#include "db_transaction.h"
#include "redis_helper.h"
#include "ouro_table_redis.h"
#include "db_interface_redis.h"
#include "db_interface/db_interface.h"
#include "db_interface/entity_table.h"
#include "entitydef/entitydef.h"
#include "entitydef/scriptdef_module.h"
#include "server/serverconfig.h"

namespace Ouroboros { 

//-------------------------------------------------------------------------------------
bool OUROEntityLogTableRedis::syncToDB(DBInterface* pdbi)
{
	/*
	Generate table data when there is data
	ouro_entitylog:dbid:entityType = hashes(entityID, ip, port, componentID, serverGroupID)
	*/

	return RedisHelper::dropTable(static_cast<DBInterfaceRedis*>(pdbi), fmt::format(OURO_TABLE_PERFIX "_entitylog:*:*"), false);
}

//-------------------------------------------------------------------------------------
bool OUROEntityLogTableRedis::logEntity(DBInterface * pdbi, const char* ip, uint32 port, DBID dbid,
					COMPONENT_ID componentID, ENTITY_ID entityID, ENTITY_SCRIPT_UID entityType)
{
	/*
	ouro_entitylog:dbid:entityType = hashes(entityID, ip, port, componentID, serverGroupID)
	*/
	std::string sqlstr = fmt::format("HSET " OURO_TABLE_PERFIX "_entitylog:{}:{} entityID {} ip {} port {} componentID {} serverGroupID {}", 
		dbid, entityType, entityID, ip, port, componentID, g_componentID);

	pdbi->query(sqlstr.c_str(), sqlstr.size(), false);
	return true;
}

//-------------------------------------------------------------------------------------
bool OUROEntityLogTableRedis::queryEntity(DBInterface * pdbi, DBID dbid, EntityLog& entitylog, ENTITY_SCRIPT_UID entityType)
{
	/*
	ouro_entitylog:dbid:entityType = hashes(entityID, ip, port, componentID, serverGroupID)
	*/
	redisReply* pRedisReply = NULL;

	static_cast<DBInterfaceRedis*>(pdbi)->query(fmt::format("HMGET " OURO_TABLE_PERFIX "_entitylog:{}:{} entityID ip port componentID serverGroupID",
		dbid, entityType), &pRedisReply, false);

	entitylog.dbid = dbid;
	entitylog.componentID = 0;
	entitylog.serverGroupID = 0;
	entitylog.entityID = 0;
	entitylog.ip[0] = '\0';
	entitylog.port = 0;

	if(pRedisReply)
	{
		if(pRedisReply->type == REDIS_REPLY_ARRAY)
		{
			if(RedisHelper::check_array_results(pRedisReply))
			{
				StringConv::str2value(entitylog.entityID, pRedisReply->element[0]->str);
				ouro_snprintf(entitylog.ip, MAX_IP, "%s", pRedisReply->element[1]->str);
				StringConv::str2value(entitylog.port, pRedisReply->element[2]->str);
				StringConv::str2value(entitylog.componentID, pRedisReply->element[3]->str);
				StringConv::str2value(entitylog.serverGroupID, pRedisReply->element[4]->str);
			}
		}
		
		freeReplyObject(pRedisReply); 
	}

	return entitylog.componentID > 0;
}

//-------------------------------------------------------------------------------------
bool OUROEntityLogTableRedis::eraseEntityLog(DBInterface * pdbi, DBID dbid, ENTITY_SCRIPT_UID entityType)
{
	std::string sqlstr = fmt::format("HDEL " OURO_TABLE_PERFIX "_entitylog:{}:{}", 
		dbid, entityType);
	
	pdbi->query(sqlstr.c_str(), sqlstr.size(), false);
	return true;
}

//-------------------------------------------------------------------------------------
bool OUROEntityLogTableRedis::eraseBaseappEntityLog(DBInterface * pdbi, COMPONENT_ID componentID)
{
	return false;
}

//-------------------------------------------------------------------------------------
OUROEntityLogTableRedis::OUROEntityLogTableRedis(EntityTables* pEntityTables) :
OUROEntityLogTable(pEntityTables)
{
}


//-------------------------------------------------------------------------------------
bool OUROServerLogTableRedis::syncToDB(DBInterface* pdbi)
{
	return true;
}

//-------------------------------------------------------------------------------------
bool OUROServerLogTableRedis::updateServer(DBInterface * pdbi)
{
	return true;
}

//-------------------------------------------------------------------------------------
bool OUROServerLogTableRedis::queryServer(DBInterface * pdbi, ServerLog& serverlog)
{
	return true;
}

//-------------------------------------------------------------------------------------
std::vector<COMPONENT_ID> OUROServerLogTableRedis::queryTimeOutServers(DBInterface * pdbi)
{
	std::vector<COMPONENT_ID> cids;

	return cids;
}

//-------------------------------------------------------------------------------------
std::vector<COMPONENT_ID> OUROServerLogTableRedis::queryServers(DBInterface * pdbi)
{
	std::vector<COMPONENT_ID> cids;

	return cids;
}

//-------------------------------------------------------------------------------------
bool OUROServerLogTableRedis::clearServers(DBInterface * pdbi, const std::vector<COMPONENT_ID>& cids)
{
	return true;
}

//-------------------------------------------------------------------------------------
OUROServerLogTableRedis::OUROServerLogTableRedis(EntityTables* pEntityTables):
OUROServerLogTable(pEntityTables)
{
}

//-------------------------------------------------------------------------------------
bool OUROAccountTableRedis::syncToDB(DBInterface* pdbi)
{
	return true;
}

//-------------------------------------------------------------------------------------
OUROAccountTableRedis::OUROAccountTableRedis(EntityTables* pEntityTables) :
OUROAccountTable(pEntityTables)
{
}

//-------------------------------------------------------------------------------------
bool OUROAccountTableRedis::setFlagsDeadline(DBInterface * pdbi, const std::string& name, uint32 flags, uint64 deadline)
{
	/*
	ouro_accountinfos:accountName = hashes(password, bindata, email, entityDBID, flags, deadline, regtime, lasttime, numlogin)
	*/
	
	// Return exists if the query fails, avoiding possible errors
	if(pdbi->query(fmt::format("HSET " OURO_TABLE_PERFIX "_accountinfos:{} flags {} deadline {}", 
		name, flags, deadline), false))
		return true;

	return false;
}

//-------------------------------------------------------------------------------------
bool OUROAccountTableRedis::queryAccount(DBInterface * pdbi, const std::string& name, ACCOUNT_INFOS& info)
{
	/*
	ouro_accountinfos:accountName = hashes(password, bindata, email, entityDBID, flags, deadline, regtime, lasttime, numlogin)
	*/
	redisReply* pRedisReply = NULL;

	static_cast<DBInterfaceRedis*>(pdbi)->query(fmt::format("HMGET " OURO_TABLE_PERFIX "_accountinfos:{} entityDBID password flags deadline",
		name), &pRedisReply, false);
	
	info.dbid = 0;
	
	if(pRedisReply)
	{
		if(pRedisReply->type == REDIS_REPLY_ARRAY)
		{
			if(RedisHelper::check_array_results(pRedisReply))
			{			
				StringConv::str2value(info.dbid, pRedisReply->element[0]->str);
				info.name = name;
				info.password = pRedisReply->element[1]->str;

				StringConv::str2value(info.flags, pRedisReply->element[2]->str);
				StringConv::str2value(info.deadline, pRedisReply->element[3]->str);
			}
		}
		
		freeReplyObject(pRedisReply); 
	}

	return info.dbid > 0;
}

//-------------------------------------------------------------------------------------
bool OUROAccountTableRedis::queryAccountAllInfos(DBInterface * pdbi, const std::string& name, ACCOUNT_INFOS& info)
{
	/*
	ouro_accountinfos:accountName = hashes(password, bindata, email, entityDBID, flags, deadline, regtime, lasttime, numlogin)
	*/
	redisReply* pRedisReply = NULL;

	static_cast<DBInterfaceRedis*>(pdbi)->query(fmt::format("HMGET " OURO_TABLE_PERFIX "_accountinfos:{} entityDBID password email flags deadline",
		name), &pRedisReply, false);

	info.dbid = 0;

	if(pRedisReply)
	{
		if(pRedisReply->type == REDIS_REPLY_ARRAY)
		{
			if(RedisHelper::check_array_results(pRedisReply))
			{			
				StringConv::str2value(info.dbid, pRedisReply->element[0]->str);
				info.name = name;
				info.password = pRedisReply->element[1]->str;
				info.email = pRedisReply->element[2]->str;
				StringConv::str2value(info.flags, pRedisReply->element[3]->str);
				StringConv::str2value(info.deadline, pRedisReply->element[4]->str);
			}
		}
		
		freeReplyObject(pRedisReply); 
	}

	return info.dbid > 0;
}

//-------------------------------------------------------------------------------------
bool OUROAccountTableRedis::updateCount(DBInterface * pdbi, const std::string& name, DBID dbid)
{
	/*
	ouro_accountinfos:accountName = hashes(password, bindata, email, entityDBID, flags, deadline, regtime, lasttime, numlogin)
	*/
	redis::DBTransaction transaction(pdbi);
	
	// Return exists if the query fails, avoiding possible errors
	if(!pdbi->query(fmt::format("HINCRBY " OURO_TABLE_PERFIX "_accountinfos:{} numlogin", name), false))
	{
		transaction.rollback();
		return false;
	}
	
	// Return exists if the query fails, avoiding possible errors
	if(!pdbi->query(fmt::format("HSET " OURO_TABLE_PERFIX "_accountinfos:{} lasttime {}", name, time(NULL)), false))
	{
		transaction.rollback();
		return false;
	}
	
	transaction.commit();
	
	redisReply* pRedisReply = transaction.pRedisReply();

	return pRedisReply && pRedisReply->type == REDIS_REPLY_ARRAY && pRedisReply->elements == 2;
}

//-------------------------------------------------------------------------------------
bool OUROAccountTableRedis::updatePassword(DBInterface * pdbi, const std::string& name, const std::string& password)
{
	// Return exists if the query fails, avoiding possible errors
	if(!pdbi->query(fmt::format("HSET " OURO_TABLE_PERFIX "_accountinfos:{} password {}", name, password), false))
		return false;

	return true;
}

//-------------------------------------------------------------------------------------
bool OUROAccountTableRedis::logAccount(DBInterface * pdbi, ACCOUNT_INFOS& info)
{
	std::string sqlstr = fmt::format("HSET " OURO_TABLE_PERFIX "_accountinfos:{} accountName {} password {} bindata {} ",
		"email {} entityDBID {} flags {} deadline {} regtime {} lasttime {}", 
		info.name, OURO_MD5::getDigest(info.password.data(), info.password.length()).c_str(),
		info.datas, info.email, info.dbid, info.flags, info.deadline, time(NULL), time(NULL));

	// Return exists if the query fails, avoiding possible errors
	if(!pdbi->query(sqlstr.c_str(), sqlstr.size(), false))
	{
		ERROR_MSG(fmt::format("OUROAccountTableRedis::logAccount({}): cmd({}) is failed({})!\n", 
				info.name, sqlstr, pdbi->getstrerror()));

		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------
OUROEmailVerificationTableRedis::OUROEmailVerificationTableRedis(EntityTables* pEntityTables) :
OUROEmailVerificationTable(pEntityTables)
{
}
	
//-------------------------------------------------------------------------------------
OUROEmailVerificationTableRedis::~OUROEmailVerificationTableRedis()
{
}

//-------------------------------------------------------------------------------------
bool OUROEmailVerificationTableRedis::queryAccount(DBInterface * pdbi, int8 type, const std::string& name, ACCOUNT_INFOS& info)
{
	/*
	ouro_email_verification:code = hashes(accountName, type, datas, logtime)
	ouro_email_verification:accountName = code
	*/
	redisReply* pRedisReply = NULL;

	if (!static_cast<DBInterfaceRedis*>(pdbi)->query(fmt::format("GET " OURO_TABLE_PERFIX "_email_verification:{}", name), &pRedisReply, false))
		return false;
	
	info.datas = "";
	
	if(pRedisReply)
	{
		if(pRedisReply->type == REDIS_REPLY_STRING)
		{
			info.datas = pRedisReply->str;
		}
		
		freeReplyObject(pRedisReply); 
		pRedisReply = NULL;
	}

	if(info.datas.size() > 0)
	{
		if (!static_cast<DBInterfaceRedis*>(pdbi)->query(fmt::format("HMGET " OURO_TABLE_PERFIX "_email_verification:{} datas", info.datas), &pRedisReply, false))
			return false;
	}
	else
	{
		ERROR_MSG(fmt::format("OUROEmailVerificationTableMysql::queryAccount({}): cmd({}) is failed({})!\n", 
				name, pdbi->lastquery(), pdbi->getstrerror()));
				
		return false;
	}

	if(pRedisReply)
	{
		if(pRedisReply->type == REDIS_REPLY_STRING)
		{
			info.password = pRedisReply->str;
		}
		
		freeReplyObject(pRedisReply); 
		pRedisReply = NULL;
	}
	else
	{
		ERROR_MSG(fmt::format("OUROEmailVerificationTableMysql::queryAccount({}): cmd({}) is failed({})!\n", 
				name, pdbi->lastquery(), pdbi->getstrerror()));
				
		return false;
	}

	info.name = name;

	return info.datas.size() > 0;
}

//-------------------------------------------------------------------------------------
int OUROEmailVerificationTableRedis::getDeadline(int8 type)
{
	int deadline = 3600;
	if(type == (int8)OUROEmailVerificationTable::V_TYPE_CREATEACCOUNT)
		deadline = g_ouroSrvConfig.emailAtivationInfo_.deadline;
	else if(type == (int8)OUROEmailVerificationTable::V_TYPE_RESETPASSWORD)
		deadline = g_ouroSrvConfig.emailResetPasswordInfo_.deadline;
	else if(type == (int8)OUROEmailVerificationTable::V_TYPE_BIND_MAIL)
		deadline = g_ouroSrvConfig.emailBindInfo_.deadline;
	
	return deadline;
}

//-------------------------------------------------------------------------------------
bool OUROEmailVerificationTableRedis::logAccount(DBInterface * pdbi, int8 type, const std::string& name, 
												const std::string& datas, const std::string& code)
{
	/*
	ouro_email_verification:code = hashes(accountName, type, datas, logtime)
	ouro_email_verification:accountName = code
	*/
	
	redis::DBTransaction transaction(pdbi);
	
	// Return exists if the query fails, avoiding possible errors
	if(!pdbi->query(fmt::format("HSET " OURO_TABLE_PERFIX "_email_verification:{} accountName {} type {} datas {} logtime {}", 
		code, name, type, datas, time(NULL)), false))
	{
		ERROR_MSG(fmt::format("OUROEmailVerificationTableMysql::logAccount({}): cmd({}) is failed({})!\n", 
				code, pdbi->lastquery(), pdbi->getstrerror()));
		
		transaction.rollback();
		return false;
	}

	// Return exists if the query fails, avoiding possible errors
	if(!pdbi->query(fmt::format("SET " OURO_TABLE_PERFIX "_email_verification:{} {}", name, code), false))
	{
		ERROR_MSG(fmt::format("OUROEmailVerificationTableMysql::logAccount({}): cmd({}) is failed({})!\n", 
				code, pdbi->lastquery(), pdbi->getstrerror()));	
		
		transaction.rollback();	
		return false;
	}

	// Return exists if the query fails, avoiding possible errors
	if(!pdbi->query(fmt::format("EXPIRE " OURO_TABLE_PERFIX "_email_verification:{} {}", 
		code.c_str(), getDeadline(type)), false))
	{
		ERROR_MSG(fmt::format("OUROEmailVerificationTableMysql::logAccount({}): cmd({}) is failed({})!\n", 
				code, pdbi->lastquery(), pdbi->getstrerror()));
		
		transaction.rollback();	
		return false;
	}
	
	// Return exists if the query fails, avoiding possible errors
	if(!pdbi->query(fmt::format("EXPIRE " OURO_TABLE_PERFIX "_email_verification:{} {}", 
		name.c_str(), getDeadline(type)), false))
	{
		ERROR_MSG(fmt::format("OUROEmailVerificationTableMysql::logAccount({}): cmd({}) is failed({})!\n", 
				code, pdbi->lastquery(), pdbi->getstrerror()));
		
		transaction.rollback();	
		return false;
	}
	
	transaction.commit();
	
	redisReply* pRedisReply = transaction.pRedisReply();

	return pRedisReply && pRedisReply->type == REDIS_REPLY_ARRAY && pRedisReply->elements == 4;
}

//-------------------------------------------------------------------------------------
bool OUROEmailVerificationTableRedis::activateAccount(DBInterface * pdbi, const std::string& code, ACCOUNT_INFOS& info)
{
	/*
	ouro_email_verification:code = hashes(accountName, type, datas, logtime)
	ouro_email_verification:accountName = code
	*/	
	redisReply* pRedisReply = NULL;

	if (!static_cast<DBInterfaceRedis*>(pdbi)->query(fmt::format("HMGET " OURO_TABLE_PERFIX "_email_verification:{} accountName type, datas logtime", code), &pRedisReply, false))
		return false;

	uint64 logtime = 1;
	int type = -1;
	
	if(pRedisReply)
	{
		if(pRedisReply->type == REDIS_REPLY_ARRAY)
		{
			if(RedisHelper::check_array_results(pRedisReply))
			{			
				info.name = pRedisReply->element[0]->str;
				StringConv::str2value(type, pRedisReply->element[1]->str);
				info.password = pRedisReply->element[2]->str;
				StringConv::str2value(logtime, pRedisReply->element[3]->str);
			}
		}
		
		freeReplyObject(pRedisReply); 
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
	
	if((int)OUROEmailVerificationTable::V_TYPE_CREATEACCOUNT != type)
	{
		ERROR_MSG(fmt::format("OUROEmailVerificationTableMysql::activateAccount({}): type({}) error!\n", 
				code, type));

		return false;
	}

	std::string password = info.password;

	// Find if dblog has this account
	OUROAccountTable* pTable = static_cast<OUROAccountTable*>(EntityTables::findByInterfaceName(pdbi->name()).findOUROTable(OURO_TABLE_PERFIX "_accountinfos"));
	OURO_ASSERT(pTable);
	
	info.flags = 0;
	if(!pTable->queryAccount(pdbi, info.name, info))
	{
		return false;
	}

	if((info.flags & ACCOUNT_FLAG_NOT_ACTIVATED) <= 0)
	{
		ERROR_MSG(fmt::format("OUROEmailVerificationTableRedis::activateAccount({}): Has been activated, flags={}.\n", 
				code, info.flags));

		return false;
	}

	info.flags &= ~ACCOUNT_FLAG_NOT_ACTIVATED; 

	if(!pTable->setFlagsDeadline(pdbi, info.name, info.flags, info.deadline))
	{
		ERROR_MSG(fmt::format("OUROEmailVerificationTableRedis::activateAccount({}): set deadline error({})!\n", 
				code, pdbi->getstrerror()));
		return false;
	}

	if(!pTable->updatePassword(pdbi, info.name, password))
	{
		ERROR_MSG(fmt::format("OUROEmailVerificationTableRedis::activateAccount({}): update password error({})!\n", 
				code, pdbi->getstrerror()));

		return false;
	}

	info.dbid = 0;

	ScriptDefModule* pModule = EntityDef::findScriptModule(DBUtil::accountScriptName());

	// Prevent multithreading problems, make a copy here.
	MemoryStream copyAccountDefMemoryStream(pTable->accountDefMemoryStream());

	info.dbid = EntityTables::findByInterfaceName(pdbi->name()).writeEntity(pdbi, 0, -1,
			&copyAccountDefMemoryStream, pModule);

	OURO_ASSERT(info.dbid > 0);

	/*
	ouro_accountinfos:accountName = hashes(password, bindata, email, entityDBID, flags, deadline, regtime, lasttime, numlogin)
	*/
	
	// Return exists if the query fails, avoiding possible errors
	if(!pdbi->query(fmt::format("HSET " OURO_TABLE_PERFIX "_accountinfos:{} entityDBID {}", 
		info.name, info.dbid), false))
	{
		ERROR_MSG(fmt::format("OUROEmailVerificationTableRedis::activateAccount({}): update " OURO_TABLE_PERFIX "_accountinfos error({})!\n", 
				code, pdbi->getstrerror()));

		return false;
	}

	delAccount(pdbi, (int8)V_TYPE_CREATEACCOUNT, info.name);
	return true;
}

//-------------------------------------------------------------------------------------
bool OUROEmailVerificationTableRedis::bindEMail(DBInterface * pdbi, const std::string& name, const std::string& code)
{
	/*
	ouro_email_verification:code = hashes(accountName, type, datas, logtime)
	ouro_email_verification:accountName = code
	*/	
	redisReply* pRedisReply = NULL;

	if (!pdbi->query(fmt::format("HMGET " OURO_TABLE_PERFIX "_email_verification:{} accountName type, datas logtime", code), false))
	{
		ERROR_MSG(fmt::format("OUROEmailVerificationTableRedis::bindEMail({}): cmd({}) is failed({})!\n", 
				code, pdbi->lastquery(), pdbi->getstrerror()));
	}	

	uint64 logtime = 1;
	int type = -1;
	std::string qname, qemail;
	
	if(pRedisReply)
	{
		if(pRedisReply->type == REDIS_REPLY_ARRAY)
		{
			if(RedisHelper::check_array_results(pRedisReply))
			{			
				qname = pRedisReply->element[0]->str;
				StringConv::str2value(type, pRedisReply->element[1]->str);
				qemail = pRedisReply->element[2]->str;
				StringConv::str2value(logtime, pRedisReply->element[3]->str);
			}
		}
		
		freeReplyObject(pRedisReply); 
	}

	if(logtime > 0 && time(NULL) - logtime > g_ouroSrvConfig.emailBindInfo_.deadline)
	{
		ERROR_MSG(fmt::format("OUROEmailVerificationTableRedis::bindEMail({}): is expired! {} > {}.\n", 
				code, (time(NULL) - logtime), g_ouroSrvConfig.emailBindInfo_.deadline));

		return false;
	}

	if(qname.size() == 0 || qemail.size() == 0)
	{
		ERROR_MSG(fmt::format("OUROEmailVerificationTableRedis::bindEMail({}): name or email is NULL.\n", 
				code));

		return false;
	}
	
	if(qemail != name)
	{
		WARNING_MSG(fmt::format("OUROEmailVerificationTableRedis::bindEMail: code({}) username({}:{}, {}) not match.\n" 
			, code, name, qname, qemail));

		return false;
	}

	if((int)OUROEmailVerificationTable::V_TYPE_BIND_MAIL != type)
	{
		ERROR_MSG(fmt::format("OUROEmailVerificationTableMysql::bindEMail({}): type({}) error!\n", 
				code, type));

		return false;
	}
	
	/*
	ouro_accountinfos:accountName = hashes(password, bindata, email, entityDBID, flags, deadline, regtime, lasttime, numlogin)
	*/
	
	// Return exists if the query fails, avoiding possible errors
	if(!pdbi->query(fmt::format("HSET " OURO_TABLE_PERFIX "_accountinfos:{} email {}", 
		qname, qemail), false))
	{
		ERROR_MSG(fmt::format("OUROEmailVerificationTableRedis::bindEMail({}): update " OURO_TABLE_PERFIX "_accountinfos({}) error({})!\n", 
				code, qname, pdbi->getstrerror()));

		return false;
	}

	delAccount(pdbi, (int8)V_TYPE_BIND_MAIL, name);
	return true;
}

//-------------------------------------------------------------------------------------
bool OUROEmailVerificationTableRedis::resetpassword(DBInterface * pdbi, const std::string& name, 
												   const std::string& password, const std::string& code)
{
	/*
	ouro_email_verification:code = hashes(accountName, type, datas, logtime)
	ouro_email_verification:accountName = code
	*/	
	redisReply* pRedisReply = NULL;

	if (!pdbi->query(fmt::format("HMGET " OURO_TABLE_PERFIX "_email_verification:{} accountName type, datas logtime", code), false))
	{
		ERROR_MSG(fmt::format("OUROEmailVerificationTableRedis::bindEMail({}): cmd({}) is failed({})!\n", 
				code, pdbi->lastquery(), pdbi->getstrerror()));
	}	

	uint64 logtime = 1;
	int type = -1;
	std::string qname, qemail;
	
	if(pRedisReply)
	{
		if(pRedisReply->type == REDIS_REPLY_ARRAY)
		{
			if(RedisHelper::check_array_results(pRedisReply))
			{			
				qname = pRedisReply->element[0]->str;
				StringConv::str2value(type, pRedisReply->element[1]->str);
				qemail = pRedisReply->element[2]->str;
				StringConv::str2value(logtime, pRedisReply->element[3]->str);
			}
		}
		
		freeReplyObject(pRedisReply); 
	}

	if(logtime > 0 && time(NULL) - logtime > g_ouroSrvConfig.emailResetPasswordInfo_.deadline)
	{
		ERROR_MSG(fmt::format("OUROEmailVerificationTableRedis::resetpassword({}): is expired! {} > {}.\n", 
				code, (time(NULL) - logtime), g_ouroSrvConfig.emailResetPasswordInfo_.deadline));

		return false;
	}

	if(qname.size() == 0 || password.size() == 0)
	{
		ERROR_MSG(fmt::format("OUROEmailVerificationTableRedis::resetpassword({}): name or password is NULL.\n", 
				code));

		return false;
	}

	if(qname != name)
	{
		WARNING_MSG(fmt::format("OUROEmailVerificationTableRedis::resetpassword: code({}) username({} != {}) not match.\n" 
			, code, name, qname));

		return false;
	}

	if((int)OUROEmailVerificationTable::V_TYPE_RESETPASSWORD != type)
	{
		ERROR_MSG(fmt::format("OUROEmailVerificationTableMysql::resetpassword({}): type({}) error!\n", 
				code, type));

		return false;
	}
	
	// Find if dblog has this account
	OUROAccountTable* pTable = static_cast<OUROAccountTable*>(EntityTables::findByInterfaceName(pdbi->name()).findOUROTable(OURO_TABLE_PERFIX "_accountinfos"));
	OURO_ASSERT(pTable);

	if(!pTable->updatePassword(pdbi, qname, OURO_MD5::getDigest(password.data(), password.length())))
	{
		ERROR_MSG(fmt::format("OUROEmailVerificationTableRedis::resetpassword({}): update accountName({}) password error({})!\n", 
				code, qname, pdbi->getstrerror()));

		return false;
	}

	delAccount(pdbi, (int8)V_TYPE_RESETPASSWORD, qname);
	return true;
}

//-------------------------------------------------------------------------------------
bool OUROEmailVerificationTableRedis::delAccount(DBInterface * pdbi, int8 type, const std::string& name)
{
	/*
	ouro_email_verification:code = hashes(accountName, type, datas, logtime)
	ouro_email_verification:accountName = code
	*/
	redisReply* pRedisReply = NULL;
	
	// Return exists if the query fails, avoiding possible errors
	if(!pdbi->query(fmt::format("GET " OURO_TABLE_PERFIX "_email_verification:{}", name), false))
	{
		return false;
	}

	std::string code;
	
	if(pRedisReply)
	{
		if(pRedisReply->type == REDIS_REPLY_STRING)
		{
			code = pRedisReply->str;
		}
		
		freeReplyObject(pRedisReply); 
		pRedisReply = NULL;
	}

	// The transaction starts
	redis::DBTransaction transaction(pdbi);
	
	if(code.size() > 0)
	{
		if (!pdbi->query(fmt::format("DEL " OURO_TABLE_PERFIX "_email_verification:{}", code), false))
			return false;
	
		if (!pdbi->query(fmt::format("DEL " OURO_TABLE_PERFIX "_email_verification:{}", name), false))
			return false;
	}
	else
	{
		ERROR_MSG(fmt::format("OUROEmailVerificationTableMysql::queryAccount({}): cmd({}) is failed({})!\n", 
				name, pdbi->lastquery(), pdbi->getstrerror()));
				
		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------
bool OUROEmailVerificationTableRedis::syncToDB(DBInterface* pdbi)
{
	return true;
}

//-------------------------------------------------------------------------------------
}
