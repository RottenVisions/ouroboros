// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com


#ifndef OURO_BASEAPPMGR_H
#define OURO_BASEAPPMGR_H

#include "baseapp.h"
#include "server/ouromain.h"
#include "server/serverapp.h"
#include "server/idallocate.h"
#include "server/serverconfig.h"
#include "server/forward_messagebuffer.h"
#include "common/timer.h"
#include "network/endpoint.h"

namespace Ouroboros{

class Baseappmgr :	public ServerApp,
					public Singleton<Baseappmgr>
{
public:
	enum TimeOutType
	{
		TIMEOUT_GAME_TICK = TIMEOUT_SERVERAPP_MAX + 1
	};

	Baseappmgr(Network::EventDispatcher& dispatcher,
		Network::NetworkInterface& ninterface,
		COMPONENT_TYPE componentType,
		COMPONENT_ID componentID);

	~Baseappmgr();

	bool run();

	virtual void onChannelDeregister(Network::Channel * pChannel);
	virtual void onAddComponent(const Components::ComponentInfos* pInfos);

	void handleTimeout(TimerHandle handle, void * arg);
	void handleGameTick();

	/* Initialize related interfaces */
	bool initializeBegin();
	bool inInitialize();
	bool initializeEnd();
	void finalise();

	COMPONENT_ID findFreeBaseapp();
	void updateBestBaseapp();

	/** Network Interface
		baseapp::createEntityAnywhere queries the current best component ID
	*/
	void reqCreateEntityAnywhereFromDBIDQueryBestBaseappID(Network::Channel* pChannel, MemoryStream& s);

	/** Network Interface
		Received a baseapp::createEntityAnywhere requests to create a baseEntity on a free baseapp
		@param sp: This packet is stored in entityType	: Entity category, entities.Defined in xml.
										strInitData	: Some data that should be initialized after this entity is created,
													  Need to use pickle.loads Unpack.
										componentID	: The component ID of the baseapp requesting to create an entity
	*/
	void reqCreateEntityAnywhere(Network::Channel* pChannel, MemoryStream& s);

	/** Network Interface
	Received a baseapp::createEntityRemotely request to create a baseEntity on an idle baseapp
	@param sp: The data stored in this packet is entityType : the category of entity, as defined in entities.xml.
	strInitData	: Some data that should be initialized after this entity is created,
	Need to use pickle.loads to unpack.
	componentID	: The component ID of the baseapp requesting to create an entity
	*/
	void reqCreateEntityRemotely(Network::Channel* pChannel, MemoryStream& s);

	/** Network Interface
		Received baseapp::createEntityAnywhereFromDBID request to create a baseEntity on a free baseapp
	*/
	void reqCreateEntityAnywhereFromDBID(Network::Channel* pChannel, MemoryStream& s);

	/** Network Interface
		Received baseapp::createEntityRemotelyFromDBID request to create a baseEntity on an idle baseapp
	*/
	void reqCreateEntityRemotelyFromDBID(Network::Channel* pChannel, MemoryStream& s);

	/** Network Interface
		The message is forwarded by an app that wants to forward the message to an app through the app.
	*/
	void forwardMessage(Network::Channel* pChannel, MemoryStream& s);

	/** Network Interface
		A newly logged in account has the right to legally login to baseapp. Now you need to register your account with baseapp.
		Make it allow login on this baseapp.
	*/
	void registerPendingAccountToBaseapp(Network::Channel* pChannel, MemoryStream& s);

	/** Network Interface
		A newly logged in account has the right to legally login to baseapp. Now it is necessary to register the account to the specified baseapp
		Make it allow login on this baseapp.
	*/
	void registerPendingAccountToBaseappAddr(Network::Channel* pChannel, MemoryStream& s);

	/** Network Interface
		Baseapp sends its own address to the loginapp and forwards it to the client.
	*/
	void onPendingAccountGetBaseappAddr(Network::Channel* pChannel,
								  std::string& loginName, std::string& accountName,
								  std::string& addr, uint16 tcp_port, uint16 udp_port);

	/** Network Interface
		Update the baseapp situation.
	*/
	void updateBaseapp(Network::Channel* pChannel, COMPONENT_ID componentID,
								ENTITY_ID numEntitys, ENTITY_ID numProxices, float load, uint32 flags);

	/** Network Interface
		Baseapp synchronizes its own initialization information
		startGlobalOrder: Global startup sequence includes a variety of different components
		startGroupOrder: The start order in the group, such as the first few start in all baseapps.
	*/
	void onBaseappInitProgress(Network::Channel* pChannel, COMPONENT_ID cid, float progress);

	/**
		The assigned baseapp address is sent to the loginapp and forwarded to the client.
	*/
	void sendAllocatedBaseappAddr(Network::Channel* pChannel,
								  std::string& loginName, std::string& accountName,
								  const std::string& addr, uint16 tcp_port, uint16 udp_port);

	bool componentsReady();
	bool componentReady(COMPONENT_ID cid);

	std::map< COMPONENT_ID, Baseapp >& baseapps();

	uint32 numLoadBalancingApp();

	/** Network Interface
		Query all relevant process load information
	*/
	void queryAppsLoads(Network::Channel* pChannel, MemoryStream& s);

	/** Network Interface
		Baseapp request binding e-mail (return to need to find loginapp address)
	*/
	void reqAccountBindEmailAllocCallbackLoginapp(Network::Channel* pChannel, COMPONENT_ID reqBaseappID, ENTITY_ID entityID, std::string& accountName, std::string& email,
		SERVER_ERROR_CODE failedcode, std::string& code);

	/** Network Interface
		Request binding email, loginapp returns the address of the need to find loginapp
	*/
	void onReqAccountBindEmailCBFromLoginapp(Network::Channel* pChannel, COMPONENT_ID reqBaseappID, ENTITY_ID entityID, std::string& accountName, std::string& email,
		SERVER_ERROR_CODE failedcode, std::string& code, std::string& loginappCBHost, uint16 loginappCBPort);

protected:
	TimerHandle													gameTimer_;

	ForwardAnywhere_MessageBuffer								forward_anywhere_baseapp_messagebuffer_;
	ForwardComponent_MessageBuffer								forward_baseapp_messagebuffer_;

	COMPONENT_ID												bestBaseappID_;

	std::map< COMPONENT_ID, Baseapp >							baseapps_;

	OUROUnordered_map< std::string, COMPONENT_ID >				pending_logins_;

	float														baseappsInitProgress_;
};

}

#endif // OURO_BASEAPPMGR_H
