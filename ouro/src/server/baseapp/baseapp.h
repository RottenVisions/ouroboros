// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com


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
	bool initialize();
	bool initializeBegin();
	bool initializeEnd();
	void finalise();
	
	virtual ShutdownHandler::CAN_SHUTDOWN_STATE canShutdown();
	virtual void onShutdownBegin();
	virtual void onShutdown(bool first);
	virtual void onShutdownEnd();

	virtual bool initializeWatcher();

	static PyObject* __py_quantumPassedPercent(PyObject* self, PyObject* args);
	float _getLoad() const { return getLoad(); }
	virtual void onUpdateLoad();

	virtual void onChannelDeregister(Network::Channel * pChannel);

	/**
		Death of a cellapp
	*/
	void onCellAppDeath(Network::Channel * pChannel);

		/** Network Interface
		Dbmgr tells the address of other baseapp or cellapp that has been started
		The current app needs to actively connect with them.
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
		Automatic entity loading information returned in the database
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
		Create a baseEntity on a lower load baseapp
	*/
	void createEntityAnywhere(const char* entityType, PyObject* params, PyObject* pyCallback);

	/** Received a baseappmgr decision to execute a baseapp request createEntityAnywhere request on this baseapp
		@param entityType : The class of the entity, as defined in entities.xml.
		@param strInitData : This entity should be initialized with some data after it is created. It needs to be unpacked using pickle.loads.
		@param componentID : The component ID of the baseapp requesting to create an entity
	*/
	void onCreateEntityAnywhere(Network::Channel* pChannel, MemoryStream& s);

	/**
	Baseapp's createEntityAnywhere callback
	*/
	void onCreateEntityAnywhereCallback(Network::Channel* pChannel, Ouroboros::MemoryStream& s);
	void _onCreateEntityAnywhereCallback(Network::Channel* pChannel, CALLBACK_ID callbackID,
		std::string& entityType, ENTITY_ID eid, COMPONENT_ID componentID);

	/**
	Create a baseEntity on a lower load baseapp
	*/
	void createEntityRemotely(const char* entityType, COMPONENT_ID componentID, PyObject* params, PyObject* pyCallback);

	/** Received a baseappmgr decision to execute a baseapp request createEntityAnywhere request on this baseapp
	@param entityType : The class of the entity, as defined in entities.xml.
	@param strInitData : This entity should be initialized with some data after it is created. It needs to be unpacked using pickle.loads.
	@param componentID : The component ID of the baseapp requesting to create an entity
	*/
	void onCreateEntityRemotely(Network::Channel* pChannel, MemoryStream& s);

	/**
	Baseapp's createEntityAnywhere callback
	*/
	void onCreateEntityRemotelyCallback(Network::Channel* pChannel, Ouroboros::MemoryStream& s);
	void _onCreateEntityRemotelyCallback(Network::Channel* pChannel, CALLBACK_ID callbackID,
		std::string& entityType, ENTITY_ID eid, COMPONENT_ID componentID);

	/** 
		Create an entity from the db to get information
	*/
	void createEntityFromDBID(const char* entityType, DBID dbid, PyObject* pyCallback, const std::string& dbInterfaceName);

		/** Network Interface
		Callback to createEntityFromDBID.
	*/
	void onCreateEntityFromDBIDCallback(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	/** 
		Create an entity from the db to get information
	*/
	void createEntityAnywhereFromDBID(const char* entityType, DBID dbid, PyObject* pyCallback, const std::string& dbInterfaceName);

		/** Network Interface
		Callback to createEntityAnywhereFromDBID.
	*/
	// Query the component id callback used to create the entity from baseappmgr
	void onGetCreateEntityAnywhereFromDBIDBestBaseappID(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

		/** Network Interface
		Callback to createEntityAnywhereFromDBID.
	*/
	// callback from the database
	void onCreateEntityAnywhereFromDBIDCallback(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	// request to create this entity on this process
	void createEntityAnywhereFromDBIDOtherBaseapp(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	// Callback after creation
	void onCreateEntityAnywhereFromDBIDOtherBaseappCallback(Network::Channel* pChannel, COMPONENT_ID createByBaseappID, 
							std::string entityType, ENTITY_ID createdEntityID, CALLBACK_ID callbackID, DBID dbid);
	
	/**
	Create an entity from the db to get information
	*/
	void createEntityRemotelyFromDBID(const char* entityType, DBID dbid, COMPONENT_ID createToComponentID, 
		PyObject* pyCallback, const std::string& dbInterfaceName);

		/** Network Interface
	Callback to createEntityRemotelyFromDBID.
	*/
	// callback from the database
	void onCreateEntityRemotelyFromDBIDCallback(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	// request to create this entity on this process
	void createEntityRemotelyFromDBIDOtherBaseapp(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	// Callback after creation
	void onCreateEntityRemotelyFromDBIDOtherBaseappCallback(Network::Channel* pChannel, COMPONENT_ID createByBaseappID,
		std::string entityType, ENTITY_ID createdEntityID, CALLBACK_ID callbackID, DBID dbid);

	/** 
		Create a cellEntity on the specified cell for a baseEntity
	*/
	void createCellEntity(EntityCallAbstract* createToCellEntityCall, Entity* pEntity);
	
		/** Network Interface
		The callback for createCellEntity failed.
	*/
	void onCreateCellFailure(Network::Channel* pChannel, ENTITY_ID entityID);

		/** Network Interface
		The cell entity of createCellEntity creates a successful callback.
	*/
	void onEntityGetCell(Network::Channel* pChannel, ENTITY_ID id, COMPONENT_ID componentID, SPACE_ID spaceID);

	/** 
		Inform the client to create a proxy corresponding entity
	*/
	bool createClientProxies(Proxy* pEntity, bool reload = false);

	/** 
		Request a database command to dbmgr
	*/
	static PyObject* __py_executeRawDatabaseCommand(PyObject* self, PyObject* args);
	void executeRawDatabaseCommand(const char* datas, uint32 size, PyObject* pycallback, ENTITY_ID eid, const std::string& dbInterfaceName);
	void onExecuteRawDatabaseCommandCB(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

		/** Network Interface
		Dbmgr sends initial information
		startID: initial allocation ENTITY_ID segment start position
		endID: initial allocation ENTITY_ID segment end position
		startGlobalOrder: global startup sequence including various components
		startGroupOrder: The startup order within the group, such as the first few starts in all baseapps.
		machineGroupOrder: The actual group order in the machine, providing the underlying at some point to determine if it is the first baseapp
	*/
	void onDbmgrInitCompleted(Network::Channel* pChannel, 
		GAME_TIME gametime, ENTITY_ID startID, ENTITY_ID endID, COMPONENT_ORDER startGlobalOrder, 
		COMPONENT_ORDER startGroupOrder, const std::string& digest);

		/** Network Interface
		Dbmgr broadcast changes to global data
	*/
	void onBroadcastBaseAppDataChanged(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

		/** Network Interface
		Register the account that will be logged in. After registration, you are allowed to log in to this gateway.
	*/
	void registerPendingLogin(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

		/** Network Interface
		New user requests to log in to the gateway
	*/
	void loginBaseapp(Network::Channel* pChannel, std::string& accountName, std::string& password);

		/** Network Interface
		Log out to the gateway, just disconnect and trigger the entity's onClientDead, the entity is destroyed by the user
	*/
	void logoutBaseapp(Network::Channel* pChannel, uint64 key, ENTITY_ID entityID);

	/**
		Kick out a Channel
	*/
	void kickChannel(Network::Channel* pChannel, SERVER_ERROR_CODE failedcode);

		/** Network Interface
		Re-login Quickly establish an interaction with the gateway (provided that you have logged in before,
		After disconnecting, the server can quickly establish a connection with the server and achieve the purpose of manipulating the entity if the server determines that the front-end Entity has not expired.
	*/
	void reloginBaseapp(Network::Channel* pChannel, std::string& accountName, 
		std::string& password, uint64 key, ENTITY_ID entityID);

	/**
	   	   Login failed
	   @failedcode: Failure return code NETWORK_ERR_SRV_NO_READY: The server is not ready,
									NETWORK_ERR_ILLEGAL_LOGIN: illegal login,
									NETWORK_ERR_NAME_PASSWORD: Username or password is incorrect
	*/
	void loginBaseappFailed(Network::Channel* pChannel, std::string& accountName, 
		SERVER_ERROR_CODE failedcode, bool relogin = false);

		/** Network Interface
		Obtain the account Entity information from dbmgr
	*/
	void onQueryAccountCBFromDbmgr(Network::Channel* pChannel, Ouroboros::MemoryStream& s);
	
	/**
		The client itself has entered the world
	*/
	void onClientEntityEnterWorld(Proxy* pEntity, COMPONENT_ID componentID);

		/** Network Interface
		The entity receives the remote call request, which is initiated by the entityCall on an app (only used internally by the server, the client's entitycall call method is taken
		onRemoteCellMethodCallFromClient)
	*/
	void onEntityCall(Network::Channel* pChannel, Ouroboros::MemoryStream& s);
	
		/** Network Interface
		Client access cell method of entity
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
		Cellapp writeToDB completed
	*/
	void onCellWriteToDBCompleted(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

		/** Network Interface
		Cellapp forwards the entity message to the client
	*/
	void forwardMessageToClientFromCellapp(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

		/** Network Interface
		The cellapp forwards the entity message to the cellEntity of a baseEntity.
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
		Increase the results count
	*/
	void incProxicesCount() { ++numProxices_; }

	/**
		Reduce the proxices count
	*/
	void decProxicesCount() { --numProxices_; }

	/**
		Get the proxices count
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
		Request to return results in other APP disaster recovery
	*/
	void onRequestRestoreCB(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	/**
		The cell's entity has been restored.
	*/
	void onRestoreEntitiesOver(RestoreEntityHandler* pRestoreEntityHandler);

		/** Network Interface
		The space on a baseapp restores the cell, and determines whether the current baseapp has an associated entity and needs to recover the cell.
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
		Get the process is shutting down
	*/
	static PyObject* __py_isShuttingDown(PyObject* self, PyObject* args);

	/**
		Get the process internal network address
	*/
	static PyObject* __py_address(PyObject* self, PyObject* args);

	/**
		Delete an entity from the database via dbid

		Delete the entity from the database. If the entity is not online, you can directly delete the callback and return true. If online, the callback returns the entityCall of the entity, and any other reason returns false.
	*/
	static PyObject* __py_deleteEntityByDBID(PyObject* self, PyObject* args);

		/** Network Interface
		Remove the callback of an entity from the database via dbid
	*/
	void deleteEntityByDBIDCB(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	/**
		Query whether an entity is checked out from the database by dbid

		if the entity online callback returns baseentitycall, the callback returns true if the entity is not online, and returns false for any other reason.
	*/
	static PyObject* __py_lookUpEntityByDBID(PyObject* self, PyObject* args);

		/** Network Interface
		if the entity online callback returns baseentitycall, the callback returns true if the entity is not online, and returns false for any other reason.
	*/
	void lookUpEntityByDBIDCB(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

		/** Network Interface
		Request binding email
	*/
	void reqAccountBindEmail(Network::Channel* pChannel, ENTITY_ID entityID, std::string& password, std::string& email);

		/** Network Interface
		Request to bind email, dbmgr returns results
	*/
	void onReqAccountBindEmailCBFromDBMgr(Network::Channel* pChannel, ENTITY_ID entityID, std::string& accountName, std::string& email,
		SERVER_ERROR_CODE failedcode, std::string& code);

		/** Network Interface
		Request to bind email, baseappmgr returns the address where you need to find loginapp
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

	// Record the account that was logged in to the server but has not been processed yet
	PendingLoginMgr											pendingLoginMgr_;

	ForwardComponent_MessageBuffer							forward_messagebuffer_;

	// backup archive related
	KBEShared_ptr< Backuper >								pBackuper_;	
	KBEShared_ptr< Archiver >								pArchiver_;	

	int32													numProxices_;

	TelnetServer*											pTelnetServer_;

	std::vector< KBEShared_ptr< RestoreEntityHandler > >	pRestoreEntityHandlers_;

	TimerHandle												pResmgrTimerHandle_;

	InitProgressHandler*									pInitProgressHandler_;
	
	// APP logo
	uint32													flags_;

	// For the client to dynamically import the entitydef protocol
	Network::Bundle*										pBundleImportEntityDefDatas_;
};

}

#endif // OURO_BASEAPP_H
