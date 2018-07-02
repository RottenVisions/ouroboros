// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com


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
		Sub-class of the some of the py operation is filled into the derived class
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
		Create a entity
	*/
	client::Entity* createEntity(const char* entityType, PyObject* params,
		bool isInitializeScript = true, ENTITY_ID eid = 0, bool initProperty = true,
		EntityCall* base = NULL, EntityCall* cell = NULL);

	PY_CALLBACKMGR& callbackMgr(){ return pyCallbackMgr_; }

	/**
		By entityID the destruction of a entity
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

	static PyObject* __py_callback(PyObject* self, PyObject* args);
	static PyObject* __py_cancelCallback(PyObject* self, PyObject* args);

	static PyObject* __py_getWatcher(PyObject* self, PyObject* args);
	static PyObject* __py_getWatcherDir(PyObject* self, PyObject* args);

	static PyObject* __py_disconnect(PyObject* self, PyObject* args);

	/**
		 If entitiessize less than 256
		By index position to get the entityID
		Otherwise directly take the ID
	*/
	ENTITY_ID readEntityIDFromStream(MemoryStream& s);

	/**
		By entityCall to try to get a channel Examples
	*/
	virtual Network::Channel* findChannelByEntityCall(EntityCallAbstract& entityCall);

	/**
		By entity by ID Try to look for it in the instance
	*/
	virtual PyObject* tryGetEntity(COMPONENT_ID componentID, ENTITY_ID entityID);

	/** The network interface
			The client and server is first established the interaction, the service end returns
	*/
	virtual void onHelloCB_(Network::Channel* pChannel, const std::string& verInfo,
		const std::string& scriptVerInfo, const std::string& protocolMD5,
		const std::string& entityDefMD5, COMPONENT_TYPE componentType);

	virtual void onHelloCB(Network::Channel* pChannel, MemoryStream& s);

	/** The network interface
			And server version do not match
	*/
	virtual void onVersionNotMatch(Network::Channel* pChannel, MemoryStream& s);

	/** The network interface
			And server-side scripting layer version mismatch
	*/
	virtual void onScriptVersionNotMatch(Network::Channel* pChannel, MemoryStream& s);

	/** The network interface
			Create account success and failure callback
			@failedcode: failed return code NETWORK_ERR_SRV_NO_READY:server not ready,
			NETWORK_ERR_ACCOUNT_CREATE:failure to create already exists,
			NETWORK_SUCCESS:account successfully created

			SERVER_ERROR_CODE failedcode
	*/
	virtual void onCreateAccountResult(Network::Channel * pChannel, MemoryStream& s);

/** Network interface Login failure callback
@failedcode: Failed return code NETWORK_ERR_SRV_NO_READY: The server is not ready,
NETWORK_ERR_SRV_OVERLOAD: The server is overloaded,
NETWORK_ERR_NAME_PASSWORD: The username or password is incorrect */
	virtual void onLoginFailed(Network::Channel * pChannel, MemoryStream& s);

	/** Network Interface Login Success
	@ip: Server IP Address
	@port: Server Port
	*/
	virtual void onLoginSuccessfully(Network::Channel * pChannel, MemoryStream& s);

/** Network interface Login failure callback
@failedcode: Failed return code
NETWORK_ERR_SRV_NO_READY: Server not ready,
NETWORK_ERR_ILLEGAL_LOGIN: Illegal login,
NETWORK_ERR_NAME_PASSWORD: Incorrect username or password */
	virtual void onLoginBaseappFailed(Network::Channel * pChannel, SERVER_ERROR_CODE failedcode);
	virtual void onReloginBaseappFailed(Network::Channel * pChannel, SERVER_ERROR_CODE failedcode);

/** Network Interface Re-entering the baseapp successfully */
	virtual void onReloginBaseappSuccessfully(Network::Channel * pChannel, MemoryStream& s);

/** Network Interface The server side has created a proxy Entity that is associated with the client and can also signal a
successful callback when logging in. @datas: The account entity information */
	virtual void onCreatedProxies(Network::Channel * pChannel, uint64 rndUUID,
		ENTITY_ID eid, std::string& entityType);

/** Network Interface The entity on the server has entered the game world. */
	virtual void onEntityEnterWorld(Network::Channel * pChannel, MemoryStream& s);

/** Network Interface The entity on the server has left the game world */
	virtual void onEntityLeaveWorld(Network::Channel * pChannel, ENTITY_ID eid);
	virtual void onEntityLeaveWorldOptimized(Network::Channel * pChannel, MemoryStream& s);

/** The network interface tells the client that an entity is destroyed. Such an entity is usually not yet onEntityEnterWorld */
	virtual void onEntityDestroyed(Network::Channel * pChannel, ENTITY_ID eid);

/** Network Interface The entity on the server has entered space */
	virtual void onEntityEnterSpace(Network::Channel * pChannel, MemoryStream& s);

/** Network Interface The entity on the server has left space */
	virtual void onEntityLeaveSpace(Network::Channel * pChannel, ENTITY_ID eid);

/** Network Interface Method for remotely calling entity */
	virtual void onRemoteMethodCall(Network::Channel* pChannel, MemoryStream& s);
	virtual void onRemoteMethodCallOptimized(Network::Channel* pChannel, MemoryStream& s);
	void onRemoteMethodCall_(ENTITY_ID eid, MemoryStream& s);

/**
	network interface was kicked out of the server
*/
	virtual void onKicked(Network::Channel * pChannel, SERVER_ERROR_CODE failedcode);

/** Network Interface Server Update entity attribute */
	virtual void onUpdatePropertys(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdatePropertysOptimized(Network::Channel* pChannel, MemoryStream& s);
	void onUpdatePropertys_(ENTITY_ID eid, MemoryStream& s);

/** Network Interface The server enforces the location and orientation of the entity */
	virtual void onSetEntityPosAndDir(Network::Channel* pChannel, MemoryStream& s);

	/** The network interface
		Server Update avatar The basis of the position and orientation of the
	*/
	virtual void onUpdateBasePos(Network::Channel* pChannel, float x, float y, float z);
	virtual void onUpdateBasePosXZ(Network::Channel* pChannel, float x, float z);
	virtual void onUpdateBaseDir(Network::Channel* pChannel, MemoryStream& s);

	/** The network interface
		Server Update VolatileData
	*/
	virtual void onUpdateData(Network::Channel* pChannel, MemoryStream& s);

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
		float pitch, float yaw, int8 isOnGround);

	/**
		Update the player to the server
	*/
	virtual void updatePlayerToServer();

	/** The network interface
		download stream Started
	*/
	virtual void onStreamDataStarted(Network::Channel* pChannel, int16 id, uint32 datasize, std::string& descr);

	/** The network interface
		Received streamData
	*/
	virtual void onStreamDataRecv(Network::Channel* pChannel, MemoryStream& s);

	/** The network interface
		download stream Completed
	*/
	virtual void onStreamDataCompleted(Network::Channel* pChannel, int16 id);

	/** The network interface
			The server tells the client: you current / unlink control who of the displacement of synchronization
	*/
	virtual void onControlEntity(Network::Channel* pChannel, int32 eid, int8 p_isControlled);

	/** The network interface
		Received ClientMessages (Is usually web Etc will only be applied to)
	*/
	virtual void onImportClientMessages(Network::Channel* pChannel, MemoryStream& s){}

	/** The network interface
		Received entitydef (Is usually web Etc will only be applied to)
	*/
	virtual void onImportClientEntityDef(Network::Channel* pChannel, MemoryStream& s){}

	/** The network interface
			Error Code Description export(usually the web, etc. will be applied to)
	*/
	virtual void onImportServerErrorsDescr(Network::Channel* pChannel, MemoryStream& s){}

	/** The network interface
			Reset account password to request to return
	*/
	virtual void onReqAccountResetPasswordCB(Network::Channel* pChannel, SERVER_ERROR_CODE failedcode){}

	/** The network interface
			A request to bind a mailbox to return
	*/
	virtual void onReqAccountBindEmailCB(Network::Channel* pChannel, SERVER_ERROR_CODE failedcode){}

	/** The network interface
			The request to modify the password, return
	*/
	virtual void onReqAccountNewPasswordCB(Network::Channel* pChannel, SERVER_ERROR_CODE failedcode){}

	/**
		Get player Examples
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
		space Related to the operation interface
		Service end add a space geometric mapping
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

	/** The network interface
		Server heartbeat returns
	*/
	void onAppActiveTickCB(Network::Channel* pChannel);

	/**
		Allow the script to assert the underlying
	*/
	static PyObject* __py_assert(PyObject* self, PyObject* args);

protected:
	int32													appID_;

	// The service end of the network channel
	Network::Channel*										pServerChannel_;

	// Store all of entity in container
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

	// The current client to the selected target
	ENTITY_ID												targetID_;

	// Whether loaded through the terrain data
	bool													isLoadedGeometry_;

	SPACE_DATA												spacedatas_;

	Timers													timers_;
	ScriptCallbacks											scriptCallbacks_;

	uint64													locktime_;

	// For re-login the gateway when the key
	uint64													rndUUID_;

    // Subject to the control of the client entity List
    std::list<client::Entity *>                             controlledEntities_;
};



}
#endif
