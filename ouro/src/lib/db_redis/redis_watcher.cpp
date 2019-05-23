// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#include "redis_watcher.h"
#include "common.h"
#include "common/common.h"
#include "helper/watcher.h"
#include "thread/threadguard.h"
#include "thread/threadmutex.h"
#include "server/serverconfig.h"
#include "common/memorystream.h"
#include "helper/debug_helper.h"

namespace Ouroboros{ 

static Ouroboros::thread::ThreadMutex _g_logMutex;
static OUROUnordered_map< std::string, uint32 > g_querystatistics;
static bool _g_installedWatcher = false;
static bool _g_debug = false;

//-------------------------------------------------------------------------------------
static uint32 watcher_query(std::string cmd)
{
	Ouroboros::thread::ThreadGuard tg(&_g_logMutex);

	OUROUnordered_map< std::string, uint32 >::iterator iter = g_querystatistics.find(cmd);
	if (iter != g_querystatistics.end())
	{
		return iter->second;
	}

	return 0;
}

//-------------------------------------------------------------------------------------

// STRING
static uint32 watcher_APPEND()
{
	return watcher_query("APPEND");
}

static uint32 watcher_BITCOUNT()
{
	return watcher_query("BITCOUNT");
}

static uint32 watcher_BITOP()
{
	return watcher_query("BITOP");
}

static uint32 watcher_DECR()
{
	return watcher_query("DECR");
}

static uint32 watcher_DECRBY()
{
	return watcher_query("DECRBY");
}

static uint32 watcher_GET()
{
	return watcher_query("GET");
}

static uint32 watcher_GETRANGE()
{
	return watcher_query("GETRANGE");
}

static uint32 watcher_GETSET()
{
	return watcher_query("GETSET");
}

static uint32 watcher_GETBIT()
{
	return watcher_query("GETBIT");
}

static uint32 watcher_INCR()
{
	return watcher_query("INCR");
}

static uint32 watcher_INCRBY()
{
	return watcher_query("INCRBY");
}

static uint32 watcher_INCRBYFLOAT()
{
	return watcher_query("INCRBYFLOAT");
}

static uint32 watcher_MGET()
{
	return watcher_query("MGET");
}

static uint32 watcher_MSET()
{
	return watcher_query("MSET");
}

static uint32 watcher_MSETNX()
{
	return watcher_query("MSETNX");
}

static uint32 watcher_PSETEX()
{
	return watcher_query("PSETEX");
}

static uint32 watcher_SET()
{
	return watcher_query("SET");
}

static uint32 watcher_SETBIT()
{
	return watcher_query("SETBIT");
}

static uint32 watcher_SETEX()
{
	return watcher_query("SETEX");
}

static uint32 watcher_SETNX()
{
	return watcher_query("SETNX");
}

static uint32 watcher_SETRANGE()
{
	return watcher_query("SETRANGE");
}

static uint32 watcher_STRLEN()
{
	return watcher_query("STRLEN");
}

// KEY
static uint32 watcher_DEL()
{
	return watcher_query("DEL");
}

static uint32 watcher_DUMP()
{
	return watcher_query("DUMP");
}

static uint32 watcher_EXISTS()
{
	return watcher_query("EXISTS");
}

static uint32 watcher_EXPIRE()
{
	return watcher_query("EXPIRE");
}

static uint32 watcher_EXPIREAT()
{
	return watcher_query("EXPIREAT");
}

static uint32 watcher_KEYS()
{
	return watcher_query("KEYS");
}

static uint32 watcher_MOVE()
{
	return watcher_query("MOVE");
}

static uint32 watcher_OBJECT()
{
	return watcher_query("OBJECT");
}

static uint32 watcher_PEXPIRE()
{
	return watcher_query("PEXPIRE");
}

static uint32 watcher_PEXPIREAT()
{
	return watcher_query("PEXPIREAT");
}

static uint32 watcher_PERSIST()
{
	return watcher_query("PERSIST");
}

static uint32 watcher_PTTL()
{
	return watcher_query("PTTL");
}

static uint32 watcher_RANDOMKEY()
{
	return watcher_query("RANDOMKEY");
}

static uint32 watcher_RENAME()
{
	return watcher_query("RENAME");
}

static uint32 watcher_RENAMENX()
{
	return watcher_query("RENAMENX");
}

static uint32 watcher_RESTORE()
{
	return watcher_query("RESTORE");
}

static uint32 watcher_SORT()
{
	return watcher_query("SORT");
}

static uint32 watcher_TTL()
{
	return watcher_query("TTL");
}

static uint32 watcher_TYPE()
{
	return watcher_query("TYPE");
}

static uint32 watcher_SCAN()
{
	return watcher_query("SCAN");
}

// HASHES
static uint32 watcher_HDEL()
{
	return watcher_query("HDEL");
}

static uint32 watcher_HEXISTS()
{
	return watcher_query("HEXISTS");
}

static uint32 watcher_HGET()
{
	return watcher_query("HGET");
}

static uint32 watcher_HGETALL()
{
	return watcher_query("HGETALL");
}

static uint32 watcher_HINCRBY()
{
	return watcher_query("HINCRBY");
}

static uint32 watcher_HINCRBYFLOAT()
{
	return watcher_query("HINCRBYFLOAT");
}

static uint32 watcher_HKEYS()
{
	return watcher_query("HKEYS");
}

static uint32 watcher_HLEN()
{
	return watcher_query("HLEN");
}

static uint32 watcher_HMGET()
{
	return watcher_query("HMGET");
}

static uint32 watcher_HMSET()
{
	return watcher_query("HMSET");
}

static uint32 watcher_HSET()
{
	return watcher_query("HSET");
}

static uint32 watcher_HSETNX()
{
	return watcher_query("HSETNX");
}

static uint32 watcher_HVALS()
{
	return watcher_query("HVALS");
}

static uint32 watcher_HSCAN()
{
	return watcher_query("HSCAN");
}

// list
static uint32 watcher_BLPOP()
{
	return watcher_query("BLPOP");
}

static uint32 watcher_BRPOP()
{
	return watcher_query("BRPOP");
}

static uint32 watcher_BRPOPLPUSH()
{
	return watcher_query("BRPOPLPUSH");
}

static uint32 watcher_LINDEX()
{
	return watcher_query("LINDEX");
}

static uint32 watcher_LINSERT()
{
	return watcher_query("LINSERT");
}

static uint32 watcher_LLEN()
{
	return watcher_query("LLEN");
}

static uint32 watcher_LPOP()
{
	return watcher_query("LPOP");
}

static uint32 watcher_LPUSH()
{
	return watcher_query("LPUSH");
}

static uint32 watcher_LPUSHX()
{
	return watcher_query("LPUSHX");
}

static uint32 watcher_LRANGE()
{
	return watcher_query("LRANGE");
}

static uint32 watcher_LREM()
{
	return watcher_query("LREM");
}

static uint32 watcher_LSET()
{
	return watcher_query("LSET");
}

static uint32 watcher_LTRIM()
{
	return watcher_query("LTRIM");
}

static uint32 watcher_RPOP()
{
	return watcher_query("RPOP");
}

static uint32 watcher_RPOPLPUSH()
{
	return watcher_query("RPOPLPUSH");
}

static uint32 watcher_RPUSH()
{
	return watcher_query("RPUSH");
}

static uint32 watcher_RPUSHX()
{
	return watcher_query("RPUSHX");
}

// sort
static uint32 watcher_SADD()
{
	return watcher_query("SADD");
}

static uint32 watcher_SCARD()
{
	return watcher_query("SCARD");
}

static uint32 watcher_SDIFF()
{
	return watcher_query("SDIFF");
}

static uint32 watcher_SDIFFSTORE()
{
	return watcher_query("SDIFFSTORE");
}

static uint32 watcher_SINTER()
{
	return watcher_query("SINTER");
}

static uint32 watcher_SINTERSTORE()
{
	return watcher_query("SINTERSTORE");
}

static uint32 watcher_SISMEMBER()
{
	return watcher_query("SISMEMBER");
}

static uint32 watcher_SMEMBERS()
{
	return watcher_query("SMEMBERS");
}

static uint32 watcher_SMOVE()
{
	return watcher_query("SMOVE");
}

static uint32 watcher_SPOP()
{
	return watcher_query("SPOP");
}

static uint32 watcher_SRANDMEMBER()
{
	return watcher_query("SRANDMEMBER");
}

static uint32 watcher_SREM()
{
	return watcher_query("SREM");
}

static uint32 watcher_SUNION()
{
	return watcher_query("SUNION");
}

static uint32 watcher_SUNIONSTORE()
{
	return watcher_query("SUNIONSTORE");
}

static uint32 watcher_SSCAN()
{
	return watcher_query("SSCAN");
}

// sorted set
static uint32 watcher_ZADD()
{
	return watcher_query("ZADD");
}

static uint32 watcher_ZCARD()
{
	return watcher_query("ZCARD");
}

static uint32 watcher_ZCOUNT()
{
	return watcher_query("ZCOUNT");
}

static uint32 watcher_ZINCRBY()
{
	return watcher_query("ZINCRBY");
}

static uint32 watcher_ZRANGE()
{
	return watcher_query("ZRANGE");
}

static uint32 watcher_ZRANGEBYSCORE()
{
	return watcher_query("ZRANGEBYSCORE");
}

static uint32 watcher_ZRANK()
{
	return watcher_query("ZRANK");
}

static uint32 watcher_ZREM()
{
	return watcher_query("ZREM");
}

static uint32 watcher_ZREMRANGEBYRANK()
{
	return watcher_query("ZREMRANGEBYRANK");
}

static uint32 watcher_ZREMRANGEBYSCORE()
{
	return watcher_query("ZREMRANGEBYSCORE");
}

static uint32 watcher_ZREVRANGE()
{
	return watcher_query("ZREVRANGE");
}

static uint32 watcher_ZREVRANGEBYSCORE()
{
	return watcher_query("ZREVRANGEBYSCORE");
}

static uint32 watcher_ZREVRANK()
{
	return watcher_query("ZREVRANK");
}

static uint32 watcher_ZSCORE()
{
	return watcher_query("ZSCORE");
}

static uint32 watcher_ZUNIONSTORE()
{
	return watcher_query("ZUNIONSTORE");
}

static uint32 watcher_ZINTERSTORE()
{
	return watcher_query("ZINTERSTORE");
}

static uint32 watcher_ZSCAN()
{
	return watcher_query("ZSCAN");
}

// pub/sub
static uint32 watcher_PSUBSCRIBE()
{
	return watcher_query("PSUBSCRIBE");
}

static uint32 watcher_PUBLISH()
{
	return watcher_query("PUBLISH");
}

static uint32 watcher_PUBSUB()
{
	return watcher_query("PUBSUB");
}

static uint32 watcher_PUNSUBSCRIBE()
{
	return watcher_query("PUNSUBSCRIBE");
}

static uint32 watcher_SUBSCRIBE()
{
	return watcher_query("SUBSCRIBE");
}

static uint32 watcher_UNSUBSCRIBE()
{
	return watcher_query("UNSUBSCRIBE");
}

// transaction
static uint32 watcher_DISCARD()
{
	return watcher_query("DISCARD");
}

static uint32 watcher_EXEC()
{
	return watcher_query("EXEC");
}

static uint32 watcher_MULTI()
{
	return watcher_query("MULTI");
}

static uint32 watcher_UNWATCH()
{
	return watcher_query("UNWATCH");
}

static uint32 watcher_WATCH()
{
	return watcher_query("WATCH");
}

// script
static uint32 watcher_EVAL()
{
	return watcher_query("EVAL");
}

static uint32 watcher_EVALSHA()
{
	return watcher_query("EVALSHA");
}

static uint32 watcher_SCRIPT()
{
	return watcher_query("SCRIPT");
}

// Connection (connection)
static uint32 watcher_AUTH()
{
	return watcher_query("AUTH");
}

static uint32 watcher_ECHO()
{
	return watcher_query("ECHO");
}

static uint32 watcher_PING()
{
	return watcher_query("PING");
}

static uint32 watcher_QUIT()
{
	return watcher_query("QUIT");
}

static uint32 watcher_SELECT()
{
	return watcher_query("SELECT");
}

// Server (server)
static uint32 watcher_BGREWRITEAOF()
{
	return watcher_query("BGREWRITEAOF");
}

static uint32 watcher_BGSAVE()
{
	return watcher_query("BGSAVE");
}

static uint32 watcher_CLIENT()
{
	return watcher_query("CLIENT");
}

static uint32 watcher_CONFIG()
{
	return watcher_query("CONFIG");
}

static uint32 watcher_DBSIZE()
{
	return watcher_query("DBSIZE");
}

static uint32 watcher_DEBUG()
{
	return watcher_query("DEBUG");
}

static uint32 watcher_FLUSHALL()
{
	return watcher_query("FLUSHALL");
}

static uint32 watcher_FLUSHDB()
{
	return watcher_query("FLUSHDB");
}

static uint32 watcher_INFO()
{
	return watcher_query("INFO");
}

static uint32 watcher_LASTSAVE()
{
	return watcher_query("LASTSAVE");
}

static uint32 watcher_MONITOR()
{
	return watcher_query("MONITOR");
}

static uint32 watcher_PSYNC()
{
	return watcher_query("PSYNC");
}

static uint32 watcher_SAVE()
{
	return watcher_query("SAVE");
}

static uint32 watcher_SHUTDOWN()
{
	return watcher_query("SHUTDOWN");
}

static uint32 watcher_SLAVEOF()
{
	return watcher_query("SLAVEOF");
}

static uint32 watcher_SLOWLOG()
{
	return watcher_query("SLOWLOG");
}

static uint32 watcher_SYNC()
{
	return watcher_query("SYNC");
}

static uint32 watcher_TIME()
{
	return watcher_query("TIME");
}

//-------------------------------------------------------------------------------------
void RedisWatcher::initializeWatcher()
{
	if(_g_installedWatcher)
		return;

	_g_installedWatcher = true;
	_g_debug = g_ouroSrvConfig.getDBMgr().debugDBMgr;
	
	// string
	WATCH_OBJECT("db_querys/APPEND", &Ouroboros::watcher_APPEND);
	WATCH_OBJECT("db_querys/BITCOUNT", &Ouroboros::watcher_BITCOUNT);
	WATCH_OBJECT("db_querys/BITOP", &Ouroboros::watcher_BITOP);
	WATCH_OBJECT("db_querys/DECR", &Ouroboros::watcher_DECR);
	WATCH_OBJECT("db_querys/DECRBY", &Ouroboros::watcher_DECRBY);
	WATCH_OBJECT("db_querys/GET", &Ouroboros::watcher_GET);
	WATCH_OBJECT("db_querys/GETRANGE", &Ouroboros::watcher_GETRANGE);
	WATCH_OBJECT("db_querys/GETSET", &Ouroboros::watcher_GETSET);
	WATCH_OBJECT("db_querys/GETBIT", &Ouroboros::watcher_GETBIT);
	WATCH_OBJECT("db_querys/INCR", &Ouroboros::watcher_INCR);
	WATCH_OBJECT("db_querys/INCRBY", &Ouroboros::watcher_INCRBY);
	WATCH_OBJECT("db_querys/INCRBYFLOAT", &Ouroboros::watcher_INCRBYFLOAT);
	WATCH_OBJECT("db_querys/MGET", &Ouroboros::watcher_MGET);
	WATCH_OBJECT("db_querys/MSET", &Ouroboros::watcher_MSET);
	WATCH_OBJECT("db_querys/MSETNX", &Ouroboros::watcher_MSETNX);
	WATCH_OBJECT("db_querys/PSETEX", &Ouroboros::watcher_PSETEX);
	WATCH_OBJECT("db_querys/SET", &Ouroboros::watcher_SET);
	WATCH_OBJECT("db_querys/SETBIT", &Ouroboros::watcher_SETBIT);
	WATCH_OBJECT("db_querys/SETEX", &Ouroboros::watcher_SETEX);
	WATCH_OBJECT("db_querys/SETNX", &Ouroboros::watcher_SETNX);
	WATCH_OBJECT("db_querys/SETRANGE", &Ouroboros::watcher_SETRANGE);
	WATCH_OBJECT("db_querys/STRLEN", &Ouroboros::watcher_STRLEN);
	
	// keys
	WATCH_OBJECT("db_querys/DEL", &Ouroboros::watcher_DEL);
	WATCH_OBJECT("db_querys/DUMP", &Ouroboros::watcher_DUMP);
	WATCH_OBJECT("db_querys/EXISTS", &Ouroboros::watcher_EXISTS);
	WATCH_OBJECT("db_querys/EXPIRE", &Ouroboros::watcher_EXPIRE);
	WATCH_OBJECT("db_querys/EXPIREAT", &Ouroboros::watcher_EXPIREAT);
	WATCH_OBJECT("db_querys/KEYS", &Ouroboros::watcher_KEYS);
	WATCH_OBJECT("db_querys/MOVE", &Ouroboros::watcher_MOVE);
	WATCH_OBJECT("db_querys/OBJECT", &Ouroboros::watcher_OBJECT);
	WATCH_OBJECT("db_querys/PEXPIRE", &Ouroboros::watcher_PEXPIRE);
	WATCH_OBJECT("db_querys/PEXPIREAT", &Ouroboros::watcher_PEXPIREAT);	
	WATCH_OBJECT("db_querys/PERSIST", &Ouroboros::watcher_PERSIST);
	WATCH_OBJECT("db_querys/PTTL", &Ouroboros::watcher_PTTL);	
	WATCH_OBJECT("db_querys/RANDOMKEY", &Ouroboros::watcher_RANDOMKEY);
	WATCH_OBJECT("db_querys/RENAME", &Ouroboros::watcher_RENAME);
	WATCH_OBJECT("db_querys/RENAMENX", &Ouroboros::watcher_RENAMENX);
	WATCH_OBJECT("db_querys/RESTORE", &Ouroboros::watcher_RESTORE);
	WATCH_OBJECT("db_querys/SORT", &Ouroboros::watcher_SORT);
	WATCH_OBJECT("db_querys/TTL", &Ouroboros::watcher_TTL);
	WATCH_OBJECT("db_querys/TYPE", &Ouroboros::watcher_TYPE);
	WATCH_OBJECT("db_querys/SCAN", &Ouroboros::watcher_SCAN);
	
	// heshes
	WATCH_OBJECT("db_querys/HDEL", &Ouroboros::watcher_HDEL);
	WATCH_OBJECT("db_querys/HEXISTS", &Ouroboros::watcher_HEXISTS);
	WATCH_OBJECT("db_querys/HGET", &Ouroboros::watcher_HGET);
	WATCH_OBJECT("db_querys/HGETALL", &Ouroboros::watcher_HGETALL);
	WATCH_OBJECT("db_querys/HINCRBY", &Ouroboros::watcher_HINCRBY);
	WATCH_OBJECT("db_querys/HINCRBYFLOAT", &Ouroboros::watcher_HINCRBYFLOAT);
	WATCH_OBJECT("db_querys/HKEYS", &Ouroboros::watcher_HKEYS);
	WATCH_OBJECT("db_querys/HLEN", &Ouroboros::watcher_HLEN);
	WATCH_OBJECT("db_querys/HMGET", &Ouroboros::watcher_HMGET);
	WATCH_OBJECT("db_querys/HMSET", &Ouroboros::watcher_HMSET);
	WATCH_OBJECT("db_querys/HSET", &Ouroboros::watcher_HSET);
	WATCH_OBJECT("db_querys/HSETNX", &Ouroboros::watcher_HSETNX);
	WATCH_OBJECT("db_querys/HVALS", &Ouroboros::watcher_HVALS);
	WATCH_OBJECT("db_querys/HSCAN", &Ouroboros::watcher_HSCAN);	
	
	// list
	WATCH_OBJECT("db_querys/BLPOP", &Ouroboros::watcher_BLPOP);
	WATCH_OBJECT("db_querys/BRPOP", &Ouroboros::watcher_BRPOP);
	WATCH_OBJECT("db_querys/BRPOPLPUSH", &Ouroboros::watcher_BRPOPLPUSH);
	WATCH_OBJECT("db_querys/LINDEX", &Ouroboros::watcher_LINDEX);
	WATCH_OBJECT("db_querys/LINSERT", &Ouroboros::watcher_LINSERT);
	WATCH_OBJECT("db_querys/LLEN", &Ouroboros::watcher_LLEN);
	WATCH_OBJECT("db_querys/LPOP", &Ouroboros::watcher_LPOP);
	WATCH_OBJECT("db_querys/LPUSH", &Ouroboros::watcher_LPUSH);
	WATCH_OBJECT("db_querys/LPUSHX", &Ouroboros::watcher_LPUSHX);
	WATCH_OBJECT("db_querys/LRANGE", &Ouroboros::watcher_LRANGE);
	WATCH_OBJECT("db_querys/LREM", &Ouroboros::watcher_LREM);
	WATCH_OBJECT("db_querys/LSET", &Ouroboros::watcher_LSET);
	WATCH_OBJECT("db_querys/LTRIM", &Ouroboros::watcher_LTRIM);
	WATCH_OBJECT("db_querys/RPOP", &Ouroboros::watcher_RPOP);
	WATCH_OBJECT("db_querys/RPOPLPUSH", &Ouroboros::watcher_RPOPLPUSH);
	WATCH_OBJECT("db_querys/RPUSH", &Ouroboros::watcher_RPUSH);
	WATCH_OBJECT("db_querys/RPUSHX", &Ouroboros::watcher_RPUSHX);
	
	// set
	WATCH_OBJECT("db_querys/SADD", &Ouroboros::watcher_SADD);
	WATCH_OBJECT("db_querys/SCARD", &Ouroboros::watcher_SCARD);
	WATCH_OBJECT("db_querys/SDIFF", &Ouroboros::watcher_SDIFF);
	WATCH_OBJECT("db_querys/SDIFFSTORE", &Ouroboros::watcher_SDIFFSTORE);
	WATCH_OBJECT("db_querys/SINTER", &Ouroboros::watcher_SINTER);
	WATCH_OBJECT("db_querys/SINTERSTORE", &Ouroboros::watcher_SINTERSTORE);
	WATCH_OBJECT("db_querys/SISMEMBER", &Ouroboros::watcher_SISMEMBER);
	WATCH_OBJECT("db_querys/SMEMBERS", &Ouroboros::watcher_SMEMBERS);
	WATCH_OBJECT("db_querys/SMOVE", &Ouroboros::watcher_SMOVE);
	WATCH_OBJECT("db_querys/SPOP", &Ouroboros::watcher_SPOP);
	WATCH_OBJECT("db_querys/SRANDMEMBER", &Ouroboros::watcher_SRANDMEMBER);
	WATCH_OBJECT("db_querys/SREM", &Ouroboros::watcher_SREM);
	WATCH_OBJECT("db_querys/SUNION", &Ouroboros::watcher_SUNION);
	WATCH_OBJECT("db_querys/SUNIONSTORE", &Ouroboros::watcher_SUNIONSTORE);
	WATCH_OBJECT("db_querys/SSCAN", &Ouroboros::watcher_SSCAN);
	
	// sortedSet
	WATCH_OBJECT("db_querys/ZADD", &Ouroboros::watcher_ZADD);
	WATCH_OBJECT("db_querys/ZCARD", &Ouroboros::watcher_ZCARD);
	WATCH_OBJECT("db_querys/ZCOUNT", &Ouroboros::watcher_ZCOUNT);
	WATCH_OBJECT("db_querys/ZINCRBY", &Ouroboros::watcher_ZINCRBY);
	WATCH_OBJECT("db_querys/ZRANGE", &Ouroboros::watcher_ZRANGE);
	WATCH_OBJECT("db_querys/ZRANGEBYSCORE", &Ouroboros::watcher_ZRANGEBYSCORE);
	WATCH_OBJECT("db_querys/ZRANK", &Ouroboros::watcher_ZRANK);
	WATCH_OBJECT("db_querys/ZREM", &Ouroboros::watcher_ZREM);
	WATCH_OBJECT("db_querys/ZREMRANGEBYRANK", &Ouroboros::watcher_ZREMRANGEBYRANK);
	WATCH_OBJECT("db_querys/ZREMRANGEBYSCORE", &Ouroboros::watcher_ZREMRANGEBYSCORE);
	WATCH_OBJECT("db_querys/ZREVRANGE", &Ouroboros::watcher_ZREVRANGE);
	WATCH_OBJECT("db_querys/ZREVRANGEBYSCORE", &Ouroboros::watcher_ZREVRANGEBYSCORE);
	WATCH_OBJECT("db_querys/ZREVRANK", &Ouroboros::watcher_ZREVRANK);
	WATCH_OBJECT("db_querys/ZSCORE", &Ouroboros::watcher_ZSCORE);
	WATCH_OBJECT("db_querys/ZUNIONSTORE", &Ouroboros::watcher_ZUNIONSTORE);
	WATCH_OBJECT("db_querys/ZINTERSTORE", &Ouroboros::watcher_ZINTERSTORE);
	WATCH_OBJECT("db_querys/ZSCAN", &Ouroboros::watcher_ZSCAN);
	
	// pub/sub
	WATCH_OBJECT("db_querys/PSUBSCRIBE", &Ouroboros::watcher_PSUBSCRIBE);
	WATCH_OBJECT("db_querys/PUBLISH", &Ouroboros::watcher_PUBLISH);
	WATCH_OBJECT("db_querys/PUBSUB", &Ouroboros::watcher_PUBSUB);
	WATCH_OBJECT("db_querys/PUNSUBSCRIBE", &Ouroboros::watcher_PUNSUBSCRIBE);
	WATCH_OBJECT("db_querys/SUBSCRIBE", &Ouroboros::watcher_SUBSCRIBE);
	WATCH_OBJECT("db_querys/UNSUBSCRIBE", &Ouroboros::watcher_UNSUBSCRIBE);
	
	// Transaction
	WATCH_OBJECT("db_querys/DISCARD", &Ouroboros::watcher_DISCARD);
	WATCH_OBJECT("db_querys/EXEC", &Ouroboros::watcher_EXEC);
	WATCH_OBJECT("db_querys/MULTI", &Ouroboros::watcher_MULTI);
	WATCH_OBJECT("db_querys/UNWATCH", &Ouroboros::watcher_UNWATCH);
	WATCH_OBJECT("db_querys/WATCH", &Ouroboros::watcher_WATCH);
	
	// scrit
	WATCH_OBJECT("db_querys/EVAL", &Ouroboros::watcher_EVAL);
	WATCH_OBJECT("db_querys/EVALSHA", &Ouroboros::watcher_EVALSHA);
	WATCH_OBJECT("db_querys/SCRIPT", &Ouroboros::watcher_SCRIPT);	

	// Connection (connection)
	WATCH_OBJECT("db_querys/AUTH", &Ouroboros::watcher_AUTH);
	WATCH_OBJECT("db_querys/ECHO", &Ouroboros::watcher_ECHO);
	WATCH_OBJECT("db_querys/PING", &Ouroboros::watcher_PING);
	WATCH_OBJECT("db_querys/QUIT", &Ouroboros::watcher_QUIT);
	WATCH_OBJECT("db_querys/SELECT", &Ouroboros::watcher_SELECT);
	
	// Server (server)
	WATCH_OBJECT("db_querys/BGREWRITEAOF", &Ouroboros::watcher_BGREWRITEAOF);
	WATCH_OBJECT("db_querys/BGSAVE", &Ouroboros::watcher_BGSAVE);
	WATCH_OBJECT("db_querys/CLIENT", &Ouroboros::watcher_CLIENT);
	WATCH_OBJECT("db_querys/CONFIG", &Ouroboros::watcher_CONFIG);
	WATCH_OBJECT("db_querys/DBSIZE", &Ouroboros::watcher_DBSIZE);
	WATCH_OBJECT("db_querys/DEBUG", &Ouroboros::watcher_DEBUG);
	WATCH_OBJECT("db_querys/FLUSHALL", &Ouroboros::watcher_FLUSHALL);
	WATCH_OBJECT("db_querys/FLUSHDB", &Ouroboros::watcher_FLUSHDB);
	WATCH_OBJECT("db_querys/INFO", &Ouroboros::watcher_INFO);
	WATCH_OBJECT("db_querys/LASTSAVE", &Ouroboros::watcher_LASTSAVE);
	WATCH_OBJECT("db_querys/MONITOR", &Ouroboros::watcher_MONITOR);
	WATCH_OBJECT("db_querys/PSYNC", &Ouroboros::watcher_PSYNC);
	WATCH_OBJECT("db_querys/SAVE", &Ouroboros::watcher_SAVE);
	WATCH_OBJECT("db_querys/SHUTDOWN", &Ouroboros::watcher_SHUTDOWN);
	WATCH_OBJECT("db_querys/SELECT", &Ouroboros::watcher_SLAVEOF);
	WATCH_OBJECT("db_querys/SLOWLOG", &Ouroboros::watcher_SLOWLOG);
	WATCH_OBJECT("db_querys/SYNC", &Ouroboros::watcher_SYNC);
	WATCH_OBJECT("db_querys/TIME", &Ouroboros::watcher_TIME);				
}

//-------------------------------------------------------------------------------------
void RedisWatcher::querystatistics(const char* strCommand, uint32 size)
{
	std::string op;
	for(uint32 i=0; i<size; ++i)
	{
		if(strCommand[i] == ' ')
			break;

		op += strCommand[i];
	}

	if(op.size() == 0)
		return;

	std::transform(op.begin(), op.end(), op.begin(), toupper);

	_g_logMutex.lockMutex();

	OUROUnordered_map< std::string, uint32 >::iterator iter = g_querystatistics.find(op);
	if(iter == g_querystatistics.end())
	{
		g_querystatistics[op] = 1;
	}
	else
	{
		iter->second += 1;
	}

	_g_logMutex.unlockMutex();
}

//-------------------------------------------------------------------------------------

}

