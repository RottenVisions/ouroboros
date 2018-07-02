// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com


#ifndef OURO_BASEAPP_H
#define OURO_BASEAPP_H

// common include
#include "entity.h"
#include "proxy.h"
#include "profile.h"
#include "server/entity_app.h"
#include "server/pendingLoginmgr.h"
#include "server/forward_messagebuffer.h"
#include "network/endpoint.h"

//#define NDEBUG
// windows include
#if OURO_PLATFORM == PLATFORM_WIN32
#else
// linux include
#endif

namespace Ouroboros{

namespace Network{
	class Channel;
	class Bundle;
}

class Proxy;
class Backuper;
class Archiver;
class TelnetServer;
class RestoreEntityHandler;
class InitProgressHandler;

class Baseapp :	public EntityApp<Entity>,
				public Singleton<Baseapp>
{
public:
	enum TimeOutType
	{
		TIMEOUT_CHECK_STATUS = TIMEOUT_ENTITYAPP_MAX + 1,
		TIMEOUT_MAX
	};

	Baseapp(Network::EventDispatcher& dispatcher,
		Network::NetworkInterface& ninterface,
		COMPONENT_TYPE componentType,
		COMPONENT_ID componentID);

	~Baseapp();

	virtual bool installPyModules();
	virtual void onInstallPyModules();
	virtual bool uninstallPyModules();

	bool run();

	/**
		Related processing interface
	*/
	virtual void handleTimeout(TimerHandle handle, void * arg);
	virtual void handleGameTick();
	void handleCheckStatusTick();
	void handleBackup();
	void handleArchive();

	/**
		Initialize related interfaces
	*/
	bool initializeBegin();
	bool initializeEnd();
	void finalise();

	virtual bool canShutdown();
	virtual void onShutdownBegin();
	virtual void onShutdown(bool first);
	virtual void onShutdownEnd();

	virtual bool initializeWatcher();

	static PyObject* __py_quantumPassedPercent(PyObject* self, PyObject* args);
	float _getLoad() const { return getLoad(); }
	virtual void onUpdateLoad();

	virtual void onChannelDeregister(Network::Channel * pChannel);

	/**
		A cellapp died
	*/
	void onCellAppDeath(Network::Channel * pChannel);

	/** Network Interface
		Dbmgr tells the address of other baseapp or cellapp that has been started
		Current app needs to actively establish a connection with them
	*/
	virtual void onGetEntityAppFromDbmgr(Network::Channel* pChannel,
							int32 uid,
							std::string& username,
							COMPONENT_TYPE componentType, COMPONENT_ID componentID, COMPONENT_ORDER globalorderID, COMPONENT_ORDER grouporderID,
							uint32 intaddr, uint16 intport, uint32 extaddr, uint16 extport, std::string& extaddrEx);

	/** Network Interface
		A client informs the app that it is active.
	*/
	void onClientActiveTick(Network::Channel* pChannel);

	/** Network Interface
		Automatic entity load information retrieved from the database returns
	*/
	void onEntityAutoLoadCBFromDBMgr(Network::Channel* pChannel, MemoryStream& s);

	/**
		Created an entity callback
	*/
	virtual Entity* onCreateEntity(PyObject* pyEntity, ScriptDefModule* sm, ENTITY_ID eid);

	/**
		Create an entity
	*/
	static PyObject* __py_createEntity(PyObject* self, PyObject* args);
	static PyObject* __py_createEntityAnywhere(PyObject* self, PyObject* args);
	static PyObject* __py_createEntityRemotely(PyObject* self, PyObject* args);
	static PyObject* __py_createEntityFromDBID(PyObject* self, PyObject* args);
	static PyObject* __py_createEntityAnywhereFromDBID(PyObject* self, PyObject* args);
	static PyObject* __py_createEntityRemotelyFromDBID(PyObject* self, PyObject* args);

	/**
		Create a new space
	*/
	void createCellEntityInNewSpace(Entity* pEntity, PyObject* pyCellappIndex);

	/**
		Restore a space
	*/
	void restoreSpaceInCell(Entity* pEntity);

	/**
		Create a baseEntity on a lower-loaded baseapp
	*/
	void createEntityAnywhere(const char* entityType, PyObject* params, PyObject* pyCallback);

	/** Received baseappmgr decided to request a baseapp request createEntityAnywhere executed on this baseapp
		@param entityType	: The category of entity, defined in entities.xml.
		@param strInitData	: After the entity is created, some data that should be initialized for him needs to be unwrapped using pickle.loads.
		@param componentID	: The component ID of the baseapp requesting to create an entity
	*/
	void onCreateEntityAnywhere(Network::Channel* pChannel, MemoryStream& s);

	/**
	baseapp createEntityAnywhere callbacks
	*/
	void onCreateEntityAnywhereCallback(Network::Channel* pChannel, Ouroboros::MemoryStream& s);
	void _onCreateEntityAnywhereCallback(Network::Channel* pChannel, CALLBACK_ID callbackID,
		std::string& entityType, ENTITY_ID eid, COMPONENT_ID componentID);

	/**
	Create a baseEntity on a lower-loaded baseapp
	*/
	void createEntityRemotely(const char* entityType, COMPONENT_ID componentID, PyObject* params, PyObject* pyCallback);

	/** Received baseappmgr decided to request a baseapp request createEntityAnywhere executed on this baseapp
	@param entityType	: The category of entity, defined in entities.xml.
	@param strInitData	: After the entity is created, some data that should be initialized for him needs to be unwrapped using pickle.loads.
	@param componentID	: The component ID of the baseapp requesting to create an entity
	*/
	void onCreateEntityRemotely(Network::Channel* pChannel, MemoryStream& s);

	/**
	baseapp createEntityAnywhere callbacks
	*/
	void onCreateEntityRemotelyCallback(Network::Channel* pChannel, Ouroboros::MemoryStream& s);
	void _onCreateEntityRemotelyCallback(Network::Channel* pChannel, CALLBACK_ID callbackID,
		std::string& entityType, ENTITY_ID eid, COMPONENT_ID componentID);

	/**
		Create an entity by obtaining information from db
	*/
	void createEntityFromDBID(const char* entityType, DBID dbid, PyObject* pyCallback, const std::string& dbInterfaceName);

	/** Network Interface
		Callback for createEntityFromDBID.
	*/
	void onCreateEntityFromDBIDCallback(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	/**
		Create an entity by obtaining information from db
	*/
	void createEntityAnywhereFromDBID(const char* entityType, DBID dbid, PyObject* pyCallback, const std::string& dbInterfaceName);

	/** Network Interface
		Callback for createEntityAnywhereFromDBID.
	*/
	// Query component id callback for creating entity from baseappmgr
	void onGetCreateEntityAnywhereFromDBIDBestBaseappID(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	/** Network Interface
		Callback for createEntityAnywhereFromDBID.
	*/
	// Callbacks from the database
	void onCreateEntityAnywhereFromDBIDCallback(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	// Request to create this entity on this process
	void createEntityAnywhereFromDBIDOtherBaseapp(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	// Callback after creation
	void onCreateEntityAnywhereFromDBIDOtherBaseappCallback(Network::Channel* pChannel, COMPONENT_ID createByBaseappID,
							std::string entityType, ENTITY_ID createdEntityID, CALLBACK_ID callbackID, DBID dbid);

	/**
	Create an entity by obtaining information from db
	*/
	void createEntityRemotelyFromDBID(const char* entityType, DBID dbid, COMPONENT_ID createToComponentID,
		PyObject* pyCallback, const std::string& dbInterfaceName);

	/** Network Interface
	Callback for createEntityRemotelyFromDBID.
	*/
	// Callbacks from the database
	void onCreateEntityRemotelyFromDBIDCallback(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	// Request to create this entity on this process
	void createEntityRemotelyFromDBIDOtherBaseapp(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	// Callback after creation
	void onCreateEntityRemotelyFromDBIDOtherBaseappCallback(Network::Channel* pChannel, COMPONENT_ID createByBaseappID,
		std::string entityType, ENTITY_ID createdEntityID, CALLBACK_ID callbackID, DBID dbid);

	/**
		Create a cellEntity on a specified cell for a baseEntity
	*/
	void createCellEntity(EntityCallAbstract* createToCellEntityCall, Entity* pEntity);

	/** Network Interface
		createCellEntity failed callback.
	*/
	void onCreateCellFailure(Network::Channel* pChannel, ENTITY_ID entityID);

	/** Network Interface
		The createCellEntity cell entity creates a successful callback.
	*/
	void onEntityGetCell(Network::Channel* pChannel, ENTITY_ID id, COMPONENT_ID componentID, SPACE_ID spaceID);

	/**
		Inform the client to create a proxy corresponding entity
	*/
	bool createClientProxies(Proxy* pEntity, bool reload = false);

	/**
		Execute a database command to dbmgr request
	*/
	static PyObject* __py_executeRawDatabaseCommand(PyObject* self, PyObject* args);
	void executeRawDatabaseCommand(const char* datas, uint32 size, PyObject* pycallback, ENTITY_ID eid, const std::string& dbInterfaceName);
	void onExecuteRawDatabaseCommandCB(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	/** Network Interface
		Dbmgr sends initial information
		startID: Initial allocation of ENTITY_ID segment starting position
		endID: Initial allocation of ENTITY_ID segment end position
		startGlobalOrder: Global startup sequence Includes a variety of different components
		startGroupOrder: Group startup sequence, For example, the first few start in all baseapp.
		machineGroupOrder: Real group order in the machine, Provides a low level to use when determining if it is the first baseapp
	*/
	void onDbmgrInitCompleted(Network::Channel* pChannel,
		GAME_TIME gametime, ENTITY_ID startID, ENTITY_ID endID, COMPONENT_ORDER startGlobalOrder,
		COMPONENT_ORDER startGroupOrder, const std::string& digest);

	/** Network Interface
		Dbmgr broadcast global data changes
	*/
	void onBroadcastBaseAppDataChanged(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	/** Network Interface
		Register the account to be logged in. After registering, login to this gateway is allowed.
	*/
	void registerPendingLogin(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	/** Network Interface
		New user requests to log in to the gateway
	*/
	void loginBaseapp(Network::Channel* pChannel, std::string& accountName, std::string& password);

	/**
		Kicking out a Channel
	*/
	void kickChannel(Network::Channel* pChannel, SERVER_ERROR_CODE failedcode);

	/** Network Interface
		Log in again quickly to establish an interactive relationship with the gateway (if you have logged in before,
		Then the disconnection can quickly establish a connection with the server and achieve the purpose of controlling the entity if the server determines that the Entity of the front end does not time out and destroy.)
	*/
	void reloginBaseapp(Network::Channel* pChannel, std::string& accountName,
		std::string& password, uint64 key, ENTITY_ID entityID);

	/**
	   Login failed
	   @failedcode: Failed return code NETWORK_ERR_SRV_NO_READY: The server is not ready
									NETWORK_ERR_ILLEGAL_LOGIN:Illegal login,
									NETWORK_ERR_NAME_PASSWORD:incorrect username or password
	*/
	void loginBaseappFailed(Network::Channel* pChannel, std::string& accountName,
		SERVER_ERROR_CODE failedcode, bool relogin = false);

	/** Network Interface
		Get account Entity information from dbmgr
	*/
	void onQueryAccountCBFromDbmgr(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	/**
		The client itself enters the world
	*/
	void onClientEntityEnterWorld(Proxy* pEntity, COMPONENT_ID componentID);

	/** Network Interface
		The entity receives a remote call request, initiated by an entityCall on the app (only used internally by the server, the client's entitycall call method
		onRemoteCellMethodCallFromClient)
	*/
	void onEntityCall(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	/** Network Interface
		Client access entity cell method
	*/
	void onRemoteCallCellMethodFromClient(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	/** Network Interface
		Client update data
	*/
	void onUpdateDataFromClient(Network::Channel* pChannel, Ouroboros::MemoryStream& s);
	void onUpdateDataFromClientForControlledEntity(Network::Channel* pChannel, Ouroboros::MemoryStream& s);


	/** Network Interface
		Cellapp backup cell data of entity
	*/
	void onBackupEntityCellData(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	/** Network Interface
		cellapp writeToDB complete
	*/
	void onCellWriteToDBCompleted(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	/** Network Interface
		Cellapp forwards entity message to client
	*/
	void forwardMessageToClientFromCellapp(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	/** Network Interface
		Cellapp forwards the entity message to the cellEntity of a baseEntity
	*/
	void forwardMessageToCellappFromCellapp(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	/**
		Get game time
	*/
	static PyObject* __py_gametime(PyObject* self, PyObject* args);

	/** Network Interface
		Write entity to db callback
	*/
	void onWriteToDBCallback(Network::Channel* pChannel, ENTITY_ID eid, DBID entityDBID,
		uint16 dbInterfaceIndex, CALLBACK_ID callbackID, bool success);

	/**
		Increase proxices count
	*/
	void incProxicesCount() { ++numProxices_; }

	/**
		Reduce proxices count
	*/
	void decProxicesCount() { --numProxices_; }

	/**
		Get proxices count
	*/
	int32 numProxices() const { return numProxices_; }

	/**
		Get numClients count
	*/
	int32 numClients() { return this->networkInterface().numExtChannels(); }

	/**
		Request recharge
	*/
	static PyObject* __py_charge(PyObject* self, PyObject* args);
	void charge(std::string chargeID, DBID dbid, const std::string& datas, PyObject* pycallback);
	void onChargeCB(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	/**
		hook entitycallcall
	*/
	RemoteEntityMethod* createEntityCallCallEntityRemoteMethod(MethodDescription* pMethodDescription, EntityCallAbstract* pEntityCall);

	virtual void onHello(Network::Channel* pChannel,
		const std::string& verInfo,
		const std::string& scriptVerInfo,
		const std::string& encryptedKey);

	// Engine version does not match
	virtual void onVersionNotMatch(Network::Channel* pChannel);

	// Engine script layer version does not match
	virtual void onScriptVersionNotMatch(Network::Channel* pChannel);

	/** Network Interface
		Request to return results on other APP disaster recovery
	*/
	void onRequestRestoreCB(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	/**
		A cell's entity is restored
	*/
	void onRestoreEntitiesOver(RestoreEntityHandler* pRestoreEntityHandler);

	/** Network Interface
		The space on a baseapp restores the cell. Determines whether the current baseapp has related entities and needs to restore the cell.
	*/
	void onRestoreSpaceCellFromOtherBaseapp(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	/** Network Interface
		An app requests to view the app
	*/
	virtual void lookApp(Network::Channel* pChannel);

	/** Network Interface
		Client protocol export
	*/
	void importClientMessages(Network::Channel* pChannel);

	/** Network Interface
		Client entitydef export
	*/
	void importClientEntityDef(Network::Channel* pChannel);

	/**
		Re-import all scripts
	*/
	static PyObject* __py_reloadScript(PyObject* self, PyObject* args);
	virtual void reloadScript(bool fullReload);
	virtual void onReloadScript(bool fullReload);

	/**
		Get process is shutting down
	*/
	static PyObject* __py_isShuttingDown(PyObject* self, PyObject* args);

	/**
		Get process internal network address
	*/
	static PyObject* __py_address(PyObject* self, PyObject* args);

	/**
		Delete an entity from the database by dbid

		Delete the entity from the database, if the entity is not online, you can directly delete the callback to return true. If you are online, the callback returns the entity's entityCall, and any other reason returns false.
	*/
	static PyObject* __py_deleteEntityByDBID(PyObject* self, PyObject* args);

	/** Network Interface
		Remove the callback of an entity from the database by dbid
	*/
	void deleteEntityByDBIDCB(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	/**
		Check if an entity is checked out from the database by dbid

		If the entity returns a baseentitycall on an online callback, the callback returns true if the entity is offline and false for any other reason..
	*/
	static PyObject* __py_lookUpEntityByDBID(PyObject* self, PyObject* args);

	/** Network Interface
		If the entity returns a baseentitycall on an online callback, the callback returns true if the entity is offline and false for any other reason..
	*/
	void lookUpEntityByDBIDCB(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	/** Network Interface
		Request binding email
	*/
	void reqAccountBindEmail(Network::Channel* pChannel, ENTITY_ID entityID, std::string& password, std::string& email);

	/** Network Interface
		Request binding email, dbmgr returns results
	*/
	void onReqAccountBindEmailCBFromDBMgr(Network::Channel* pChannel, ENTITY_ID entityID, std::string& accountName, std::string& email,
		SERVER_ERROR_CODE failedcode, std::string& code);

	/** Network Interface
		Request binding email, baseappmgr returns the address of the need to find loginapp
	*/
	void onReqAccountBindEmailCBFromBaseappmgr(Network::Channel* pChannel, ENTITY_ID entityID, std::string& accountName, std::string& email,
		SERVER_ERROR_CODE failedcode, std::string& code, std::string& loginappCBHost, uint16 loginappCBPort);

	/** Network Interface
		Request binding email
	*/
	void reqAccountNewPassword(Network::Channel* pChannel, ENTITY_ID entityID, std::string& oldpassworld, std::string& newpassword);

	void onReqAccountNewPasswordCB(Network::Channel* pChannel, ENTITY_ID entityID, std::string& accountName,
		SERVER_ERROR_CODE failedcode);

	uint32 flags() const { return flags_; }
	void flags(uint32 v) { flags_ = v; }
	static PyObject* __py_setFlags(PyObject* self, PyObject* args);
	static PyObject* __py_getFlags(PyObject* self, PyObject* args);

protected:
	TimerHandle												loopCheckTimerHandle_;

	// globalBases
	GlobalDataClient*										pBaseAppData_;

	// Log accounts that have logged in to the server but have not been processed yet
	PendingLoginMgr											pendingLoginMgr_;

	ForwardComponent_MessageBuffer							forward_messagebuffer_;

	// Backup archive related
	OUROShared_ptr< Backuper >								pBackuper_;
	OUROShared_ptr< Archiver >								pArchiver_;

	int32													numProxices_;

	TelnetServer*											pTelnetServer_;

	std::vector< OUROShared_ptr< RestoreEntityHandler > >	pRestoreEntityHandlers_;

	TimerHandle												pResmgrTimerHandle_;

	InitProgressHandler*									pInitProgressHandler_;

	// APP logo
	uint32													flags_;

	// Dynamic import of entitydef protocol for clients
	Network::Bundle*										pBundleImportEntityDefDatas_;
};

}

#endif // OURO_BASEAPP_H
