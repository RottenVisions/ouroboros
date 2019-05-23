// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com



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
		/** Subclassing populates some py operations into derived classes*/
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
		Database association ID
	*/
	INLINE DBID dbid() const;
	INLINE void dbid(uint16 dbInterfaceIndex, DBID id);
	DECLARE_PY_GET_MOTHOD(pyGetDBID);

	/**
	Database association name
	*/
	INLINE uint16 dbInterfaceIndex() const;
	DECLARE_PY_GET_MOTHOD(pyGetDBInterfaceName);

	/** 
		Destroy the entity of the cell part
	*/
	bool destroyCellEntity(void);

	DECLARE_PY_MOTHOD_ARG0(pyDestroyCellEntity);
	
	/** 
		Script to get entityCall
	*/
	DECLARE_PY_GET_MOTHOD(pyGetCellEntityCall);

	EntityCall* cellEntityCall(void) const;

	void cellEntityCall(EntityCall* entityCall);
	
	/** 
		Script to get entityCall
	*/
	DECLARE_PY_GET_MOTHOD(pyGetClientEntityCall);

	EntityCall* clientEntityCall() const;

	void clientEntityCall(EntityCall* entityCall);

	/**
		Have you created a space?
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
		Request the cell part to update the celldata of the entity.
	*/
	void reqBackupCellData();
	
	/** 
		Write backup information to the stream
	*/
	void writeBackupData(MemoryStream* s);
	void onBackup();

	/** 
		Write archive information to the stream
	*/
	void writeArchiveData(MemoryStream* s);

	/** 
		Notification before saving to the database
	*/
	void onWriteToDB();
	void onCellWriteToDBCompleted(CALLBACK_ID callbackID, int8 shouldAutoLoad, int dbInterfaceIndex);
	void onWriteToDBCallback(ENTITY_ID eid, DBID entityDBID, uint16 dbInterfaceIndex,
		CALLBACK_ID callbackID, int8 shouldAutoLoad, bool success);

		/** Network Interface
		The first time the entity writes the database to the dbid returned by dbmgr
	*/
	void onGetDBID(Network::Channel* pChannel, DBID dbid);

	/** 
		Create cell failure callback
	*/
	void onCreateCellFailure(void);

	/** 
		Create cell successfully callback
	*/
	void onGetCell(Network::Channel* pChannel, COMPONENT_ID componentID);

	/** 
		Notification of lost cell
	*/
	void onLoseCell(Network::Channel* pChannel, MemoryStream& s);

	/** 
		When the cellapp terminates unexpectedly, the baseapp will restore it if it finds the appropriate cellapp.
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
		Method of remotely calling this entity
	*/
	void onRemoteMethodCall(Network::Channel* pChannel, MemoryStream& s);

	/** 
		Destroy this entity
	*/
	void onDestroy(bool callScript);

	/**
		Destroy base internal notification
	*/
	void onDestroyEntity(bool deleteFromDB, bool writeToDB);

	/** 
		Create a cellEntity on the specified cell for a baseEntity
	*/
	DECLARE_PY_MOTHOD_ARG1(createCellEntity, PyObject_ptr);
	
	/** 
		Restore a cellEntity on the specified cell for a baseEntity
	*/
	void restoreCell(EntityCallAbstract* cellEntityCall);
	INLINE bool inRestore();

	/** 
		Create a cellEntity on a new space
	*/
	DECLARE_PY_MOTHOD_ARG1(createCellEntityInNewSpace, PyObject_ptr);

		/** Network Interface
		The client sends a message directly to the cell entity.
	*/
	void forwardEntityMessageToCellappFromClient(Network::Channel* pChannel, MemoryStream& s);
	
	/**
		Send a message to cellapp
	*/
	void sendToCellapp(Network::Bundle* pBundle);
	void sendToCellapp(Network::Channel* pChannel, Network::Bundle* pBundle);

	/**
		Send callback
	*/
	void onTeleportCB(Network::Channel* pChannel, SPACE_ID spaceID, bool fromCellTeleport);  
	void onTeleportFailure();  
	void onTeleportSuccess(SPACE_ID spaceID);

		/** Network Interface
		The entity requests the process to migrate to another cellapp to start and end.
	*/
	void onMigrationCellappStart(Network::Channel* pChannel, COMPONENT_ID sourceCellAppID, COMPONENT_ID targetCellAppID);
	void onMigrationCellappEnd(Network::Channel* pChannel, COMPONENT_ID sourceCellAppID, COMPONENT_ID targetCellAppID);
	void onMigrationCellappOver(COMPONENT_ID targetCellAppID);
	
	/**
		Set whether to get automatic archive
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
		Cellapp
	*/
	void onCellAppDeath();

	/** 
		Forward message completion
	*/
	void onBufferedForwardToCellappMessagesOver();
	void onBufferedForwardToClientMessagesOver();
	
	INLINE BaseMessagesForwardClientHandler* pBufferedSendToClientMessages();
	
	/** 
		Set whether the entity persistence data is dirty, dirty will automatically archive
	*/
	INLINE void setDirty(uint32* digest = NULL);
	INLINE bool isDirty() const;
	
protected:
	/** 
		Defining attribute data has been changed
	*/
	void onDefDataChanged(EntityComponent* pEntityComponent, const PropertyDescription* propertyDescription,
			PyObject* pyData);

	/**
		Erase online log from db
	*/
	void eraseEntityLog();

protected:
	// The client entityCall cellapp entityCall of this entity
	EntityCall*								clientEntityCall_;
	EntityCall*								cellEntityCall_;

	// After the entity is created, when the cell part is not created, save some cell attribute data here.
	PyObject*								cellDataDict_;

	// Is it an entity stored in the database?
	bool									hasDB_;
	DBID									DBID_;

	// Is it getting celldata?
	bool									isGetingCellData_;

	// Is it in the archive?
	bool									isArchiveing_;

	// Whether to auto-archive <= 0 is false, 1 is true, OURO_NEXT_ONLY is automatically false after executing once
	int8									shouldAutoArchive_;
	
	// Whether to perform automatic backup <= 0 is false, 1 is true, OURO_NEXT_ONLY is automatically false after executing once
	int8									shouldAutoBackup_;

	// Whether you are creating a cell
	bool									creatingCell_;

	// Have you created a space?
	bool									createdSpace_;

	// Is it recovering?
	bool									inRestore_;
	
	// If the entity has not been set to ENTITY_FLAGS_TELEPORT_START at this time, the onMigrationCellappArrived package takes precedence over
	// onMigrationCellappStart arrives (in case of some pressure, the entity will jump across the process (by cell1 to cell2).
	// The packet generated before the jump will arrive slower than the enterSpace packet of cell2), so when this happens, the packet of cell2 needs to be cached first.
	// After the package of cell1 arrives, execute the package of cell2.
	BaseMessagesForwardClientHandler*		pBufferedSendToClientMessages_;
	
	// Whether the data that needs to be persisted is dirty (memory sha1), no persistence if it is not dirty
	uint32									persistentDigest_[5];

	// If this entity has been written to the database, then this property is the index of the corresponding database interface.
	uint16									dbInterfaceIndex_;
};

}


#ifdef CODE_INLINE
#include "entity.inl"
#endif

#endif // OURO_BASE_ENTITY_H
