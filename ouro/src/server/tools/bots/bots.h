// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com


#ifndef OURO_BOTS_H
#define OURO_BOTS_H

// common include
#include "profile.h"
#include "create_and_login_handler.h"
#include "common/timer.h"
#include "pyscript/script.h"
#include "network/endpoint.h"
#include "helper/debug_helper.h"
#include "helper/script_loglevel.h"
#include "xml/xml.h"
#include "common/singleton.h"
#include "common/smartpointer.h"
#include "common/timer.h"
#include "network/interfaces.h"
#include "network/event_dispatcher.h"
#include "network/network_interface.h"
#include "network/event_poller.h"
#include "client_lib/clientapp.h"
#include "pyscript/pyobject_pointer.h"
#include "entitydef/entitydef.h"

//#define NDEBUG
// windows include
#if OURO_PLATFORM == PLATFORM_WIN32
#else
// linux include
#endif

namespace Ouroboros{

class ClientObject;
class PyBots;
class TelnetServer;

class Bots  : public ClientApp
{
public:
	Bots(Network::EventDispatcher& dispatcher,
		Network::NetworkInterface& ninterface,
		COMPONENT_TYPE componentType,
		COMPONENT_ID componentID);

	~Bots();

	virtual bool initialize();
	virtual void finalise();

	virtual bool initializeBegin();
	virtual bool initializeEnd();

	virtual bool installPyModules();
	virtual void onInstallPyModules() {};
	virtual bool uninstallPyModules();
	bool uninstallPyScript();
	bool installEntityDef();

	virtual void handleTimeout(TimerHandle, void * pUser);
	virtual void handleGameTick();

	static Bots& getSingleton(){
		return *static_cast<Bots*>(ClientApp::getSingletonPtr());
	}

	/**
		Set script output type prefix
	*/
	static PyObject* __py_setScriptLogType(PyObject* self, PyObject* args);

	bool run(void);

	/**
		Try to get an instance of a channel by entityCall
	*/
	virtual Network::Channel* findChannelByEntityCall(EntityCallAbstract& entityCall);

	/**
	Trying to find an instance of it by the ID of the entity
	*/
	virtual PyObject* tryGetEntity(COMPONENT_ID componentID, ENTITY_ID entityID);

	/** Network Interface
		An app requests to view the app
	*/
	virtual void lookApp(Network::Channel* pChannel);

	/** Network Interface
		Request to shut down the server
	*/
	virtual void reqCloseServer(Network::Channel* pChannel, MemoryStream& s);

	/** Network Interface
		Request to shut down the server
	*/
	void reqKillServer(Network::Channel* pChannel, MemoryStream& s);

	void onExecScriptCommand(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	typedef std::map< Network::Channel*, ClientObject* > CLIENTS;
	CLIENTS& clients(){ return clients_; }

	uint32 reqCreateAndLoginTotalCount(){ return reqCreateAndLoginTotalCount_; }
	void reqCreateAndLoginTotalCount(uint32 v){ reqCreateAndLoginTotalCount_ = v; }

	uint32 reqCreateAndLoginTickCount(){ return reqCreateAndLoginTickCount_; }
	void reqCreateAndLoginTickCount(uint32 v){ reqCreateAndLoginTickCount_ = v; }

	float reqCreateAndLoginTickTime(){ return reqCreateAndLoginTickTime_; }
	void reqCreateAndLoginTickTime(float v){ reqCreateAndLoginTickTime_ = v; }

	bool addClient(ClientObject* pClient);
	bool delClient(ClientObject* pClient);
	bool delClient(Network::Channel * pChannel);

	ClientObject* findClient(Network::Channel * pChannel);
	ClientObject* findClientByAppID(int32 appID);

	static PyObject* __py_addBots(PyObject* self, PyObject* args);

	/** Network Interface
	   Add bots
	   @total uint32: Total number added
	   @ticknum uint32: How many ticks per tick
	   @ticktime float: a tick's time
	*/
	virtual void addBots(Network::Channel * pChannel, MemoryStream& s);

	/** Network Interface
		An app informs the app that it is active.
	*/
	void onAppActiveTick(Network::Channel* pChannel, COMPONENT_TYPE componentType, COMPONENT_ID componentID);

	virtual void onHelloCB_(Network::Channel* pChannel, const std::string& verInfo,
		const std::string& scriptVerInfo, const std::string& protocolMD5,
		const std::string& entityDefMD5, COMPONENT_TYPE componentType);

	/** Network Interface
		The server version does not match
	*/
	virtual void onVersionNotMatch(Network::Channel* pChannel, MemoryStream& s);

	/** Network Interface
		Mismatched script layer version with server
	*/
	virtual void onScriptVersionNotMatch(Network::Channel* pChannel, MemoryStream& s);

	/** Network Interface
		Create account success and failure callbacks
	   @failedcode: Failed return code NETWORK_ERR_SRV_NO_READY:The server is not ready,
									NETWORK_ERR_ACCOUNT_CREATE:Failed to create (already exists),
									NETWORK_SUCCESS:Account created successfully

									SERVER_ERROR_CODE failedcode;
		@Binary data:Binary extra data: uint32 length + bytearray
	*/
	virtual void onCreateAccountResult(Network::Channel * pChannel, MemoryStream& s);

	Network::EventPoller* pEventPoller(){ return pEventPoller_; }

	/** Network Interface
	   Login failure callback
	   @failedcode: Failed return code NETWORK_ERR_SRV_NO_READY:The server is not ready,
									NETWORK_ERR_SRV_OVERLOAD:Server is overloaded,
									NETWORK_ERR_NAME_PASSWORD:incorrect username or password
	*/
	virtual void onLoginFailed(Network::Channel * pChannel, MemoryStream& s);

	/** Network Interface
	   login successful
	   @ip: Server IP address
	   @port: Server port
	*/
	virtual void onLoginSuccessfully(Network::Channel * pChannel, MemoryStream& s);

	/** Network Interface
	   Login failure callback
	   @failedcode: Failed return code NETWORK_ERR_SRV_NO_READY:The server is not ready,
									NETWORK_ERR_ILLEGAL_LOGIN:Illegal login,
									NETWORK_ERR_NAME_PASSWORD:incorrect username or password
	*/
	virtual void onLoginBaseappFailed(Network::Channel * pChannel, SERVER_ERROR_CODE failedcode);

	/** Re-entering the baseapp successfully
	   Re-entering the baseapp successfully
	*/
	virtual void onReloginBaseappSuccessfully(Network::Channel * pChannel, MemoryStream& s);

	/** Network Interface
		The server side has created a proxy Entity associated with the client
	   A successful callback can also be expressed at login
	   @datas: Account information
	*/
	virtual void onCreatedProxies(Network::Channel * pChannel, uint64 rndUUID,
		ENTITY_ID eid, std::string& entityType);

	/** Network Interface
		The entity on the server has entered the game world
	*/
	virtual void onEntityEnterWorld(Network::Channel * pChannel, MemoryStream& s);


	/** Network Interface
		The entity on the server has left the game world
	*/
	virtual void onEntityLeaveWorld(Network::Channel * pChannel, ENTITY_ID eid);
	virtual void onEntityLeaveWorldOptimized(Network::Channel * pChannel, MemoryStream& s);

	/** Network Interface
		Tell the client that an entity is destroyed This type of entity is usually not yet onEntityEnterWorld
	*/
	virtual void onEntityDestroyed(Network::Channel * pChannel, ENTITY_ID eid);

	/** Network Interface
		The entity on the server has entered space
	*/
	virtual void onEntityEnterSpace(Network::Channel * pChannel, MemoryStream& s);

	/** Network Interface
		The entity on the server has left space
	*/
	virtual void onEntityLeaveSpace(Network::Channel * pChannel, ENTITY_ID eid);

	/** Network Interface
		Method of remotely calling entity
	*/
	virtual void onRemoteMethodCall(Network::Channel* pChannel, MemoryStream& s);
	virtual void onRemoteMethodCallOptimized(Network::Channel* pChannel, MemoryStream& s);

	/** Network Interface
	   Was kicked out of the server
	*/
	virtual void onKicked(Network::Channel * pChannel, SERVER_ERROR_CODE failedcode);

	/** Network Interface
		The server updates the entity attribute
	*/
	virtual void onUpdatePropertys(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdatePropertysOptimized(Network::Channel* pChannel, MemoryStream& s);

	/** Network Interface
		The server updates the avatar base position and orientation
	*/
	virtual void onUpdateBasePos(Network::Channel* pChannel, float x, float y, float z);
	virtual void onUpdateBasePosXZ(Network::Channel* pChannel, float x, float z);
	virtual void onUpdateBaseDir(Network::Channel* pChannel, MemoryStream& s);

	/** Network Interface
		The server forces the location and orientation of the entity
	*/
	virtual void onSetEntityPosAndDir(Network::Channel* pChannel, MemoryStream& s);

	/** Network Interface
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

	/** Network Interface
		download Stream started
	*/
	virtual void onStreamDataStarted(Network::Channel* pChannel, int16 id, uint32 datasize, std::string& descr);

	/** Network Interface
		Received streamData
	*/
	virtual void onStreamDataRecv(Network::Channel* pChannel, MemoryStream& s);

	/** Network Interface
		download Stream completed
	*/
	virtual void onStreamDataCompleted(Network::Channel* pChannel, int16 id);

	/** Network Interface
		Space related operation interface
		The server adds a geometry mapping for a space
	*/
	virtual void initSpaceData(Network::Channel* pChannel, MemoryStream& s);
	virtual void setSpaceData(Network::Channel* pChannel, SPACE_ID spaceID, const std::string& key, const std::string& value);
	virtual void delSpaceData(Network::Channel* pChannel, SPACE_ID spaceID, const std::string& key);

	/** Network Interface
		Request to see watcher
	*/
	void queryWatcher(Network::Channel* pChannel, MemoryStream& s);

	/** Network Interface
		Console request start profile
	*/
	void startProfile(Network::Channel* pChannel, Ouroboros::MemoryStream& s);
	virtual void startProfile_(Network::Channel* pChannel, std::string profileName, int8 profileType, uint32 timelen);

	/** Network Interface
	    The server tells the client that you currently (cancel) control who synchronizes the shifts
	*/
	virtual void onControlEntity(Network::Channel* pChannel, int32 entityID, int8 isControlled);

	/** Network Interface
		Server heartbeat return
	*/
	void onAppActiveTickCB(Network::Channel* pChannel);

protected:
	PyBots*													pPyBots_;

	CLIENTS													clients_;

	// The number of bots that the console requests to create to the server
	uint32													reqCreateAndLoginTotalCount_;
	uint32													reqCreateAndLoginTickCount_;
	float													reqCreateAndLoginTickTime_;

	// Handle creation and login handlers
	CreateAndLoginHandler*									pCreateAndLoginHandler_;

	Network::EventPoller*									pEventPoller_;

	TelnetServer*											pTelnetServer_;
};

}

#endif // OURO_BOTS_H
