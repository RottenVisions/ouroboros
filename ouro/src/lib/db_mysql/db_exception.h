// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_MYSQL_EXCEPTION_H
#define OURO_MYSQL_EXCEPTION_H

#include <string>

namespace Ouroboros { 

class DBInterface;
class DBException : public std::exception
{
public:
	DBException(DBInterface* pdbi);
	~DBException() throw();

	virtual const char * what() const throw() { return errStr_.c_str(); }

	bool shouldRetry() const;
	bool isLostConnection() const;

	void setError(const std::string& errStr, unsigned int errNum)
	{
		errStr_ = errStr;
		errNum_ = errNum;
	}

private:
	std::string errStr_;
	unsigned int errNum_;
};

}

#endif // OURO_MYSQL_EXCEPTION_H


