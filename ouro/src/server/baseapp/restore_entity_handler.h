// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_RESTORE_ENTITY_HANDLER_H
#define OURO_RESTORE_ENTITY_HANDLER_H

#include "helper/debug_helper.h"
#include "common/common.h"

namespace Ouroboros{

class Entity;
class EntityCall;

class RestoreEntityHandler : public Task
{
	struct RestoreData
	{
		ENTITY_ID id;
		bool creatingCell;
		bool processed;
		SPACE_ID spaceID;
		EntityCall* cell;
	};

public:
	RestoreEntityHandler(COMPONENT_ID cellappID, Network::NetworkInterface & networkInterface);
	~RestoreEntityHandler();
	
	bool process();
	
	void pushEntity(ENTITY_ID id);

	/** 
		The space on a baseapp restores the cell, and determines whether the current baseapp has an associated entity and needs to recover the cell.
	*/
	void onRestoreSpaceCellFromOtherBaseapp(COMPONENT_ID baseappID, COMPONENT_ID cellappID, 
		SPACE_ID spaceID, ENTITY_ID spaceEntityID, ENTITY_SCRIPT_UID utype, bool destroyed);

	/** 
		Whether to allow recovery
	*/
	bool canRestore() const{ return canRestore_; }
	void canRestore(bool v){ canRestore_ = v; }

	COMPONENT_ID cellappID() const{ return cellappID_; }
private:
	Network::NetworkInterface & networkInterface_;
	std::vector<RestoreData> entities_;
	bool inProcess_;

	std::vector<RestoreData> restoreSpaces_;
	std::vector<RestoreData> otherRestoredSpaces_;

	// space creates a good cell and broadcasts it to other baseapps
	bool broadcastOtherBaseapps_;

	uint64 tickReport_;

	std::vector<SPACE_ID> spaceIDs_;

	COMPONENT_ID cellappID_;

	bool canRestore_;
};


}

#endif // OURO_RESTORE_ENTITY_HANDLER_H
