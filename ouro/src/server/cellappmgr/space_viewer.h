// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

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
	SpaceViewer();
	virtual ~SpaceViewer();
	
	virtual void timeout();
	virtual void sendStream(MemoryStream* s);

	void updateViewer(const Network::Address& addr, SPACE_ID spaceID);

	const Network::Address& addr() const {
		return addr_;
	}

protected:
	void updateClient();

	Network::Address addr_;

	// currently viewed space
	SPACE_ID spaceID_;
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

	void updateSpaceViewer(const Network::Address& addr, SPACE_ID spaceID, bool del);

protected:
	virtual void handleTimeout(TimerHandle handle, void * arg);
	TimerHandle reportLimitTimerHandle_;

	std::map< Network::Address, SpaceViewer> spaceViews_;
};

}

#endif // OURO_SPACE_VIEWER_H
