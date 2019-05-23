// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_DBMGR_H
#define OURO_DBMGR_H

#include "db_interface/db_threadpool.h"
#include "buffered_dbtasks.h"
#include "server/ouromain.h"
#include "pyscript/script.h"
#include "pyscript/pyobject_pointer.h"
#include "entitydef/entitydef.h"
#include "server/python_app.h"
#include "server/idallocate.h"
#include "server/serverconfig.h"
#include "server/globaldata_client.h"
#include "server/globaldata_server.h"
#include "server/callbackmgr.h"	
#include "common/timer.h"
#include "network/endpoint.h"
#include "resmgr/resmgr.h"
#include "thread/threadpool.h"


namespace Ouroboros{

class DBInterface;
class TelnetServer;
class InterfacesHandler;
class SyncAppDatasHandler;
class UpdateDBServerLogHandler;

class Dbmgr :	public PythonApp, 
				public Singleton<Dbmgr>
{
public:
	enum TimeOutType
	{
		TIMEOUT_TICK = TIMEOUT_PYTHONAPP_MAX + 1,
		TIMEOUT_CHECK_STATUS
	};
	
	Dbmgr(Network::EventDispatcher& dispatcher, 
		Network::NetworkInterface& ninterface, 
		COMPONENT_TYPE componentType,
		COMPONENT_ID componentID);

	~Dbmgr();
	
	bool run();
	
	void handleTimeout(TimerHandle handle, void * arg);
	void handleMainTick();
	void handleCheckStatusTick();

		/*Initialize related interfaces*/
	bool initializeBegin();
	bool inInitialize();
	bool initializeEnd();
	void finalise();

	bool installPyModules();
	bool uninstallPyModules();
	void onInstallPyModules();
	
	bool initInterfacesHandler();

	bool initDB();

	virtual ShutdownHandler::CAN_SHUTDOWN_STATE canShutdown();

	virtual void onShutdownBegin();
	virtual void onShutdownEnd();

		/** Get ID server pointer*/
	IDServer<ENTITY_ID>& idServer(void){ return idServer_; }

		/** Network Interface
		Request to assign an ENTITY_ID segment
	*/
	void onReqAllocEntityID(Network::Channel* pChannel, COMPONENT_ORDER componentType, COMPONENT_ID componentID);

		/* Network Interface
		Register a newly activated baseapp or cellapp or dbmgr
		Usually a new app is launched, it needs to register itself with some components.
	*/
	virtual void onRegisterNewApp(Network::Channel* pChannel, 
							int32 uid, 
							std::string& username, 
							COMPONENT_TYPE componentType, COMPONENT_ID componentID, COMPONENT_ORDER globalorderID, COMPONENT_ORDER grouporderID,
							uint32 intaddr, uint16 intport, uint32 extaddr, uint16 extport, std::string& extaddrEx);


		/** Network Interface
		Dbmgr broadcast changes to global data
	*/
	void onGlobalDataClientLogon(Network::Channel* pChannel, COMPONENT_TYPE componentType);
	void onBroadcastGlobalDataChanged(Network::Channel* pChannel, Ouroboros::MemoryStream& s);
	
		/** Network Interface
		Request to create an account
	*/
	void reqCreateAccount(Network::Channel* pChannel, Ouroboros::MemoryStream& s);
	void onCreateAccountCBFromInterfaces(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

		/** Network Interface
		Request to wipe client request task
	*/
	void eraseClientReq(Network::Channel* pChannel, std::string& logkey);

		/** Network Interface
		A new user login, need to check the legality
	*/
	void onAccountLogin(Network::Channel* pChannel, Ouroboros::MemoryStream& s);
	void onLoginAccountCBBFromInterfaces(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

		/** Network Interface
		Baseapp request to query account information
	*/
	void queryAccount(Network::Channel* pChannel, std::string& accountName, std::string& password, bool needCheckPassword,
		COMPONENT_ID componentID, ENTITY_ID entityID, DBID entityDBID, uint32 ip, uint16 port);

		/** Network Interface
		Entity autoloading
	*/
	void entityAutoLoad(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

		/** Network Interface
		The account is online from baseapp
	*/
	void onAccountOnline(Network::Channel* pChannel, std::string& accountName, 
		COMPONENT_ID componentID, ENTITY_ID entityID);

		/** Network Interface
		Entity-baseapp is offline
	*/
	void onEntityOffline(Network::Channel* pChannel, DBID dbid, ENTITY_SCRIPT_UID sid, uint16 dbInterfaceIndex);

		/** Network Interface
		Perform database query
	*/
	void executeRawDatabaseCommand(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

		/** Network Interface
		An entity archive
	*/
	void writeEntity(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

		/** Network Interface
		Delete archive data for an entity
	*/
	void removeEntity(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

		/** Network Interface
		Remove the callback of an entity from the database via dbid
	*/
	void deleteEntityByDBID(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

		/** Network Interface
		Query whether an entity is checked out from the database by dbid
	*/
	void lookUpEntityByDBID(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

		/** Network Interface
		Request to get all data of entity from db
	*/
	void queryEntity(Network::Channel* pChannel, uint16 dbInterfaceIndex, COMPONENT_ID componentID, int8	queryMode, DBID dbid, 
		std::string& entityType, CALLBACK_ID callbackID, ENTITY_ID entityID);

		/** Network Interface
		Synchronous entity flow template
	*/
	void syncEntityStreamTemplate(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	virtual bool initializeWatcher();

		/** Network Interface
		Request recharge
	*/
	void charge(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

		/** Network Interface
		Recharge callback
	*/
	void onChargeCB(Network::Channel* pChannel, Ouroboros::MemoryStream& s);


		/** Network Interface
		Activate callback
	*/
	void accountActivate(Network::Channel* pChannel, std::string& scode);

		/** Network Interface
		Account reset password
	*/
	void accountReqResetPassword(Network::Channel* pChannel, std::string& accountName);
	void accountResetPassword(Network::Channel* pChannel, std::string& accountName, 
		std::string& newpassword, std::string& code);

		/** Network Interface
		Account binding mailbox
	*/
	void accountReqBindMail(Network::Channel* pChannel, ENTITY_ID entityID, std::string& accountName, 
		std::string& password, std::string& email);
	void accountBindMail(Network::Channel* pChannel, std::string& username, std::string& scode);

		/** Network Interface
		Account change password
	*/
	void accountNewPassword(Network::Channel* pChannel, ENTITY_ID entityID, std::string& accountName, 
		std::string& password, std::string& newpassword);
	
	SyncAppDatasHandler* pSyncAppDatasHandler() const { return pSyncAppDatasHandler_; }
	void pSyncAppDatasHandler(SyncAppDatasHandler* p){ pSyncAppDatasHandler_ = p; }

	std::string selectAccountDBInterfaceName(const std::string& name);

	Buffered_DBTasks* findBufferedDBTask(const std::string& dbInterfaceName)
	{
		BUFFERED_DBTASKS_MAP::iterator dbin_iter = bufferedDBTasksMaps_.find(dbInterfaceName);
		if (dbin_iter == bufferedDBTasksMaps_.end())
			return NULL;

		return &dbin_iter->second;
	}

	virtual void onChannelDeregister(Network::Channel * pChannel);

	InterfacesHandler* findBestInterfacesHandler();

	/**
		Request a database command to dbmgr
	*/
	static PyObject* __py_executeRawDatabaseCommand(PyObject* self, PyObject* args);
	void executeRawDatabaseCommand(const char* datas, uint32 size, PyObject* pycallback, ENTITY_ID eid, const std::string& dbInterfaceName);
	void onExecuteRawDatabaseCommandCB(Ouroboros::MemoryStream& s);

	PY_CALLBACKMGR& callbackMgr() { return pyCallbackMgr_; }

protected:
	TimerHandle											loopCheckTimerHandle_;
	TimerHandle											mainProcessTimer_;

	// entityID allocation server
	IDServer<ENTITY_ID>									idServer_;

	// globalData
	GlobalDataServer*									pGlobalData_;

	// baseAppData
	GlobalDataServer*									pBaseAppData_;

	// cellAppData
	GlobalDataServer*									pCellAppData_;

	typedef KBEUnordered_map<std::string, Buffered_DBTasks> BUFFERED_DBTASKS_MAP;
	BUFFERED_DBTASKS_MAP								bufferedDBTasksMaps_;

	// Statistics
	uint32												numWrittenEntity_;
	uint32												numRemovedEntity_;
	uint32												numQueryEntity_;
	uint32												numExecuteRawDatabaseCommand_;
	uint32												numCreatedAccount_;

	std::vector<InterfacesHandler*>						pInterfacesHandlers_;

	SyncAppDatasHandler*								pSyncAppDatasHandler_;
	UpdateDBServerLogHandler*							pUpdateDBServerLogHandler_;
	
	TelnetServer*										pTelnetServer_;

	std::map<COMPONENT_ID, uint64>						loseBaseappts_;

	PY_CALLBACKMGR										pyCallbackMgr_;
};

}

#endif // OURO_DBMGR_H
