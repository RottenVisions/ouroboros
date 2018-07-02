// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com


#ifndef OURO_CELLAPPMGR_H
#define OURO_CELLAPPMGR_H

#include "cellapp.h"
#include "space_viewer.h"
#include "server/ouromain.h"
#include "server/serverapp.h"
#include "server/idallocate.h"
#include "server/serverconfig.h"
#include "server/forward_messagebuffer.h"
#include "common/timer.h"
#include "network/endpoint.h"

namespace Ouroboros{


class Cellappmgr :	public ServerApp,
					public Singleton<Cellappmgr>
{
public:
	enum TimeOutType
	{
		TIMEOUT_GAME_TICK = TIMEOUT_SERVERAPP_MAX + 1
	};

	Cellappmgr(Network::EventDispatcher& dispatcher,
		Network::NetworkInterface& ninterface,
		COMPONENT_TYPE componentType,
		COMPONENT_ID componentID);

	~Cellappmgr();

	bool run();

	virtual void onChannelDeregister(Network::Channel * pChannel);

	void handleTimeout(TimerHandle handle, void * arg);
	void handleGameTick();

	/* Initialize related interfaces */
	bool initializeBegin();
	bool inInitialize();
	bool initializeEnd();
	void finalise();

	/** Finding the most free cellapp */
	COMPONENT_ID findFreeCellapp(void);
	void updateBestCellapp();

	/** Network Interface
		baseEntity request created in a new space
	*/
	void reqCreateCellEntityInNewSpace(Network::Channel* pChannel, MemoryStream& s);

	/** Network Interface
		baseEntity request created in a new space
	*/
	void reqRestoreSpaceInCell(Network::Channel* pChannel, MemoryStream& s);

	/** Network Interface
		The message is forwarded by an app that wants to forward the message to an app through the app.
	*/
	void forwardMessage(Network::Channel* pChannel, MemoryStream& s);

	/** Network Interface
		Update cellapp situation.
	*/
	void updateCellapp(Network::Channel* pChannel, COMPONENT_ID componentID, ENTITY_ID numEntities, float load, uint32 flags);

	/** Network Interface
		Cellapp synchronizes its own initialization information
		startGlobalOrder: Global startup sequence includes a variety of different components
		startGroupOrder: The start order in the group, such as the first few start in all baseapps.
	*/
	void onCellappInitProgress(Network::Channel* pChannel, COMPONENT_ID cid, float progress,
		COMPONENT_ORDER componentGlobalOrder, COMPONENT_ORDER componentGroupOrder);

	bool componentsReady();
	bool componentReady(COMPONENT_ID cid);

	void removeCellapp(COMPONENT_ID cid);
	Cellapp& getCellapp(COMPONENT_ID cid);
	std::map< COMPONENT_ID, Cellapp >& cellapps();

	uint32 numLoadBalancingApp();

	/* Take groupOrderID as the sorting benchmark
	   Add a cellapp component id to the list of cellapp_cids_
	*/
	void addCellappComponentID(COMPONENT_ID cid);

	/** Network Interface
	Query all relevant process load information
	*/
	void queryAppsLoads(Network::Channel* pChannel, MemoryStream& s);

	/** Network Interface
	Query all relevant process space information
	*/
	void querySpaces(Network::Channel* pChannel, MemoryStream& s);

	/** Network Interface
	Update the relevant process space information. Note that this spaceData is not the spaceData described in the API documentation.
	Refers to space information
	*/
	void updateSpaceData(Network::Channel* pChannel, MemoryStream& s);

	/** Network Interface
	Tool request to change the space viewer (including add and delete functions)
	If the viewer is requesting an update and the address does not exist on the server, it is automatically created. If it is deleted, the delete request is explicitly given.
	*/
	void setSpaceViewer(Network::Channel* pChannel, MemoryStream& s);

protected:
	TimerHandle							gameTimer_;
	ForwardAnywhere_MessageBuffer		forward_anywhere_cellapp_messagebuffer_;
	ForwardComponent_MessageBuffer		forward_cellapp_messagebuffer_;

	COMPONENT_ID						bestCellappID_;

	std::map< COMPONENT_ID, Cellapp >	cellapps_;
	std::vector<COMPONENT_ID>			cellapp_cids_;

	// View space through tools
	SpaceViewers						spaceViewers_;
};

}

#endif // OURO_CELLAPPMGR_H
