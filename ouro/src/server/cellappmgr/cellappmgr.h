// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com


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

		/*Initialize related interfaces*/
	bool initializeBegin();
	bool inInitialize();
	bool initializeEnd();
	void finalise();

		/** Find one of the most idle cellapps*/
	COMPONENT_ID findFreeCellapp(void);
	void updateBestCellapp();

		/** Network Interface
		baseEntity request is created in a new space
	*/
	void reqCreateCellEntityInNewSpace(Network::Channel* pChannel, MemoryStream& s);

		/** Network Interface
		baseEntity request is created in a new space
	*/
	void reqRestoreSpaceInCell(Network::Channel* pChannel, MemoryStream& s);
	
		/** Network Interface
		Message forwarding, an app wants to forward a message to an app through this app.
	*/
	void forwardMessage(Network::Channel* pChannel, MemoryStream& s);

		/** Network Interface
		Update the cellapp situation.
	*/
	void updateCellapp(Network::Channel* pChannel, COMPONENT_ID componentID, ENTITY_ID numEntities, float load, uint32 flags);

		/** Network Interface
		Cellapp synchronizes its own initialization information
		startGlobalOrder: global startup sequence including various components
		startGroupOrder: The startup order within the group, such as the first few starts in all baseapps.
	*/
	void onCellappInitProgress(Network::Channel* pChannel, COMPONENT_ID cid, float progress, 
		COMPONENT_ORDER componentGlobalOrder, COMPONENT_ORDER componentGroupOrder);

	bool componentsReady();
	bool componentReady(COMPONENT_ID cid);

	void removeCellapp(COMPONENT_ID cid);
	Cellapp& getCellapp(COMPONENT_ID cid);
	std::map< COMPONENT_ID, Cellapp >& cellapps();

	uint32 numLoadBalancingApp();

	/* is based on groupOrderID,
	   Add a cellapp component id to the cellapp_cids_ list
	*/
	void addCellappComponentID(COMPONENT_ID cid);

		/** Network Interface
	Query all relevant process load information
	*/
	void queryAppsLoads(Network::Channel* pChannel, MemoryStream& s);

		/** Network Interface
	Query all related process space information
	*/
	void querySpaces(Network::Channel* pChannel, MemoryStream& s);

		/** Network Interface
	Update the relevant process space information. Note: This spaceData is not the spaceData described in the API documentation.
	Refers to some information about space
	*/
	void updateSpaceData(Network::Channel* pChannel, MemoryStream& s);

		/** Network Interface
	Tool request to change the space viewer (with add and delete functions)
	if it is a viewer that requests an update and the address does not exist on the server, it is automatically created, and if it is deleted, the deletion request is explicitly given.
	*/
	void setSpaceViewer(Network::Channel* pChannel, MemoryStream& s);

protected:
	TimerHandle							gameTimer_;
	ForwardAnywhere_MessageBuffer		forward_anywhere_cellapp_messagebuffer_;
	ForwardComponent_MessageBuffer		forward_cellapp_messagebuffer_;

	COMPONENT_ID						bestCellappID_;

	std::map< COMPONENT_ID, Cellapp >	cellapps_;
	std::vector<COMPONENT_ID>			cellapp_cids_;

	// View space by tool
	SpaceViewers						spaceViewers_;
};

} 

#endif // OURO_CELLAPPMGR_H
