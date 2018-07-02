// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_INTERFACES_TOOL_H
#define OURO_INTERFACES_TOOL_H

#include "server/ouromain.h"
#include "server/python_app.h"
#include "server/serverconfig.h"
#include "common/timer.h"
#include "network/endpoint.h"
#include "resmgr/resmgr.h"
#include "thread/threadpool.h"

namespace Ouroboros{

class DBInterface;
class Orders;
class CreateAccountTask;
class LoginAccountTask;
class TelnetServer;


class Interfaces :	public PythonApp,
					public Singleton<Interfaces>
{
public:
	enum TimeOutType
	{
		TIMEOUT_TICK = TIMEOUT_PYTHONAPP_MAX + 1
	};

	Interfaces(Network::EventDispatcher& dispatcher,
		Network::NetworkInterface& ninterface,
		COMPONENT_TYPE componentType,
		COMPONENT_ID componentID);

	~Interfaces();

	bool run();

	void handleTimeout(TimerHandle handle, void * arg);
	void handleMainTick();

	/* Initialize related interfaces */
	bool initializeBegin();
	bool inInitialize();
	bool initializeEnd();
	void finalise();
	void onInstallPyModules();

	bool initDB();

	virtual void onShutdownBegin();
	virtual void onShutdownEnd();

	/** Network Interface
		Request to create an account
	*/
	void reqCreateAccount(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	/** Network Interface
		A new user logs in, need to check legality
	*/
	void onAccountLogin(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	/** Network Interface
		Request to erase client request task
	*/
	void eraseClientReq(Network::Channel* pChannel, std::string& logkey);

	/** Network Interface
		Request recharge
	*/
	void charge(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	/** Python callback interface
	    Recharge response
	*/
	void chargeResponse(std::string orderID, std::string extraDatas, Ouroboros::SERVER_ERROR_CODE errorCode);
	static PyObject* __py_chargeResponse(PyObject* self, PyObject* args);

	/** Python callback interface
	    Request login account response
	*/
	void accountLoginResponse(std::string commitName, std::string realAccountName,
		std::string extraDatas, Ouroboros::SERVER_ERROR_CODE errorCode);
	static PyObject* __py_accountLoginResponse(PyObject* self, PyObject* args);

	/** Python callback interface
	    Request to create account's response
	*/
	void createAccountResponse(std::string commitName, std::string realAccountName,
		std::string extraDatas, Ouroboros::SERVER_ERROR_CODE errorCode);
	static PyObject* __py_createAccountResponse(PyObject* self, PyObject* args);

	typedef OUROUnordered_map<std::string, OUROShared_ptr<Orders> > ORDERS;
	Interfaces::ORDERS& orders(){ return orders_; }

	typedef OUROUnordered_map<std::string, CreateAccountTask*> REQCREATE_MAP;
	typedef OUROUnordered_map<std::string, LoginAccountTask*> REQLOGIN_MAP;
	REQCREATE_MAP& reqCreateAccount_requests(){ return reqCreateAccount_requests_; }
	REQLOGIN_MAP& reqAccountLogin_requests(){ return reqAccountLogin_requests_; }

	void eraseOrders(std::string ordersid);
	bool hasOrders(std::string ordersid);

protected:
	TimerHandle																mainProcessTimer_;

	// Orders
	ORDERS																	orders_;

	// All request records, avoid certain types of repetitive requests.
	REQCREATE_MAP															reqCreateAccount_requests_;
	REQLOGIN_MAP															reqAccountLogin_requests_;

	TelnetServer*															pTelnetServer_;

};

}

#endif // OURO_INTERFACES_TOOL_H
