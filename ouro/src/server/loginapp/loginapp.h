// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com


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

		/*Initialize related interfaces*/
	bool initializeBegin();
	bool inInitialize();
	bool initializeEnd();
	void finalise();
	void onInstallPyModules();
	
	virtual void onShutdownBegin();
	virtual void onShutdownEnd();

	/** Signal Processing
	*/
	virtual bool installSignals();
	virtual void onSignalled(int sigNum);

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
		Reset account password request (forgot your password?)
	*/
	void reqAccountResetPassword(Network::Channel* pChannel, std::string& accountName);
	void onReqAccountResetPasswordCB(Network::Channel* pChannel, std::string& accountName, std::string& email,
		SERVER_ERROR_CODE failedcode, std::string& code);

		/** Network Interface
		Dbmgr account activation return
	*/
	void onAccountActivated(Network::Channel* pChannel, std::string& code, bool success);

		/** Network Interface
		Dbmgr account binding email return
	*/
	void onAccountBindedEmail(Network::Channel* pChannel, std::string& code, bool success);

		/** Network Interface
		Dbmgr account reset password return
	*/
	void onAccountResetPassword(Network::Channel* pChannel, std::string& code, bool success);

		/** Network Interface
	The baseapp request binds the email (you need to find the address of the loginapp when you return)
	*/
	void onReqAccountBindEmailAllocCallbackLoginapp(Network::Channel* pChannel, COMPONENT_ID reqBaseappID, ENTITY_ID entityID, std::string& accountName, std::string& email,
		SERVER_ERROR_CODE failedcode, std::string& code);

		/** Network Interface
		User login to the server
		clientType[COMPONENT_CLIENT_TYPE]: front-end category (mobile, web, pcexe)
		clientData[str]: Data attached to the front end (can be any, such as the phone model, browser type, etc.)
		accountName[str]: account name
		Password[str]: password
	*/
	void login(Network::Channel* pChannel, MemoryStream& s);

	/*
				Login failed
		Failedcode: failed return code NETWORK_ERR_SRV_NO_READY: The server is not ready,
									NETWORK_ERR_SRV_OVERLOAD: The server is overloaded,
									NETWORK_ERR_NAME_PASSWORD: Username or password is incorrect
	*/
	void _loginFailed(Network::Channel* pChannel, std::string& loginName, 
		SERVER_ERROR_CODE failedcode, std::string& datas, bool force = false);
	
		/** Network Interface
		Login account detection result returned by dbmgr
	*/
	void onLoginAccountQueryResultFromDbmgr(Network::Channel* pChannel, MemoryStream& s);

		/** Network Interface
		Login gateway address returned by baseappmgr
	*/
	void onLoginAccountQueryBaseappAddrFromBaseappmgr(Network::Channel* pChannel, std::string& loginName, 
		std::string& accountName, std::string& addr, uint16 tcp_port, uint16 udp_port);


		/** Network Interface
		Dbmgr sends initial information
		startGlobalOrder: global startup sequence including various components
		startGroupOrder: The startup order within the group, such as the first few starts in all baseapps.
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

		/** Network Interface
	Client SDK export
	*/
	void importClientSDK(Network::Channel* pChannel, MemoryStream& s);

	// Engine version does not match
	virtual void onVersionNotMatch(Network::Channel* pChannel);

	// Engine script layer version does not match
	virtual void onScriptVersionNotMatch(Network::Channel* pChannel);

		/** Network Interface
		Baseapp synchronizes its own initialization information
		startGlobalOrder: global startup sequence including various components
		startGroupOrder: The startup order within the group, such as the first few starts in all baseapps.
	*/
	void onBaseappInitProgress(Network::Channel* pChannel, float progress);

protected:
	TimerHandle							mainProcessTimer_;

	// Record the request that the registered account has not been logged in yet
	PendingLoginMgr						pendingCreateMgr_;

	// Record the account that was logged in to the server but has not been processed yet
	PendingLoginMgr						pendingLoginMgr_;

	std::string							digest_;

	HTTPCBHandler*						pHttpCBHandler;

	float								initProgress_;
	
	TelnetServer*						pTelnetServer_;
};

}

#endif // OURO_LOGINAPP_H
