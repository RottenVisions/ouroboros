// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_REDIS_TRANSACTION_HELPER_H
#define OURO_REDIS_TRANSACTION_HELPER_H

#include "hiredis.h"

namespace Ouroboros { 
class DBInterface;
namespace redis {

/**
 */
class DBTransaction
{
public:
	DBTransaction(DBInterface* pdbi, bool autostart = true);
	~DBTransaction();
	
	void start();
	void end();

	void commit();
	void rollback();
	
	bool shouldRetry() const;

	void pdbi(DBInterface* pdbi){ pdbi_ = pdbi; }
	
	redisReply* pRedisReply(){ return pRedisReply_; }

private:
	DBInterface* pdbi_;
	bool committed_;
	bool autostart_;
	redisReply* pRedisReply_;
};

}
}
#endif // OURO_REDIS_TRANSACTION_HELPER_H

