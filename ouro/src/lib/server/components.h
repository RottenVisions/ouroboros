// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_ENGINE_COMPONENT_MGR_H
#define OURO_ENGINE_COMPONENT_MGR_H
	
#include "common/timer.h"
#include "common/tasks.h"
#include "common/common.h"
#include "common/singleton.h"
#include "thread/threadmutex.h"
#include "thread/threadguard.h"
#include "server/common.h"
	
namespace Ouroboros{

namespace Network
{
class Channel;
class Address;
class NetworkInterface;
}

// ComponentInfos.flags flag
#define COMPONENT_FLAG_NORMAL 0x00000000
#define COMPONENT_FLAG_SHUTTINGDOWN 0x00000001

class Components : public Task, public Singleton<Components>
{
public:
	static int32 ANY_UID; 

	struct ComponentInfos
	{
		ComponentInfos()
		{
			uid = 0;
			flags = COMPONENT_FLAG_NORMAL;
			cid = 0;

			groupOrderid = 0;
			globalOrderid = 0;
			gus = 0;

			pChannel = NULL;
			state = COMPONENT_STATE_INIT;
			mem = cpu = 0.f;
			usedmem = 0;

			extradata = extradata1 = extradata2 = 0;

			pid = 0;
			externalAddressEx[0] = '\0';
			logTime = timestamp();
			appFlags = APP_FLAGS_NONE;
		}

		OUROShared_ptr<Network::Address> pIntAddr, pExtAddr; // internal and external addresses
		char externalAddressEx[MAX_NAME + 1]; // Forced exposure to an external public address, see externalAddressEx in the configuration

		int32 uid;
		COMPONENT_ID cid;
		COMPONENT_ORDER groupOrderid, globalOrderid;
		COMPONENT_GUS gus;
		char username[MAX_NAME + 1];
		Network::Channel* pChannel;
		COMPONENT_TYPE componentType;
		uint32 flags;

		// process status
		COMPONENT_STATE state;

		float cpu;
		float mem;
		uint32 usedmem;
		uint64 extradata, extradata1, extradata2, extradata3;
		uint32 pid;
		uint64 logTime;
		uint32 appFlags;
	};

	typedef std::vector<ComponentInfos> COMPONENTS;

		/** Component add delete handler*/
	class ComponentsNotificationHandler
	{
	public:
		virtual ~ComponentsNotificationHandler() {};
		virtual void onAddComponent(const Components::ComponentInfos*) = 0;
		virtual void onRemoveComponent(const Components::ComponentInfos*) = 0;
		virtual void onIdentityillegal(COMPONENT_TYPE componentType, COMPONENT_ID componentID, uint32 pid, const char* pAddr) = 0;
	};

public:
	Components();
	~Components();

	void initialize(Network::NetworkInterface * pNetworkInterface, COMPONENT_TYPE componentType, COMPONENT_ID componentID);
	void finalise();

	INLINE Network::NetworkInterface* pNetworkInterface()
	{ 
		return _pNetworkInterface;
	}

	void addComponent(int32 uid, const char* username, 
		COMPONENT_TYPE componentType, COMPONENT_ID componentID, COMPONENT_ORDER globalorderid, COMPONENT_ORDER grouporderid, COMPONENT_GUS gus,
		uint32 intaddr, uint16 intport, uint32 extaddr, uint16 extport, std::string& extaddrEx, uint32 pid,
		float cpu, float mem, uint32 usedmem, uint64 extradata, uint64 extradata1, uint64 extradata2, uint64 extradata3,
		Network::Channel* pChannel = NULL);

	void delComponent(int32 uid, COMPONENT_TYPE componentType, 
		COMPONENT_ID componentID, bool ignoreComponentID = false, bool shouldShowLog = true);

	void removeComponentByChannel(Network::Channel * pChannel, bool isShutingdown = false);

	void clear(int32 uid = -1, bool shouldShowLog = true);

	Components::COMPONENTS& getComponents(COMPONENT_TYPE componentType);

	/** 
		Find component
	*/
	Components::ComponentInfos* findComponent(COMPONENT_TYPE componentType, int32 uid, COMPONENT_ID componentID);
	Components::ComponentInfos* findComponent(COMPONENT_TYPE componentType, COMPONENT_ID componentID);
	Components::ComponentInfos* findComponent(COMPONENT_ID componentID);
	Components::ComponentInfos* findComponent(Network::Channel * pChannel);
	Components::ComponentInfos* findComponent(Network::Address* pAddress);

	/** 
		Find local components by process id
	*/
	Components::ComponentInfos* findLocalComponent(uint32 pid);

	int connectComponent(COMPONENT_TYPE componentType, int32 uid, COMPONENT_ID componentID, bool printlog = true);

	typedef std::map<int32/*uid*/, COMPONENT_ORDER/*lastorder*/> ORDER_LOG;
	ORDER_LOG& getGlobalOrderLog(){ return _globalOrderLog; }
	ORDER_LOG& getBaseappGroupOrderLog(){ return _baseappGrouplOrderLog; }
	ORDER_LOG& getCellappGroupOrderLog(){ return _cellappGrouplOrderLog; }
	ORDER_LOG& getLoginappGroupOrderLog(){ return _loginappGrouplOrderLog; }
	
	/** 
		Check all components to prevent duplicate uuid, and you should get an error.
	*/
	bool checkComponents(int32 uid, COMPONENT_ID componentID, uint32 pid);

	/** 
		Set up a processor instance to receive component notifications
	*/
	void pHandler(ComponentsNotificationHandler* ph){ _pHandler = ph; };

	/** 
		Check if a component port is valid.
	*/
	bool updateComponentInfos(const Components::ComponentInfos* info);

	/** 
		Whether it is a local component.
	*/
	bool isLocalComponent(const Components::ComponentInfos* info);

	/** 
		Whether the local component is running.
	*/
	const Components::ComponentInfos* lookupLocalComponentRunning(uint32 pid);

	Components::ComponentInfos* getBaseappmgr();
	Components::ComponentInfos* getCellappmgr();
	Components::ComponentInfos* getDbmgr();
	Components::ComponentInfos* getLogger();
	Components::ComponentInfos* getInterfaceses();

	Network::Channel* getBaseappmgrChannel();
	Network::Channel* getCellappmgrChannel();
	Network::Channel* getDbmgrChannel();
	Network::Channel* getLoggerChannel();

	/**
		Count the number of all components under a UID
	*/
	size_t getGameSrvComponentsSize(int32 uid);

	/** 
		Get the number of registrations required for the game server.
	*/
	size_t getGameSrvComponentsSize();

	void componentID(COMPONENT_ID id){ componentID_ = id; }
	COMPONENT_ID componentID() const { return componentID_; }
	void componentType(COMPONENT_TYPE t){ componentType_ = t; }
	COMPONENT_TYPE componentType() const { return componentType_; }
	
	Network::EventDispatcher & dispatcher();

	void onChannelDeregister(Network::Channel * pChannel, bool isShutingdown);

	void extraData1(uint64 v){ extraData1_ = v; }
	void extraData2(uint64 v){ extraData2_ = v; }
	void extraData3(uint64 v){ extraData3_ = v; }
	void extraData4(uint64 v){ extraData4_ = v; }

	bool findLogger();
	
	void broadcastSelf();

private:
	virtual bool process();
	bool findComponents();

	void onFoundAllComponents();

private:
	COMPONENTS								_baseapps;
	COMPONENTS								_cellapps;
	COMPONENTS								_dbmgrs;
	COMPONENTS								_loginapps;
	COMPONENTS								_cellappmgrs;
	COMPONENTS								_baseappmgrs;
	COMPONENTS								_machines;
	COMPONENTS								_loggers;
	COMPONENTS								_interfaceses;
	COMPONENTS								_bots;
	COMPONENTS								_consoles;

	Network::NetworkInterface*				_pNetworkInterface;
	
	// The global startup order log and group of the component (the same component is a group, such as: all baseapp is a group) startup order log
	// Note: There is a dead app component in the middle of the log. The log does not do the subtraction operation. It is not necessary to make this match from the intention of use.
	ORDER_LOG								_globalOrderLog;
	ORDER_LOG								_baseappGrouplOrderLog;
	ORDER_LOG								_cellappGrouplOrderLog;
	ORDER_LOG								_loginappGrouplOrderLog;

	ComponentsNotificationHandler*			_pHandler;

	// The category of this component
	COMPONENT_TYPE							componentType_;

	// the ID of this component
	COMPONENT_ID							componentID_;	

	uint8									state_;
	int16									findIdx_;
	int8									findComponentTypes_[8];

	uint64									extraData1_;
	uint64									extraData2_;
	uint64									extraData3_;
	uint64									extraData4_;
};

}

#endif // OURO_ENGINE_COMPONENT_MGR_H
