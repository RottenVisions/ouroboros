// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_HTTP_CB_HANDLER_H
#define OURO_HTTP_CB_HANDLER_H

#include "common/common.h"

namespace Ouroboros{
namespace Network{
class EndPoint;
}

class HTTPCBHandler : public Network::InputNotificationHandler
{
public:
	HTTPCBHandler();
	virtual ~HTTPCBHandler();

	Network::EndPoint* pEndPoint(){ return pEndPoint_; }

	void onAccountActivated(std::string& code, bool success);
	void onAccountBindedEmail(std::string& code, bool success);
	void onAccountResetPassword(std::string& code, bool success);

protected:
	
	struct CLIENT
	{
		OUROShared_ptr< Network::EndPoint > endpoint;
		uint8 state;
		std::string code;
	};

	virtual int handleInputNotification( int fd );

	Network::EndPoint* pEndPoint_;

	std::map< int, CLIENT > clients_;
};

}

#endif // OURO_HTTP_CB_HANDLER_H
