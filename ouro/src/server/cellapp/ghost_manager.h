// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

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
	* cell1: entity(1) is real, In GhostManager stored in entityIDs_ check (update to other ghost)

	* cell2: entity(1) is ghost, If cell2 is to be migrated as a whole, a route address needs to be temporarily set to ghost_route_, and the route is erased after the last packet received exceeds a certain time. If some packets are forwarded during the period, then the entity
	cannot be found on the routing table and continues to forward to the ghostEntity (for example, real destroy destroys the ghost immediately).

	* cell1: entity(1) is real, If it is re-migrated to cell3, a route address needs to be temporarily set to ghost_route_. The route is erased after the last time the ghost request packet is received. If some ghost request packets are forwarded during the period,
	 then the entity is not found and the routing table is queried and forwarded to real entity.
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
	Create a send bundle. The bundle may be obtained from send into the send queue if the queue is empty
	Create a new one
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
	// All related entities with ghost
	std::map<ENTITY_ID, Entity*> 	realEntities_;

	// Ghost route, distributed programs can not guarantee synchronization at some time, then some entities on the machine have been migrated away
	// You may receive some network messages at the same time. Because other apps may not be able to get the address immediately, we
	// You can point the migrated entity to the cache on the current app. If there is a network message, we can continue forwarding to the new address.
	std::map<ENTITY_ID, ROUTE_INFO> ghost_route_;

	// All event messages that need to be broadcast
	std::map<COMPONENT_ID, std::vector< Network::Bundle* > > messages_;

	TimerHandle* pTimerHandle_;

	uint64 checkTime_;
};


}

#endif // OURO_GHOST_MANAGER_HANDLER_H
