// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_SPACE_VIEWER_H
#define OURO_SPACE_VIEWER_H

#include "common/common.h"
#include "common/tasks.h"
#include "common/timer.h"
#include "helper/debug_helper.h"
#include "helper/eventhistory_stats.h"
#include "network/interfaces.h"

namespace Ouroboros {
namespace Network
{
class NetworkInterface;
class Address;
class MessageHandler;
}

class MemoryStream;

class SpaceViewer
{
public:
	struct ViewEntity
	{
		ViewEntity()
		{
			entityID = 0;
			updateVersion = 0;
		}

		ENTITY_ID entityID;
		Position3D position;
		Direction3D direction;

		// Update the serial number. All entities are updated. Serial number 1. In some cases, the volume is larger. Each iteration of part of the entity update.
		int updateVersion;
	};

public:
	SpaceViewer();
	virtual ~SpaceViewer();

	virtual void timeout();
	virtual void sendStream(MemoryStream* s, int type);

	void updateViewer(const Network::Address& addr, SPACE_ID spaceID, CELL_ID cellID);

	const Network::Address& addr() const {
		return addr_;
	}

protected:
	// Changed the cell of viewing space
	void onChangedSpaceOrCell();
	void resetViewer();

	void updateClient();
	void initClient();

	Network::Address addr_;

	// Currently viewed space and cell
	SPACE_ID spaceID_;
	CELL_ID cellID_;

	std::map< ENTITY_ID, ViewEntity > viewedEntities;

	int updateType_;

	// Update the serial number. All entities are updated. Serial number 1. In some cases, the volume is larger. Each iteration of part of the entity update.
	int lastUpdateVersion_;
};

class SpaceViewers : public TimerHandler
{
public:
	SpaceViewers();
	virtual ~SpaceViewers();

	void clear() {
		spaceViews_.clear();
	}

	bool addTimer();
	void finalise();

	void updateSpaceViewer(const Network::Address& addr, SPACE_ID spaceID, CELL_ID cellID, bool del);

protected:
	virtual void handleTimeout(TimerHandle handle, void * arg);
	TimerHandle reportLimitTimerHandle_;

	std::map< Network::Address, SpaceViewer> spaceViews_;
};

}

#endif // OURO_SPACE_VIEWER_H
