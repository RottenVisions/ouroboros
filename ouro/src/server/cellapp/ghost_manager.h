// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_GHOST_MANAGER_HANDLER_H
#define OURO_GHOST_MANAGER_HANDLER_H

// common include
#include "helper/debug_helper.h"
#include "common/common.h"

namespace Ouroboros{

namespace Network
{
class Bundle;
}

class Entity;

/*
	* cell1: entity(1) is real, it is stored in the GhostManager in entityIDs_ for checking (updated to other ghosts)

	* cell2: entity(1) is ghost, if cell2 is migrated as a whole, you need to temporarily set a routing address to ghost_route_, and the route is erased after the last time the packet is received for more than a certain period of time.
	                    if some packets are forwarded during the period, then the entity can't find the entity and query the routing table and continue forwarding to the ghostEntity (for example, real destroys the request to destroy the ghost immediately).

	* cell1: entity(1) is real, if it is re-migrated to cell3, it needs to temporarily set a routing address to ghost_route_, and the route is erased after the last time the ghost request packet is received for more than a certain time.
	                    if some ghost request packets are forwarded during the period, then the entity can't find the entity and query the routing table and continue forwarding to realEntity.
*/
class GhostManager : public TimerHandler
{
public:
	GhostManager();
	~GhostManager();

	void pushMessage(COMPONENT_ID componentID, Network::Bundle* pBundle);
	void pushRouteMessage(ENTITY_ID entityID, COMPONENT_ID componentID, Network::Bundle* pBundle);

	COMPONENT_ID getRoute(ENTITY_ID entityID);
	void addRoute(ENTITY_ID entityID, COMPONENT_ID componentID);

	/**
	Create a send bundle, which may be obtained from the send into the send queue, if the queue is empty
	Then create a new one
	*/
	Network::Bundle* createSendBundle(COMPONENT_ID componentID);

private:
	virtual void handleTimeout(TimerHandle handle, void * pUser);

	virtual void onRelease( TimerHandle handle, void * /*pUser*/ ){};

	void cancel();

	void start();

private:
	void syncMessages();
	void syncGhosts();

	void checkRoute();

	struct ROUTE_INFO
	{
		ROUTE_INFO():
		componentID(0),
		lastTime(0)
		{
		}

		COMPONENT_ID componentID;
		uint64 lastTime;
	};

private:
	// all related entities with ghost
	std::map<ENTITY_ID, Entity*> 	realEntities_;
	
	//ghost routing, distributed programs can't guarantee synchronization at some time, then some entities on this machine are migrated away.
	// You may also receive some network messages at the time, because other apps may not be able to get the migration address immediately, at this time we
	// You can point the migrated entity to the cache on the current app. If there is a network message, we can continue forwarding to the new address.
	std::map<ENTITY_ID, ROUTE_INFO> ghost_route_;

	// all event messages that need to be broadcast
	std::map<COMPONENT_ID, std::vector< Network::Bundle* > > messages_;

	TimerHandle* pTimerHandle_;

	uint64 checkTime_;
};


}

#endif // OURO_GHOST_MANAGER_HANDLER_H
