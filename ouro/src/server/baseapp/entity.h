// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com



#ifndef OURO_BASE_ENTITY_H
#define OURO_BASE_ENTITY_H

#include "profile.h"
#include "common/common.h"
#include "helper/debug_helper.h"
#include "pyscript/math.h"
#include "pyscript/scriptobject.h"
#include "entitydef/datatypes.h"
#include "entitydef/entitydef.h"
#include "entitydef/scriptdef_module.h"
#include "entitydef/entity_macro.h"
#include "entitydef/entity_component.h"
#include "server/script_timers.h"

namespace Ouroboros{

class EntityCall;
class EntityComponent;
class EntityMessagesForwardCellappHandler;
class BaseMessagesForwardClientHandler;

namespace Network
{
class Channel;
}


class Entity : public script::ScriptObject
{
	/** Subclassing fills some py operations into derived classes */
	BASE_SCRIPT_HREADER(Entity, ScriptObject)
	ENTITY_HEADER(Entity)
public:
	Entity(ENTITY_ID id, const ScriptDefModule* pScriptModule,
		PyTypeObject* pyType = getScriptType(), bool isInitialised = true);
	~Entity();

	/**
		Whether to store the database
	*/
	INLINE bool hasDB() const;
	INLINE void hasDB(bool has);

	/**
		Database Association ID
	*/
	INLINE DBID dbid() const;
	INLINE void dbid(uint16 dbInterfaceIndex, DBID id);
	DECLARE_PY_GET_MOTHOD(pyGetDBID);

	/**
	Database Association Name
	*/
	INLINE uint16 dbInterfaceIndex() const;
	DECLARE_PY_GET_MOTHOD(pyGetDBInterfaceName);

	/**
		The entity that destroyed the cell part
	*/
	bool destroyCellEntity(void);

	DECLARE_PY_MOTHOD_ARG0(pyDestroyCellEntity);

	/**
		Script gets entityCall
	*/
	DECLARE_PY_GET_MOTHOD(pyGetCellEntityCall);

	EntityCall* cellEntityCall(void) const;

	void cellEntityCall(EntityCall* entityCall);

	/**
		Script gets entityCall
	*/
	DECLARE_PY_GET_MOTHOD(pyGetClientEntityCall);

	EntityCall* clientEntityCall() const;

	void clientEntityCall(EntityCall* entityCall);

	/**
		Whether space was created
	*/
	INLINE bool isCreatedSpace();

	/**
		cellData section
	*/
	bool installCellDataAttr(PyObject* dictData = NULL, bool installpy = true);

	void createCellData(void);

	void destroyCellData(void);

	void addPersistentsDataToStream(uint32 flags, MemoryStream* s);

	PyObject* createCellDataDict(uint32 flags);

	INLINE PyObject* getCellData(void) const;

	INLINE bool creatingCell(void) const;

	/**
		The request cell part updates the entity's celldata.
	*/
	void reqBackupCellData();

	/**
		Write backup information to the stream
	*/
	void writeBackupData(MemoryStream* s);
	void onBackup();

	/**
		Write archived information to the stream
	*/
	void writeArchiveData(MemoryStream* s);

	/**
		Before you save to the database
	*/
	void onWriteToDB();
	void onCellWriteToDBCompleted(CALLBACK_ID callbackID, int8 shouldAutoLoad, int dbInterfaceIndex);
	void onWriteToDBCallback(ENTITY_ID eid, DBID entityDBID, uint16 dbInterfaceIndex,
		CALLBACK_ID callbackID, int8 shouldAutoLoad, bool success);

	/** Network Interface
		The first time the entity writes the dbid returned by the database from dbmgr
	*/
	void onGetDBID(Network::Channel* pChannel, DBID dbid);

	/**
		Create cell failure callback
	*/
	void onCreateCellFailure(void);

	/**
		Create cell success callback
	*/
	void onGetCell(Network::Channel* pChannel, COMPONENT_ID componentID);

	/**
		Lost cell notification
	*/
	void onLoseCell(Network::Channel* pChannel, MemoryStream& s);

	/**
		When cellapp unexpectedly terminates, baseapp can recover if it finds a suitable cellapp.
		Will call this method
	*/
	void onRestore();

	/**
		Backup cell data
	*/
	void onBackupCellData(Network::Channel* pChannel, MemoryStream& s);

	/**
		Client lost
	*/
	void onClientDeath();

	/** Network Interface
		Method for remotely calling this entity
	*/
	void onRemoteMethodCall(Network::Channel* pChannel, MemoryStream& s);

	/**
		Destroy this entity
	*/
	void onDestroy(bool callScript);

	/**
		Destroy the base internal notification
	*/
	void onDestroyEntity(bool deleteFromDB, bool writeToDB);

	/**
		Create a cellEntity on a specified cell for a baseEntity
	*/
	DECLARE_PY_MOTHOD_ARG1(createCellEntity, PyObject_ptr);

	/**
		Restore a cellEntity on a specified cell for a baseEntity
	*/
	void restoreCell(EntityCallAbstract* cellEntityCall);
	INLINE bool inRestore();

	/**
		Create a cellEntity on a new space
	*/
	DECLARE_PY_MOTHOD_ARG1(createCellEntityInNewSpace, PyObject_ptr);

	/** Network Interface
		The client directly sends messages to the cell entity
	*/
	void forwardEntityMessageToCellappFromClient(Network::Channel* pChannel, MemoryStream& s);

	/**
		Send message to cellapp
	*/
	void sendToCellapp(Network::Bundle* pBundle);
	void sendToCellapp(Network::Channel* pChannel, Network::Bundle* pBundle);

	/**
		Transfer
	*/
	DECLARE_PY_MOTHOD_ARG1(pyTeleport, PyObject_ptr);

	/**
		Send callback
	*/
	void onTeleportCB(Network::Channel* pChannel, SPACE_ID spaceID, bool fromCellTeleport);
	void onTeleportFailure();
	void onTeleportSuccess(SPACE_ID spaceID);

	/** Network Interface
		An entity requests teleport to the space of this entity.
	*/
	void reqTeleportOther(Network::Channel* pChannel, ENTITY_ID reqTeleportEntityID,
		COMPONENT_ID reqTeleportEntityCellAppID, COMPONENT_ID reqTeleportEntityBaseAppID);

	/** Network Interface
		The process of the entity request to migrate to another cellapp begins and ends.
	*/
	void onMigrationCellappStart(Network::Channel* pChannel, COMPONENT_ID sourceCellAppID, COMPONENT_ID targetCellAppID);
	void onMigrationCellappEnd(Network::Channel* pChannel, COMPONENT_ID sourceCellAppID, COMPONENT_ID targetCellAppID);
	void onMigrationCellappOver(COMPONENT_ID targetCellAppID);

	/**
		Set whether to get automatic archiving
	*/
	INLINE int8 shouldAutoArchive() const;
	INLINE void shouldAutoArchive(int8 v);
	DECLARE_PY_GETSET_MOTHOD(pyGetShouldAutoArchive, pySetShouldAutoArchive);

	/**
		Set whether to get automatic backup
	*/
	INLINE int8 shouldAutoBackup() const;
	INLINE void shouldAutoBackup(int8 v);
	DECLARE_PY_GETSET_MOTHOD(pyGetShouldAutoBackup, pySetShouldAutoBackup);

	/**
		Cellapp smashed
	*/
	void onCellAppDeath();

	/**
		Forwarding message completed
	*/
	void onBufferedForwardToCellappMessagesOver();
	void onBufferedForwardToClientMessagesOver();

	INLINE BaseMessagesForwardClientHandler* pBufferedSendToClientMessages();

	/**
		Set the entity persistence data is dirty, dirty will be automatically archived
	*/
	INLINE void setDirty(bool dirty = true);
	INLINE bool isDirty() const;

protected:
	/**
		Defined attribute data was changed
	*/
	void onDefDataChanged(EntityComponent* pEntityComponent, const PropertyDescription* propertyDescription,
			PyObject* pyData);

	/**
		Erase online log from db
	*/
	void eraseEntityLog();

protected:
	// The entity's client entityCall cellapp entityCall
	EntityCall*								clientEntityCall_;
	EntityCall*								cellEntityCall_;

	// After the entity is created, some cell attribute data is saved here when the cell part is not created
	PyObject*								cellDataDict_;

	// Whether it is stored in the database entity
	bool									hasDB_;
	DBID									DBID_;

	// Whether you are getting celldata
	bool									isGetingCellData_;

	// Whether it is being archived
	bool									isArchiveing_;

	// Whether to perform automatic archiving &lt;= 0 is false, 1 is true, OURO_NEXT_ONLY is automatically false after execution
	int8									shouldAutoArchive_;

	// Whether to perform automatic backup &lt;= 0 is false, 1 is true, OURO_NEXT_ONLY is automatically false after execution
	int8									shouldAutoBackup_;

	// Whether you are creating a cell
	bool									creatingCell_;

	// Have you created a space
	bool									createdSpace_;

	// Whether it is recovering
	bool									inRestore_;

	// If the entity has not been set to ENTITY_FLAGS_TELEPORT_START at this time, the onMigrationCellappArrived package takes precedence over
	// Arrival of onMigrationCellappStart (some pressure causes the entity to jump across processes (jump from cell1 to cell2),
	// The packet generated before the jump will arrive slower than the enter2 packet of cell2. Therefore, when this happens, the packet of cell2 needs to be cached first.
	// Wait until the packet of cell1 arrives and then execute the packet of cell2
	BaseMessagesForwardClientHandler*		pBufferedSendToClientMessages_;

	// Whether the data that needs to be persisted is dirty or not persistent if it is not dirty
	bool									isDirty_;

	// If this entity has already written to the database, then this attribute is the index of the corresponding database interface
	uint16									dbInterfaceIndex_;
};

}


#ifdef CODE_INLINE
#include "entity.inl"
#endif

#endif // OURO_BASE_ENTITY_H
