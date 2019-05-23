// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_MACHINE_H
#define OURO_MACHINE_H
	
// common include	
#include "server/ouromain.h"
#include "server/serverapp.h"
#include "server/idallocate.h"
#include "server/serverconfig.h"
#include "server/components.h"
#include "common/timer.h"
#include "network/endpoint.h"
#include "network/udp_packet_receiver.h"
#include "network/common.h"

//#define NDEBUG
#include <map>	
// windows include	
#if OURO_PLATFORM == PLATFORM_WIN32
#else
// linux include
#endif
	
namespace Ouroboros{

class Machine:	public ServerApp, 
				public Singleton<Machine>
{
public:
	enum TimeOutType
	{
		TIMEOUT_GAME_TICK = TIMEOUT_SERVERAPP_MAX + 1
	};
	
	Machine(Network::EventDispatcher& dispatcher, 
		Network::NetworkInterface& ninterface, 
		COMPONENT_TYPE componentType,
		COMPONENT_ID componentID);

	~Machine();
	
	bool run();
	
	bool findBroadcastInterface();

		/** Network Interface
		An app broadcasts its own address
	*/
	void onBroadcastInterface(Network::Channel* pChannel, int32 uid, std::string& username, 
							COMPONENT_TYPE componentType, COMPONENT_ID componentID, COMPONENT_ID componentIDEx, 
							COMPONENT_ORDER globalorderid, COMPONENT_ORDER grouporderid, COMPONENT_GUS gus,
							uint32 intaddr, uint16 intport, uint32 extaddr, uint16 extport, std::string& extaddrEx, uint32 pid,
							float cpu, float mem, uint32 usedmem, int8 state, uint32 machineID, uint64 extradata,
							uint64 extradata1, uint64 extradata2, uint64 extradata3, uint32 backRecvAddr, uint16 backRecvPort);
	
		/** Network Interface
		An app looks for the address of another app
	*/
	void onFindInterfaceAddr(Network::Channel* pChannel, int32 uid, std::string& username, 
		COMPONENT_TYPE componentType, COMPONENT_ID componentID, COMPONENT_TYPE findComponentType, uint32 finderAddr, uint16 finderRecvPort);

		/** Network Interface
		Query all interface information
	*/
	void onQueryAllInterfaceInfos(Network::Channel* pChannel, int32 uid, std::string& username, 
		uint16 finderRecvPort);

		/** Network Interface
	Query all machine processes
	*/
	void onQueryMachines(Network::Channel* pChannel, int32 uid, std::string& username,
		uint16 finderRecvPort);

	void queryComponentID(Network::Channel* pChannel, COMPONENT_TYPE componentType, COMPONENT_ID componentID,
		int32 uid, uint16 finderRecvPort, int macMD5, int32 pid);

	void removeComponentID(COMPONENT_TYPE componentType, COMPONENT_ID componentID, int32 uid);

	void handleTimeout(TimerHandle handle, void * arg);

		/*Initialize related interfaces*/
	bool initializeBegin();
	bool inInitialize();
	bool initializeEnd();
	void finalise();
	bool initNetwork();

		/** Network Interface
		Start the server
		@uid: Provide the startup uid parameter
		@components: Which components are started (possibly taking a distributed startup scheme)
	*/
	void startserver(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	/** Signal Processing
	*/
	virtual bool installSignals();
	virtual void onSignalled(int sigNum);

#if OURO_PLATFORM != PLATFORM_WIN32
	/**
	* Start a new process under linux
	*/
	uint16 startLinuxProcess(int32 uid, COMPONENT_TYPE componentType, uint64 cid, uint16 gus, 
		std::string& OURO_ROOT, std::string& OURO_RES_PATH, std::string& OURO_BIN_PATH);
#else
	/**
	* Start a new process under windows
	*/
	DWORD startWindowsProcess(int32 uid, COMPONENT_TYPE componentType, uint64 cid, uint16 gus, 
		std::string& OURO_ROOT, std::string& OURO_RES_PATH, std::string& OURO_BIN_PATH);
#endif

		/** Network Interface
		Turn off the server
		@uid: Provide the startup uid parameter
	*/
	void stopserver(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

		/** Network Interface
	Kill the server
	@uid: Provide the startup uid parameter
	*/
	void killserver(Network::Channel* pChannel, Ouroboros::MemoryStream& s);

	/**
		Check if the components running on this machine are available
	*/
	bool checkComponentUsable(const Components::ComponentInfos* info, bool getdatas, bool autoerase);

protected:
	// udp broadcast address
	u_int32_t					broadcastAddr_;
	Network::EndPoint			ep_;
	Network::EndPoint			epBroadcast_;

	Network::EndPoint			epLocal_;

	Network::UDPPacketReceiver* pEPPacketReceiver_;
	Network::UDPPacketReceiver* pEBPacketReceiver_;
	Network::UDPPacketReceiver* pEPLocalPacketReceiver_;

	// uid used by this machine
	std::vector<int32>			localuids_;

	typedef std::vector<COMPONENT_ID> ID_LOGS;
	typedef std::map<COMPONENT_TYPE, ID_LOGS> CID_MAP;

	std::map<int32, CID_MAP>		cidMap_;
	std::map<std::string, COMPONENT_ID>		pidMD5Map_;
};

}

#endif // OURO_MACHINE_H
