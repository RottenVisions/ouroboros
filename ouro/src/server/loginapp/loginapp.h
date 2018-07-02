// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com


#ifndef OURO_LOGINAPP_H
#define OURO_LOGINAPP_H

// common include
#include "server/ouromain.h"
#include "server/serverapp.h"
#include "server/idallocate.h"
#include "server/serverconfig.h"
#include "server/pendingLoginmgr.h"
#include "server/python_app.h"
#include "common/timer.h"
#include "network/endpoint.h"

namespace Ouroboros{

class HTTPCBHandler;
class TelnetServer;

class Loginapp :	public PythonApp,
					public Singleton<Loginapp>
{
public:
	enum TimeOutType
	{
		TIMEOUT_TICK = TIMEOUT_PYTHONAPP_MAX + 1
	};

	Loginapp(Network::EventDispatcher& dispatcher,
		Network::NetworkInterface& ninterface,
		COMPONENT_TYPE componentType,
		COMPONENT_ID componentID);

	~Loginapp();

	bool run();

	virtual void onChannelDeregister(Network::Channel * pChannel);

	virtual void handleTimeout(TimerHandle handle, void * arg);
	void handleMainTick();

	/* Initialize related interfaces */
	bool initializeBegin();
	bool inInitialize();
	bool initializeEnd();
	void finalise();
	void onInstallPyModules();

	virtual void onShutdownBegin();
	virtual void onShutdownEnd();

	virtual void onHello(Network::Channel* pChannel,
		const std::string& verInfo,
		const std::string& scriptVerInfo,
		const std::string& encryptedKey);

	/** Network Interface
		A client informs the app that it is active.
	*/
	void onClientActiveTick(Network::Channel* pChannel);

	/** Network Interface
		Create an account
	*/
	bool _createAccount(Network::Channel* pChannel, std::string& accountName,
		std::string& password, std::string& datas, ACCOUNT_TYPE type = ACCOUNT_TYPE_NORMAL);
	void reqCreateAccount(Network::Channel* pChannel, MemoryStream& s);

	/** Network Interface
		Create an email account
	*/
	void reqCreateMailAccount(Network::Channel* pChannel, MemoryStream& s);

	/** Network Interface
		Create an account
	*/
	void onReqCreateAccountResult(Network::Channel* pChannel, MemoryStream& s);
	void onReqCreateMailAccountResult(Network::Channel* pChannel, MemoryStream& s);

	/** Network Interface
		Reset Account Password Application (Forgot Password?)
	*/
	void reqAccountResetPassword(Network::Channel* pChannel, std::string& accountName);
	void onReqAccountResetPasswordCB(Network::Channel* pChannel, std::string& accountName, std::string& email,
		SERVER_ERROR_CODE failedcode, std::string& code);

	/** Network Interface
		Dbmgr account activation returns
	*/
	void onAccountActivated(Network::Channel* pChannel, std::string& code, bool success);

	/** Network Interface
		Dbmgr account binding email back
	*/
	void onAccountBindedEmail(Network::Channel* pChannel, std::string& code, bool success);

	/** Network Interface
		Dbmgr account reset password return
	*/
	void onAccountResetPassword(Network::Channel* pChannel, std::string& code, bool success);

	/** Network Interface
	Baseapp request binding e-mail (return to need to find loginapp address)
	*/
	void onReqAccountBindEmailAllocCallbackLoginapp(Network::Channel* pChannel, COMPONENT_ID reqBaseappID, ENTITY_ID entityID, std::string& accountName, std::string& email,
		SERVER_ERROR_CODE failedcode, std::string& code);

	/** Network Interface
		User login server
		clientType[COMPONENT_CLIENT_TYPE]: Frontend category (mobile, web, pcexe side)
		clientData[str]: Front end with data (can be arbitrary, such as with the phone model, browser type, etc.)
		accountName[str]: Username
		password[str]: password
	*/
	void login(Network::Channel* pChannel, MemoryStream& s);

	/*
		Login failed
		failedcode: Failed return code NETWORK_ERR_SRV_NO_READY:The server is not ready,
									NETWORK_ERR_SRV_OVERLOAD:Server is overloaded,
									NETWORK_ERR_NAME_PASSWORD:incorrect username or password
	*/
	void _loginFailed(Network::Channel* pChannel, std::string& loginName,
		SERVER_ERROR_CODE failedcode, std::string& datas, bool force = false);

	/** Network Interface
		Login account test results returned by dbmgr
	*/
	void onLoginAccountQueryResultFromDbmgr(Network::Channel* pChannel, MemoryStream& s);

	/** Network Interface
		Login address returned by baseappmgr
	*/
	void onLoginAccountQueryBaseappAddrFromBaseappmgr(Network::Channel* pChannel, std::string& loginName,
		std::string& accountName, std::string& addr, uint16 tcp_port, uint16 udp_port);


	/** Network Interface
		Dbmgr sends initial information
		startGlobalOrder: Global startup sequence includes a variety of different components
		startGroupOrder: The start order in the group, such as the first few start in all baseapps.
	*/
	void onDbmgrInitCompleted(Network::Channel* pChannel, COMPONENT_ORDER startGlobalOrder,
		COMPONENT_ORDER startGroupOrder, const std::string& digest);

	/** Network Interface
		Client protocol export
	*/
	void importClientMessages(Network::Channel* pChannel);

	/** Network Interface
		Error code description export
	*/
	void importServerErrorsDescr(Network::Channel* pChannel);

	// Engine version does not match
	virtual void onVersionNotMatch(Network::Channel* pChannel);

	// Engine script layer version does not match
	virtual void onScriptVersionNotMatch(Network::Channel* pChannel);

	/** Network Interface
		Baseapp synchronizes its own initialization information
		startGlobalOrder: Global startup sequence includes a variety of different components
		startGroupOrder: The start order in the group, such as the first few start in all baseapps.
	*/
	void onBaseappInitProgress(Network::Channel* pChannel, float progress);

protected:
	TimerHandle							mainProcessTimer_;

	// Logs a request for a registered account that has not yet logged in
	PendingLoginMgr						pendingCreateMgr_;

	// Log accounts that have logged in to the server but have not been processed yet
	PendingLoginMgr						pendingLoginMgr_;

	std::string							digest_;

	HTTPCBHandler*						pHttpCBHandler;

	float								initProgress_;

	TelnetServer*						pTelnetServer_;
};

}

#endif // OURO_LOGINAPP_H
