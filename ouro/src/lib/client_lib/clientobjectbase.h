// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com


#ifndef CLIENT_OBJECT_BASE_H
#define CLIENT_OBJECT_BASE_H

#include "event.h"
#include "script_callbacks.h"
#include "common/common.h"
#include "common/memorystream.h"
#include "helper/debug_helper.h"
#include "helper/script_loglevel.h"
#include "pyscript/scriptobject.h"
#include "entitydef/entities.h"
#include "entitydef/common.h"
#include "server/callbackmgr.h"
#include "server/server_errors.h"
#include "math/math.h"

namespace Ouroboros{

namespace client{
class Entity;
}

class EntityCall;
class EntityCallAbstract;

namespace Network
{
class Channel;
}

class ClientObjectBase : public script::ScriptObject
{
	/** 
		Subclassing populates some py operations into a derived class
	*/
	INSTANCE_SCRIPT_HREADER(ClientObjectBase, ScriptObject)	
public:
	ClientObjectBase(Network::NetworkInterface& ninterface, PyTypeObject* pyType = NULL);
	virtual ~ClientObjectBase();

	Network::Channel* pServerChannel() const{ return pServerChannel_; }
	void pServerChannel(Network::Channel* pChannel){ pServerChannel_ = pChannel; }

	virtual void finalise(void);
	virtual void reset(void);
	virtual void canReset(bool v){ canReset_ = v; }

	Entities<client::Entity>* pEntities() const{ return pEntities_; }

	/**
		Create an entity
	*/
	client::Entity* createEntity(const char* entityType, PyObject* params,
		bool isInitializeScript = true, ENTITY_ID eid = 0, bool initProperty = true, 
		EntityCall* base = NULL, EntityCall* cell = NULL);

	PY_CALLBACKMGR& callbackMgr(){ return pyCallbackMgr_; }	

	/**
		Destroy an entity by entityID
	*/
	virtual bool destroyEntity(ENTITY_ID entityID, bool callScript);

	void tickSend();
	
	virtual Network::Channel* initLoginappChannel(std::string accountName, 
		std::string passwd, std::string ip, Ouroboros::uint32 port);
	virtual Network::Channel* initBaseappChannel();

	bool createAccount();
	bool login();
	virtual void onLogin(Network::Bundle* pBundle);

	bool loginBaseapp();
	bool reloginBaseapp();

	int32 appID() const{ return appID_; }
	const char* name(){ return name_.c_str(); }

	ENTITY_ID entityID(){ return entityID_; }
	DBID dbid(){ return dbid_; }

	bool registerEventHandle(EventHandle* pEventHandle);
	bool deregisterEventHandle(EventHandle* pEventHandle);
	
	void fireEvent(const EventData* pEventData);
	
	EventHandler& eventHandler(){ return eventHandler_; }

	static PyObject* __pyget_pyGetEntities(PyObject *self, void *closure)
	{
		ClientObjectBase* pClientObjectBase = static_cast<ClientObjectBase*>(self);
		Py_INCREF(pClientObjectBase->pEntities());
		return pClientObjectBase->pEntities(); 
	}

	static PyObject* __pyget_pyGetID(PyObject *self, void *closure){
		
		ClientObjectBase* pClientObjectBase = static_cast<ClientObjectBase*>(self);
		return PyLong_FromLong(pClientObjectBase->appID());	
	}

	static PyObject* __py_getPlayer(PyObject *self, void *args);
	
	static PyObject* __py_callback(PyObject* self, PyObject* args);
	static PyObject* __py_cancelCallback(PyObject* self, PyObject* args);

	static PyObject* __py_getWatcher(PyObject* self, PyObject* args);
	static PyObject* __py_getWatcherDir(PyObject* self, PyObject* args);

	static PyObject* __py_disconnect(PyObject* self, PyObject* args);

	/**
		if the entitiessize is less than 256
		Get the entityID by index location
		Otherwise take the ID directly
	*/
	ENTITY_ID readEntityIDFromStream(MemoryStream& s);

	/**
		Try to get an instance of channel by entityCall
	*/
	virtual Network::Channel* findChannelByEntityCall(EntityCallAbstract& entityCall);

	/**
		Try to find an instance of it by the ID of the entity
	*/
	virtual PyObject* tryGetEntity(COMPONENT_ID componentID, ENTITY_ID entityID);

		/** Network Interface
		The client establishes an interaction with the server for the first time, and the server returns
	*/
	virtual void onHelloCB_(Network::Channel* pChannel, const std::string& verInfo,
		const std::string& scriptVerInfo, const std::string& protocolMD5, 
		const std::string& entityDefMD5, COMPONENT_TYPE componentType);

	virtual void onHelloCB(Network::Channel* pChannel, MemoryStream& s);

		/** Network Interface
		Does not match the version of the server
	*/
	virtual void onVersionNotMatch(Network::Channel* pChannel, MemoryStream& s);
	
		/** Network Interface
		Does not match the script layer version of the server
	*/
	virtual void onScriptVersionNotMatch(Network::Channel* pChannel, MemoryStream& s);

		/** Network Interface
		Create account success and failure callbacks
	   @failedcode: Failure return code NETWORK_ERR_SRV_NO_READY: The server is not ready,
									NETWORK_ERR_ACCOUNT_CREATE: Creation failed (already exists),
									NETWORK_SUCCESS: The account was created successfully.

									SERVER_ERROR_CODE failedcode;
		@binary accompanying data: binary extra data: uint32 length + bytearray
	*/
	virtual void onCreateAccountResult(Network::Channel * pChannel, MemoryStream& s);

		/** Network Interface
	   Login failure callback
	   @failedcode: Failure return code NETWORK_ERR_SRV_NO_READY: The server is not ready,
									NETWORK_ERR_SRV_OVERLOAD: The server is overloaded,
									NETWORK_ERR_NAME_PASSWORD: Username or password is incorrect
	*/
	virtual void onLoginFailed(Network::Channel * pChannel, MemoryStream& s);

		/** Network Interface
	   	   login successful
	   @ip: Server ip address
	   @port: server port
	*/
	virtual void onLoginSuccessfully(Network::Channel * pChannel, MemoryStream& s);

		/** Network Interface
	   Login failure callback
	   @failedcode: Failure return code NETWORK_ERR_SRV_NO_READY: The server is not ready,
									NETWORK_ERR_ILLEGAL_LOGIN: illegal login,
									NETWORK_ERR_NAME_PASSWORD: Username or password is incorrect
	*/
	virtual void onLoginBaseappFailed(Network::Channel * pChannel, SERVER_ERROR_CODE failedcode);
	virtual void onReloginBaseappFailed(Network::Channel * pChannel, SERVER_ERROR_CODE failedcode);

		/** Network Interface
	   Re-login to baseapp successfully
	*/
	virtual void onReloginBaseappSuccessfully(Network::Channel * pChannel, MemoryStream& s);

		/** Network Interface
		The server has created a proxy Entity associated with the client.
	   Can also express a successful callback when logging in
	   @datas: Account entity information
	*/
	virtual void onCreatedProxies(Network::Channel * pChannel, uint64 rndUUID, 
		ENTITY_ID eid, std::string& entityType);

		/** Network Interface
		The entity on the server has entered the game world.
	*/
	virtual void onEntityEnterWorld(Network::Channel * pChannel, MemoryStream& s);

		/** Network Interface
		The entity on the server has left the game world.
	*/
	virtual void onEntityLeaveWorld(Network::Channel * pChannel, ENTITY_ID eid);
	virtual void onEntityLeaveWorldOptimized(Network::Channel * pChannel, MemoryStream& s);

		/** Network Interface
		Tell the client that an entity has been destroyed. This type of entity is usually not yet onEntityEnterWorld.
	*/
	virtual void onEntityDestroyed(Network::Channel * pChannel, ENTITY_ID eid);

		/** Network Interface
		The entity on the server has entered the space.
	*/
	virtual void onEntityEnterSpace(Network::Channel * pChannel, MemoryStream& s);

		/** Network Interface
		The entity on the server has left the space.
	*/
	virtual void onEntityLeaveSpace(Network::Channel * pChannel, ENTITY_ID eid);

		/** Network Interface
		Method of calling entity remotely
	*/
	virtual void onRemoteMethodCall(Network::Channel* pChannel, MemoryStream& s);
	virtual void onRemoteMethodCallOptimized(Network::Channel* pChannel, MemoryStream& s);
	void onRemoteMethodCall_(ENTITY_ID eid, MemoryStream& s);

		/** Network Interface
	   Was kicked out of the server
	*/
	virtual void onKicked(Network::Channel * pChannel, SERVER_ERROR_CODE failedcode);

		/** Network Interface
		Server update entity attribute
	*/
	virtual void onUpdatePropertys(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdatePropertysOptimized(Network::Channel* pChannel, MemoryStream& s);
	void onUpdatePropertys_(ENTITY_ID eid, MemoryStream& s);

		/** Network Interface
		The server forces the location and orientation of the entity
	*/
	virtual void onSetEntityPosAndDir(Network::Channel* pChannel, MemoryStream& s);

		/** Network Interface
		Server update avatar base location and orientation
	*/
	virtual void onUpdateBasePos(Network::Channel* pChannel, float x, float y, float z);
	virtual void onUpdateBasePosXZ(Network::Channel* pChannel, float x, float z);
	virtual void onUpdateBaseDir(Network::Channel* pChannel, MemoryStream& s);

		/** Network Interface
		Server update VolatileData
	*/
	virtual void onUpdateData(Network::Channel* pChannel, MemoryStream& s);

		/** Network Interface
		Optimized location synchronization
	*/
	virtual void onUpdateData_ypr_optimized(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_yp_optimized(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_yr_optimized(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_pr_optimized(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_y_optimized(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_p_optimized(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_r_optimized(Network::Channel* pChannel, MemoryStream& s);

	virtual void onUpdateData_xz_optimized(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_xz_ypr_optimized(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_xz_yp_optimized(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_xz_yr_optimized(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_xz_pr_optimized(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_xz_y_optimized(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_xz_p_optimized(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_xz_r_optimized(Network::Channel* pChannel, MemoryStream& s);

	virtual void onUpdateData_xyz_optimized(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_xyz_ypr_optimized(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_xyz_yp_optimized(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_xyz_yr_optimized(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_xyz_pr_optimized(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_xyz_y_optimized(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_xyz_p_optimized(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_xyz_r_optimized(Network::Channel* pChannel, MemoryStream& s);

		/** Network Interface
		Non-optimized high precision synchronization
	*/
	virtual void onUpdateData_ypr(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_yp(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_yr(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_pr(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_y(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_p(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_r(Network::Channel* pChannel, MemoryStream& s);

	virtual void onUpdateData_xz(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_xz_ypr(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_xz_yp(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_xz_yr(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_xz_pr(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_xz_y(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_xz_p(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_xz_r(Network::Channel* pChannel, MemoryStream& s);

	virtual void onUpdateData_xyz(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_xyz_ypr(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_xyz_yp(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_xyz_yr(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_xyz_pr(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_xyz_y(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_xyz_p(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_xyz_r(Network::Channel* pChannel, MemoryStream& s);
	
	void _updateVolatileData(ENTITY_ID entityID, float x, float y, float z, float roll, 
		float pitch, float yaw, int8 isOnGround, bool isOptimized);

	/** 
		Update player to server
	*/
	virtual void updatePlayerToServer();

		/** Network Interface
		Download stream started
	*/
	virtual void onStreamDataStarted(Network::Channel* pChannel, int16 id, uint32 datasize, std::string& descr);

		/** Network Interface
		Received streamData
	*/
	virtual void onStreamDataRecv(Network::Channel* pChannel, MemoryStream& s);

		/** Network Interface
		The download stream is complete
	*/
	virtual void onStreamDataCompleted(Network::Channel* pChannel, int16 id);

		/** Network Interface
		The server tells the client: you currently (cancel) control who's displacement sync
	*/
	virtual void onControlEntity(Network::Channel* pChannel, int32 eid, int8 p_isControlled);

		/** Network Interface
		Received ClientMessages (usually web etc. will be applied)
	*/
	virtual void onImportClientMessages(Network::Channel* pChannel, MemoryStream& s){}

		/** Network Interface
		Received entitydef (usually web etc. will be applied)
	*/
	virtual void onImportClientEntityDef(Network::Channel* pChannel, MemoryStream& s){}
	
		/** Network Interface
		Error code description export (usually web etc. will be applied)
	*/
	virtual void onImportServerErrorsDescr(Network::Channel* pChannel, MemoryStream& s){}

		/** Network Interface
	Receive import sdk message (usually used during development, update client sdk)
	*/
	virtual void onImportClientSDK(Network::Channel* pChannel, MemoryStream& s) {}

		/** Network Interface
		Reset account password request return
	*/
	virtual void onReqAccountResetPasswordCB(Network::Channel* pChannel, SERVER_ERROR_CODE failedcode){}

		/** Network Interface
		Request binding mailbox return
	*/
	virtual void onReqAccountBindEmailCB(Network::Channel* pChannel, SERVER_ERROR_CODE failedcode){}

		/** Network Interface
		Request to change password return
	*/
	virtual void onReqAccountNewPasswordCB(Network::Channel* pChannel, SERVER_ERROR_CODE failedcode){}

	/** 
		Get the player instance
	*/
	client::Entity* pPlayer();

	void setTargetID(ENTITY_ID id){ 
		targetID_ = id; 
		onTargetChanged();
	}
	ENTITY_ID getTargetID() const{ return targetID_; }
	virtual void onTargetChanged(){}

	ENTITY_ID getViewEntityID(ENTITY_ID id);
	ENTITY_ID getViewEntityIDFromStream(MemoryStream& s);
	ENTITY_ID getViewEntityIDByAliasID(uint8 id);

	/** 
		Space related operation interface
		The server adds a geometric map of a space
	*/
	virtual void addSpaceGeometryMapping(SPACE_ID spaceID, const std::string& respath);
	virtual void onAddSpaceGeometryMapping(SPACE_ID spaceID, const std::string& respath){}
	virtual void onLoadedSpaceGeometryMapping(SPACE_ID spaceID){
		isLoadedGeometry_ = true;
	}

	const std::string& getGeometryPath();
	
	virtual void initSpaceData(Network::Channel* pChannel, MemoryStream& s);
	virtual void setSpaceData(Network::Channel* pChannel, SPACE_ID spaceID, const std::string& key, const std::string& value);
	virtual void delSpaceData(Network::Channel* pChannel, SPACE_ID spaceID, const std::string& key);
	bool hasSpaceData(const std::string& key);
	const std::string& getSpaceData(const std::string& key);
	static PyObject* __py_GetSpaceData(PyObject* self, PyObject* args);
	void clearSpace(bool isAll);

	Timers & timers() { return timers_; }
	void handleTimers();

	ScriptCallbacks & scriptCallbacks() { return scriptCallbacks_; }

	void locktime(uint64 t){ locktime_ = t; }
	uint64 locktime() const{ return locktime_; }

	virtual void onServerClosed();

	uint64 rndUUID() const{ return rndUUID_; }

	Network::NetworkInterface* pNetworkInterface()const { return &networkInterface_; }

		/** Network Interface
		Server heartbeat return
	*/
	void onAppActiveTickCB(Network::Channel* pChannel);

	/**
		Allow script to assert the underlying
	*/
	static PyObject* __py_assert(PyObject* self, PyObject* args);

protected:				
	int32													appID_;

	// server network channel
	Network::Channel*										pServerChannel_;

	// store all the containers of the entity
	Entities<client::Entity>*								pEntities_;	
	std::vector<ENTITY_ID>									pEntityIDAliasIDList_;

	PY_CALLBACKMGR											pyCallbackMgr_;

	ENTITY_ID												entityID_;
	SPACE_ID												spaceID_;

	DBID													dbid_;

	std::string												ip_;
	uint16													tcp_port_;
	uint16													udp_port_;

	std::string												baseappIP_;
	uint16													baseappPort_;

	uint64													lastSentActiveTickTime_;
	uint64													lastSentUpdateDataTime_;

	bool													connectedBaseapp_;
	bool													canReset_;

	std::string												name_;
	std::string												password_;

	std::string												clientDatas_;
	std::string												serverDatas_;

	CLIENT_CTYPE											typeClient_;

	typedef std::map<ENTITY_ID, OUROShared_ptr<MemoryStream> > BUFFEREDMESSAGE;
	BUFFEREDMESSAGE											bufferedCreateEntityMessage_;

	EventHandler											eventHandler_;

	Network::NetworkInterface&								networkInterface_;

	// The target selected by the current client
	ENTITY_ID												targetID_;

	// Whether terrain data has been loaded
	bool													isLoadedGeometry_;

	SPACE_DATA												spacedatas_;

	Timers													timers_;
	ScriptCallbacks											scriptCallbacks_;

	uint64													locktime_;
	
	// key used to re-login to the gateway
	uint64													rndUUID_; 

    // List of entities controlled by this client
    std::list<client::Entity *>                             controlledEntities_;
};



}
#endif
