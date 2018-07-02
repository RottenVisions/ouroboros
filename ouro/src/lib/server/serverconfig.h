// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

/*
		ServerConfig::getSingleton().loadConfig("../../res/server/Ouroboros.xml");
		ENGINE_COMPONENT_INFO& ecinfo = ServerConfig::getSingleton().getCellApp();
*/
#ifndef OURO_SERVER_CONFIG_H
#define OURO_SERVER_CONFIG_H

#define __LIB_DLLAPI__

#include "common/common.h"
#if OURO_PLATFORM == PLATFORM_WIN32
#pragma warning (disable : 4996)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <stdarg.h>
#include "common/singleton.h"
#include "thread/threadmutex.h"
#include "thread/threadguard.h"
#include "xml/xml.h"


namespace Ouroboros{
namespace Network
{
class Address;
}

struct Profiles_Config
{
	Profiles_Config():
		open_pyprofile(false),
		open_cprofile(false),
		open_eventprofile(false),
		open_networkprofile(false)
	{
	}

	bool open_pyprofile;
	bool open_cprofile;
	bool open_eventprofile;
	bool open_networkprofile;
};

struct ChannelCommon
{
	float channelInternalTimeout;
	float channelExternalTimeout;
	uint32 extReadBufferSize;
	uint32 extWriteBufferSize;
	uint32 intReadBufferSize;
	uint32 intWriteBufferSize;
	uint32 intReSendInterval;
	uint32 intReSendRetries;
	uint32 extReSendInterval;
	uint32 extReSendRetries;
};

struct EmailServerInfo
{
	std::string smtp_server;
	uint32 smtp_port;
	std::string username;
	std::string password;
	uint8 smtp_auth;
};

struct EmailSendInfo
{
	std::string subject;
	std::string message;
	std::string backlink_success_message, backlink_fail_message, backlink_hello_message;

	uint32 deadline;
};

struct DBInterfaceInfo
{
	DBInterfaceInfo()
	{
		index = 0;
		isPure = false;
		db_numConnections = 5;
		db_passwordEncrypt = true;

		memset(name, 0, sizeof(name));
		memset(db_type, 0, sizeof(db_type));
		memset(db_ip, 0, sizeof(db_ip));
		memset(db_username, 0, sizeof(db_username));
		memset(db_password, 0, sizeof(db_password));
		memset(db_name, 0, sizeof(db_name));
	}

	int index;
	bool isPure;											// Whether it is a pure library (an entity table created without an engine)
	char name[MAX_BUF];										// The interface name of the database
	char db_type[MAX_BUF];									// Database category
	uint32 db_port;											// Database port
	char db_ip[MAX_BUF];									// Database ip address
	char db_username[MAX_NAME];								// Database username
	char db_password[MAX_BUF * 10];							// Database password
	bool db_passwordEncrypt;								// Db password is encrypted
	char db_name[MAX_NAME];									// Database name
	uint16 db_numConnections;								// Maximum database connection
	std::string db_unicodeString_characterSet;				// Set the database character set
	std::string db_unicodeString_collation;
};

// Engine component information structure
typedef struct EngineComponentInfo
{
	EngineComponentInfo()
	{
		tcp_SOMAXCONN = 5;
		notFoundAccountAutoCreate = false;
		account_registration_enable = false;
		account_reset_password_enable = false;
		use_coordinate_system = true;
		account_type = 3;
		debugDBMgr = false;

		externalAddress[0] = '\0';

		isOnInitCallPropertysSetMethods = true;
		forceInternalLogin = false;
	}

	~EngineComponentInfo()
	{
	}

	uint32 port;											// The port that the component is listening on after running
	char ip[MAX_BUF];										// Runtime ip address of the component

	std::vector< std::string > machine_addresses;			// All machine addresses given in the configuration

	char entryScriptFile[MAX_NAME];							// Component's entry script file
	char dbAccountEntityScriptType[MAX_NAME];				// Database account script category
	float defaultViewRadius;								// The size of the view radius of the player configured in the cellapp node
	float defaultViewHysteresisArea;						// The lag range of the view of the player configured in the cellapp node
	uint16 witness_timeout;									// Observer default timeout (seconds)
	const Network::Address* externalTcpAddr;				// External address
	const Network::Address* externalUdpAddr;				// External address
	const Network::Address* internalTcpAddr;				// Internal address
	COMPONENT_ID componentID;

	float ghostDistance;									// Ghost zone distance
	uint16 ghostingMaxPerCheck;								// Check the number of ghosts per second
	uint16 ghostUpdateHertz;								// Ghost update hz

	bool use_coordinate_system;								// Whether to use coordinate system If false, view, trap, move and other functions will no longer be maintained
	bool coordinateSystem_hasY;								// The scope manager is to manage the Y-axis. Note: With the y-axis, functions such as view and trap have a high degree, but the management of the y-axis will bring about a certain amount of consumption.
	uint16 entity_posdir_additional_updates;				// After the entity position stops changing, the engine continues to update the tick position information to the client, and 0 is always updated.

	bool aliasEntityID;										// Optimize EntityID, less than 255 EntityIDs in view scope, 1 byte pseudo ID when transferring to client
	bool entitydefAliasID;									// Optimize the bandwidth and bandwidth occupied by the entity attribute and method. When the entity client attribute or the client does not exceed 255, use the 1-byte alias ID when transferring the method uid and the attribute uid to the client.

	char internalInterface[MAX_NAME];						// Internal network interface name
	char externalInterface[MAX_NAME];						// External NIC interface name
	char externalAddress[MAX_NAME];							// External IP address

	int32 externalTcpPorts_min;								// External socket TCP port using the specified range
	int32 externalTcpPorts_max;

	int32 externalUdpPorts_min;								// External socket UDP port using the specified range
	int32 externalUdpPorts_max;

	std::vector<DBInterfaceInfo> dbInterfaceInfos;			// Database interface
	bool notFoundAccountAutoCreate;							// When the game is legal, the game database can't find the game account automatically.
	bool allowEmptyDigest;									// Whether to check defs-MD5
	bool account_registration_enable;						// Whether open registration
	bool account_reset_password_enable;						// Whether to open password reset function

	float archivePeriod;									// Entity storage database cycle
	float backupPeriod;										// Entity backup cycle
	bool backUpUndefinedProperties;							// Whether the entity backs up undefined attributes
	uint16 entityRestoreSize;								// entity Restore the number of ticks per tick

	float loadSmoothingBias;								// Baseapp load filter balance adjustment value,
	uint32 login_port;										// Server login port currently bots in use
	char login_ip[MAX_BUF];									// Server login IP address

	ENTITY_ID ids_criticallyLowSize;						// Apply for new id resource to dbmgr when there are more than one id remaining
	ENTITY_ID ids_increasing_range;							// ID increments each time the ID is applied

	uint32 downloadBitsPerSecondTotal;						// The total download bandwidth per client per second
	uint32 downloadBitsPerSecondPerClient;					// Download bandwidth per second per client

	Profiles_Config profiles;

	uint32 defaultAddBots_totalCount;						// Automatically add so many bots after the default start process Add Total
	float defaultAddBots_tickTime;							// Automatically add so many bots after the default startup process (s)
	uint32 defaultAddBots_tickCount;						// Automatically add so many bots after starting the process by default Add quantity each time

	bool forceInternalLogin;								// For the solution of the baseAddress's externalAddress, when the externalAddress forces the IP address of the public network to provide the login,
															// If the internal LAN uses robot testing, it may not be appropriate to use public IP and traffic. At this time, it can be set to true. If you log in, you must use the intranet environment directly.

	std::string bots_account_name_prefix;					// Robot account name prefix
	uint32 bots_account_name_suffix_inc;					// The suffix of the robot account name is incremented, 0 is incremented using a random number, otherwise it is incremented according to the number of baseNum fills
	std::string bots_account_passwd;						// Robot account password

	uint32 tcp_SOMAXCONN;									// Listen listener queue maximum

	int8 encrypt_login;										// Encrypt login information

	uint32 telnet_port;
	std::string telnet_passwd;
	std::string telnet_deflayer;

	uint32 perSecsDestroyEntitySize;						// Destruction of the number of entities per second

	uint64 respool_timeout;
	uint32 respool_buffersize;

	uint8 account_type;										// 1: Ordinary account, 2: email account (need to activate), 3: smart account (automatically identify email, normal number, etc.)
	uint32 accountDefaultFlags;								// New account default tag (ACCOUNT_FLAGS can be superimposed and filled in decimal format)
	uint64 accountDefaultDeadline;							// The new account default expiration time (seconds, the engine will add the current time)

	std::string http_cbhost;
	uint16 http_cbport;										// User http callback interface, handle authentication, password reset, etc.

	bool debugDBMgr;										// Debug mode can output read and write operation information

	bool isOnInitCallPropertysSetMethods;					//Dedicated to bots: whether set_* events are triggered when the Entity is initialized
} ENGINE_COMPONENT_INFO;

class ServerConfig : public Singleton<ServerConfig>
{
public:
	ServerConfig();
	~ServerConfig();

	bool loadConfig(std::string fileName);

	INLINE ENGINE_COMPONENT_INFO& getCellApp(void);
	INLINE ENGINE_COMPONENT_INFO& getBaseApp(void);
	INLINE ENGINE_COMPONENT_INFO& getDBMgr(void);
	INLINE ENGINE_COMPONENT_INFO& getLoginApp(void);
	INLINE ENGINE_COMPONENT_INFO& getCellAppMgr(void);
	INLINE ENGINE_COMPONENT_INFO& getBaseAppMgr(void);
	INLINE ENGINE_COMPONENT_INFO& getKBMachine(void);
	INLINE ENGINE_COMPONENT_INFO& getBots(void);
	INLINE ENGINE_COMPONENT_INFO& getLogger(void);
	INLINE ENGINE_COMPONENT_INFO& getInterfaces(void);

	INLINE ENGINE_COMPONENT_INFO& getComponent(COMPONENT_TYPE componentType);

	INLINE ENGINE_COMPONENT_INFO& getConfig();

 	void updateInfos(bool isPrint, COMPONENT_TYPE componentType, COMPONENT_ID componentID,
 				const Network::Address& internalTcpAddr, const Network::Address& externalTcpAddr, const Network::Address& externalUdpAddr);

	void updateExternalAddress(char* buf);

	INLINE int16 gameUpdateHertz(void) const;
	INLINE Network::Address interfacesAddr(void) const;

	const ChannelCommon& channelCommon(){ return channelCommon_; }

	uint32 tcp_SOMAXCONN(COMPONENT_TYPE componentType);

	float shutdowntime(){ return shutdown_time_; }
	float shutdownWaitTickTime(){ return shutdown_waitTickTime_; }

	uint32 tickMaxBufferedLogs() const { return tick_max_buffered_logs_; }
	uint32 tickMaxSyncLogs() const { return tick_max_sync_logs_; }

	INLINE bool IsPureDBInterfaceName(const std::string& dbInterfaceName);
	INLINE DBInterfaceInfo* dbInterface(const std::string& name);
	INLINE int dbInterfaceName2dbInterfaceIndex(const std::string& dbInterfaceName);
	INLINE const char* dbInterfaceIndex2dbInterfaceName(size_t dbInterfaceIndex);

private:
	void _updateEmailInfos();

private:
	ENGINE_COMPONENT_INFO _cellAppInfo;
	ENGINE_COMPONENT_INFO _baseAppInfo;
	ENGINE_COMPONENT_INFO _dbmgrInfo;
	ENGINE_COMPONENT_INFO _loginAppInfo;
	ENGINE_COMPONENT_INFO _cellAppMgrInfo;
	ENGINE_COMPONENT_INFO _baseAppMgrInfo;
	ENGINE_COMPONENT_INFO _kbMachineInfo;
	ENGINE_COMPONENT_INFO _botsInfo;
	ENGINE_COMPONENT_INFO _loggerInfo;
	ENGINE_COMPONENT_INFO _interfacesInfo;

public:
	int16 gameUpdateHertz_;
	uint32 tick_max_buffered_logs_;
	uint32 tick_max_sync_logs_;

	ChannelCommon channelCommon_;

	// Maximum bandwidth per second per client
	uint32 bitsPerSecondToClient_;

	Network::Address interfacesAddr_;
	uint32 interfaces_orders_timeout_;

	float shutdown_time_;
	float shutdown_waitTickTime_;

	float callback_timeout_;										// Callback default timeout(second)
	float thread_timeout_;											// Default timeout (seconds)

	uint32 thread_init_create_, thread_pre_create_, thread_max_create_;

	EmailServerInfo	emailServerInfo_;
	EmailSendInfo emailAtivationInfo_;
	EmailSendInfo emailResetPasswordInfo_;
	EmailSendInfo emailBindInfo_;

};

#define g_ouroSrvConfig ServerConfig::getSingleton()
}


#ifdef CODE_INLINE
#include "serverconfig.inl"
#endif
#endif // OURO_SERVER_CONFIG_H
