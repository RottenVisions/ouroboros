// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_MYSQL_TRANSACTION_HELPER_H
#define OURO_MYSQL_TRANSACTION_HELPER_H

namespace Ouroboros { 
class DBInterface;
namespace mysql {

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

	bool shouldRetry() const;

	void pdbi(DBInterface* pdbi){ pdbi_ = pdbi; }
	
private:
	DBInterface* pdbi_;
	bool committed_;
	bool autostart_;
};

}
}
#endif // OURO_MYSQL_TRANSACTION_HELPER_H

