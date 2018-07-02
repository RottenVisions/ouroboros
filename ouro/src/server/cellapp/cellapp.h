// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_CELLAPP_H
#define OURO_CELLAPP_H

#include "entity.h"
#include "spacememorys.h"
#include "cells.h"
#include "space_viewer.h"
#include "updatables.h"
#include "ghost_manager.h"
#include "witnessed_timeout_handler.h"
#include "server/entity_app.h"
#include "server/forward_messagebuffer.h"

namespace Ouroboros{

class TelnetServer;

class Cellapp:	public EntityApp<Entity>,
				public Singleton<Cellapp>
{
public:
	enum TimeOutType
	{
		TIMEOUT_LOADING_TICK = TIMEOUT_ENTITYAPP_MAX + 1
	};

	Cellapp(Network::EventDispatcher& dispatcher,
		Network::NetworkInterface& ninterface,
		COMPONENT_TYPE componentType,
		COMPONENT_ID componentID);

	~Cellapp();

	virtual bool installPyModules();
	virtual void onInstallPyModules();
	virtual bool uninstallPyModules();

	bool run();

	virtual bool initializeWatcher();

	/**
		Related processing interface
	*/
	virtual void handleTimeout(TimerHandle handle, void * arg);
	virtual void handleGameTick();

	/**
		Initialize related interfaces
	*/
	bool initializeBegin();
	bool initializeEnd();
	void finalise();

	virtual bool canShutdown();
	virtual void onShutdown(bool first);

	void destroyObjPool();

	float _getLoad() const { return getLoad(); }
	virtual void onUpdateLoad();

	/**  Network Interface
		Dbmgr tells the address of other baseapp or cellapp that has been started
		Current app needs to actively establish a connection with them
	*/
	virtual void onGetEntityAppFromDbmgr(Network::Channel* pChannel,
							int32 uid,
							std::string& username,
							COMPONENT_TYPE componentType, COMPONENT_ID componentID, COMPONENT_ORDER globalorderID, COMPONENT_ORDER grouporderID,
							uint32 intaddr, uint16 intport, uint32 extaddr, uint16 extport, std::string& extaddrEx);

	/**
		Created an entity callback
	*/
	virtual Entity* onCreateEntity(PyObject* pyEntity, ScriptDefModule* sm, ENTITY_ID eid);

	/**
		Create an entity
	*/
	static PyObject* __py_createEntity(PyObject* self, PyObject* args);

	/**
		Want dbmgr to execute a database command
	*/
	static PyObject* __py_executeRawDatabaseCommand(PyObject* self, PyObject* args);
	void executeRawDatabaseCommand(const char* datas, uint32 size, PyObject* pycallback, ENTITY_ID eid, const std::string& dbInterfaceName);
	void onExecuteRawDatabaseCommandCB(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	/** Network Interface
		Dbmgr sends initial information
		startID: Initial allocation of ENTITY_ID segment starting position
		endID: Initial allocation of ENTITY_ID segment end position
		startGlobalOrder: Global startup sequence includes a variety of different components
		startGroupOrder: The start order in the group, such as the first few start in all baseapps.
		machineGroupOrder: The real group order in the machine, providing the bottom layer to use at some point to determine if it is the first cellapp
	*/
	void onDbmgrInitCompleted(Network::Channel* pChannel, GAME_TIME gametime,
		ENTITY_ID startID, ENTITY_ID endID, COMPONENT_ORDER startGlobalOrder, COMPONENT_ORDER startGroupOrder,
		const std::string& digest);

	/** Network Interface
		Dbmgr broadcast global data changes
	*/
	void onBroadcastCellAppDataChanged(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	/** Network Interface
		baseEntity request created in a new space
	*/
	void onCreateCellEntityInNewSpaceFromBaseapp(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	/** Network Interface
		baseEntity request created in a new space
	*/
	void onRestoreSpaceInCellFromBaseapp(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	/** Network Interface
	Tool request to change the space viewer (including add and delete functions)
	If the viewer is requesting an update and the address does not exist on the server, it is automatically created. If it is deleted, the delete request is explicitly given.
	*/
	void setSpaceViewer(Network::Channel* pChannel, MemoryStream& s);

	/** Network Interface
		Other APP requests in this disaster recovery
	*/
	void requestRestore(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	/** Network Interface
		Baseapp request to create an entity on this cellapp
	*/
	void onCreateCellEntityFromBaseapp(Network::Channel* pChannel, Ouroboros::MemoryStream& s);
	void _onCreateCellEntityFromBaseapp(std::string& entityType, ENTITY_ID createToEntityID, ENTITY_ID entityID,
		MemoryStream* pCellData, bool hasClient, bool inRescore, COMPONENT_ID componentID, SPACE_ID spaceID);

	/** Network Interface
		Destroy a cellEntity
	*/
	void onDestroyCellEntityFromBaseapp(Network::Channel* pChannel, ENTITY_ID eid);

	/** Network Interface
		The entity receives a remote call request initiated by an entitycall on an app
	*/
	void onEntityCall(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	/** Network Interface
		The cell method of the client access entity is forwarded by the baseapp
	*/
	void onRemoteCallMethodFromClient(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	/** Network Interface
		Client update data
	*/
	void onUpdateDataFromClient(Network::Channel* pChannel, Ouroboros::MemoryStream& s);
	void onUpdateDataFromClientForControlledEntity(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	/** Network Interface
		Real request to update attributes to ghost
	*/
	void onUpdateGhostPropertys(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	/** Network Interface
		Ghost request to call def method real
	*/
	void onRemoteRealMethodCall(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	/** Network Interface
		Real request to update attributes to ghost
	*/
	void onUpdateGhostVolatileData(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	/** Network Interface
		Base request to get celldata
	*/
	void reqBackupEntityCellData(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	/** Network Interface
		Base request to get WriteToDB
	*/
	void reqWriteToDBFromBaseapp(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	/** Network Interface
		The client directly sends messages to the cell entity
	*/
	void forwardEntityMessageToCellappFromClient(Network::Channel* pChannel, MemoryStream& s);

	/**
		Get game time
	*/
	static PyObject* __py_gametime(PyObject* self, PyObject* args);

	/**
		Adding and Removing an Updatable Object
	*/
	bool addUpdatable(Updatable* pObject);
	bool removeUpdatable(Updatable* pObject);

	/**
		hook entitycallcall
	*/
	RemoteEntityMethod* createEntityCallCallEntityRemoteMethod(MethodDescription* pMethodDescription, EntityCallAbstract* pEntityCall);

	/** Network Interface
		An app requests to view the app
	*/
	virtual void lookApp(Network::Channel* pChannel);

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

	WitnessedTimeoutHandler	* pWitnessedTimeoutHandler(){ return pWitnessedTimeoutHandler_; }

	/**
		Network Interface
		Another cellapp entity wants to teleport to the space on this cellapp
	*/
	void reqTeleportToCellApp(Network::Channel* pChannel, MemoryStream& s);
	void reqTeleportToCellAppCB(Network::Channel* pChannel, MemoryStream& s);
	void reqTeleportToCellAppOver(Network::Channel* pChannel, MemoryStream& s);

	/**
		Get and set the ghost manager
	*/
	void pGhostManager(GhostManager* v){ pGhostManager_ = v; }
	GhostManager* pGhostManager() const{ return pGhostManager_; }

	ArraySize spaceSize() const { return (ArraySize)SpaceMemorys::size(); }

	/**
		Rays
	*/
	int raycast(SPACE_ID spaceID, int layer, const Position3D& start, const Position3D& end, std::vector<Position3D>& hitPos);
	static PyObject* __py_raycast(PyObject* self, PyObject* args);

	uint32 flags() const { return flags_; }
	void flags(uint32 v) { flags_ = v; }
	static PyObject* __py_setFlags(PyObject* self, PyObject* args);
	static PyObject* __py_getFlags(PyObject* self, PyObject* args);

protected:
	// cellAppData
	GlobalDataClient*					pCellAppData_;

	ForwardComponent_MessageBuffer		forward_messagebuffer_;

	Updatables							updatables_;

	// All cells
	Cells								cells_;

	TelnetServer*						pTelnetServer_;

	WitnessedTimeoutHandler	*			pWitnessedTimeoutHandler_;

	GhostManager*						pGhostManager_;

	// APP logo
	uint32								flags_;

	// View space through tools
	SpaceViewers						spaceViewers_;
};

}

#endif // OURO_CELLAPP_H
