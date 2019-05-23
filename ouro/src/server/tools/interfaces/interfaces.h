// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_INTERFACES_TOOL_H
#define OURO_INTERFACES_TOOL_H

#include "server/ouromain.h"
#include "server/python_app.h"
#include "server/serverconfig.h"
#include "server/callbackmgr.h"	
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

		/*Initialize related interfaces*/
	bool initializeBegin();
	bool inInitialize();
	bool initializeEnd();
	void finalise();
	void onInstallPyModules();
	
	bool initDB();

	virtual void onShutdownBegin();
	virtual void onShutdownEnd();


		/** Network Interface
	Register a newly activated baseapp or cellapp or dbmgr
	Usually a new app is launched, it needs to register itself with some components.
	*/
	virtual void onRegisterNewApp(Network::Channel* pChannel,
		int32 uid,
		std::string& username,
		COMPONENT_TYPE componentType, COMPONENT_ID componentID, COMPONENT_ORDER globalorderID, COMPONENT_ORDER grouporderID,
		uint32 intaddr, uint16 intport, uint32 extaddr, uint16 extport, std::string& extaddrEx);

		/** Network Interface
		Request to create an account
	*/
	void reqCreateAccount(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

		/** Network Interface
		A new user login, need to check the legality
	*/
	void onAccountLogin(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

		/** Network Interface
		Request to wipe client request task
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
	    Request response to login account
	*/
	void accountLoginResponse(std::string commitName, std::string realAccountName, 
		std::string extraDatas, Ouroboros::SERVER_ERROR_CODE errorCode);
	static PyObject* __py_accountLoginResponse(PyObject* self, PyObject* args);

	/** Python callback interface
	    Request to create an account response
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
	
	/**
		Request a database command to dbmgr
	*/
	static PyObject* __py_executeRawDatabaseCommand(PyObject* self, PyObject* args);
	void executeRawDatabaseCommand(const char* datas, uint32 size, PyObject* pycallback, ENTITY_ID eid, const std::string& dbInterfaceName);
	void onExecuteRawDatabaseCommandCB(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	PY_CALLBACKMGR& callbackMgr() { return pyCallbackMgr_; }

protected:
	TimerHandle																mainProcessTimer_;

	// order
	ORDERS																	orders_;

	// All request records, avoiding certain types of repetitive requests.
	REQCREATE_MAP															reqCreateAccount_requests_;
	REQLOGIN_MAP															reqAccountLogin_requests_;

	TelnetServer*															pTelnetServer_;

	PY_CALLBACKMGR															pyCallbackMgr_;

};

}

#endif // OURO_INTERFACES_TOOL_H

