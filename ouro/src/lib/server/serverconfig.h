// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

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
	bool isPure; // Whether it is a clean library (an entity table created without an engine)
	char name[MAX_BUF]; // The interface name of the database
	char db_type[MAX_BUF]; // category of the database
	uint32 db_port; // port of the database
	char db_ip[MAX_BUF]; // ip address of the database
	char db_username[MAX_NAME]; // username of the database
	char db_password[MAX_BUF * 10]; // password for the database
	bool db_passwordEncrypt; // Is the db password encrypted?
	char db_name[MAX_NAME]; // database name
	uint16 db_numConnections; // database maximum connection
	std::string db_unicodeString_characterSet; // Set the database character set
	std::string db_unicodeString_collation;
};

// engine component information structure
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

	uint32 port; // port of the component that is listening after running
	char ip[MAX_BUF]; // The runtime ip address of the component

	std::vector< std::string > machine_addresses; // all machine addresses given in the configuration
	
	char entryScriptFile[MAX_NAME]; // component's entry script file
	char dbAccountEntityScriptType[MAX_NAME]; // database account script category
	float defaultViewRadius; // Configure the view radius of the player in the cellapp node
	float defaultViewHysteresisArea; // Configure the hysteresis of the view of the player in the cellapp node
	uint16 witness_timeout; // observer default timeout (seconds)
	const Network::Address* externalTcpAddr; // external address
	const Network::Address* externalUdpAddr; // external address
	const Network::Address* internalTcpAddr; // internal address
	COMPONENT_ID componentID;

	float ghostDistance; // ghost area distance
	uint16 ghostingMaxPerCheck; // check the number of ghosts per second
	uint16 ghostUpdateHertz; // ghost update hz
	
	bool use_coordinate_system; // Whether to use the coordinate system If it is false, view, trap, move and other functions will no longer be maintained
	bool coordinateSystem_hasY; // The scope manager manages the Y axis. Note: If there is a y axis, the functions such as view and trap have height, but the management of the y axis will bring some consumption.
	uint16 entity_posdir_additional_updates; // After the entity position stops changing, the engine continues to update the location information of the tick times to the client. If it is 0, it is always updated.
	uint16 entity_posdir_updates_type; // Entity location update mode, 0: non-optimized high-precision synchronization, 1: optimized synchronization, 2: intelligent selection mode
	uint16 entity_posdir_updates_smart_threshold; // Entity location update the number of people on the same screen in smart mode

	bool aliasEntityID; // Optimize EntityID, less than 255 EntityIDs in view range, 1 byte pseudo ID when transferring to client
	bool entitydefAliasID; // Optimize the bandwidth occupied by the entity attribute and method broadcast, when the entity client attribute or the client does not exceed 255, the method uid and the attribute uid use the 1-byte alias ID when transmitting to the client.

	char internalInterface[MAX_NAME]; // internal NIC interface name
	char externalInterface[MAX_NAME]; // external NIC interface name
	char externalAddress[MAX_NAME]; // external IP address

	int32 externalTcpPorts_min; // Use the specified range for the external socket TCP port
	int32 externalTcpPorts_max;

	int32 externalUdpPorts_min; // Use the specified range for the external socket UDP port
	int32 externalUdpPorts_max;

	std::vector<DBInterfaceInfo> dbInterfaceInfos; // database interface
	bool notFoundAccountAutoCreate; // The game database can be automatically created if the game database cannot be found when the login is legal.
	bool allowEmptyDigest; // check defs-MD5
	bool account_registration_enable; // Whether to open registration
	bool account_reset_password_enable; // Whether to open the reset password function
	bool isShareDB; // whether to share the database

	float archivePeriod; // entity storage database cycle
	float backupPeriod; // entity backup cycle
	bool backUpUndefinedProperties; // Whether the entity backs up undefined attributes
	uint16 entityRestoreSize; // entity restore per tick number

	float loadSmoothingBias; // baseapp load filter balance adjustment value,
	uint32 login_port; // server login port Currently bots are in use
	uint32 login_port_min; // server login port uses the specified range. Currently bots are in use.
	uint32 login_port_max;
	char login_ip[MAX_BUF]; // server login ip address

	ENTITY_ID ids_criticallyLowSize; // Apply a new id resource to dbmgr when there are so many ids left
	ENTITY_ID ids_increasing_range; // id increment range each time the ID is applied

	uint32 downloadBitsPerSecondTotal; // total client download bandwidth per second
	uint32 downloadBitsPerSecondPerClient; // download bandwidth per client per second

	Profiles_Config profiles;

	uint32 defaultAddBots_totalCount; // Automatically add so many bots after the process is started to add the total number
	float defaultAddBots_tickTime; // Automatically add so many bots after the default process, each time it takes to add (s)
	uint32 defaultAddBots_tickCount; // Automatically add so many bots after starting the process by default

	bool forceInternalLogin; // Corresponding to the baseapp's externalAddress solution, when the externalAddress is forced to send the public IP address to provide login,
															// If the internal network using the robot test also takes the public network IP and traffic may not be suitable, you can set it to true at this time, and force the direct use of the intranet environment when logging in.

	std::string bots_account_name_prefix; // prefix of the robot account name
	uint32 bots_account_name_suffix_inc; // The suffix of the robot account name is incremented, 0 is incremented by random number, otherwise it is incremented by the number filled in by baseNum
	std::string bots_account_passwd; // password for the robot account

	uint32 tcp_SOMAXCONN; // listen listen queue maximum

	int8 encrypt_login; // encrypted login information

	uint32 telnet_port;
	std::string telnet_passwd;
	std::string telnet_deflayer;

	uint32 perSecsDestroyEntitySize; // destroy the number of entities per second

	uint64 respool_timeout;
	uint32 respool_buffersize;

	uint8 account_type; // 1: normal account, 2: email account (requires activation), 3: smart account (automatic identification of email, normal number, etc.)
	uint32 accountDefaultFlags; // new account default tag (ACCOUNT_FLAGS can be superimposed, filled in decimal format)
	uint64 accountDefaultDeadline; // The default expiration time of the new account (seconds, the engine will add the current time)
	
	std::string http_cbhost;
	uint16 http_cbport; // User http callback interface, handling authentication, password reset, etc.

	bool debugDBMgr; // debug mode can output read and write operation information

	bool isOnInitCallPropertysSetMethods; // bots dedicated: whether to trigger the set_* event of the property when Entity is initialized
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

	Network::Address interfacesAddr(void) const;
	INLINE std::vector< Network::Address > interfacesAddrs(void) const;

	const ChannelCommon& channelCommon(){ return channelCommon_; }

	uint32 tcp_SOMAXCONN(COMPONENT_TYPE componentType);

	float shutdowntime(){ return shutdown_time_; }
	float shutdownWaitTickTime(){ return shutdown_waitTickTime_; }

	uint32 tickMaxBufferedLogs() const { return tick_max_buffered_logs_; }
	uint32 tickMaxSyncLogs() const { return tick_max_sync_logs_; }

	INLINE float channelExternalTimeout(void) const;
	INLINE bool isPureDBInterfaceName(const std::string& dbInterfaceName);
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

	// The maximum bandwidth consumed per client per second
	uint32 bitsPerSecondToClient_;		

	Network::Address interfacesAddr_;
	std::vector< Network::Address > interfacesAddrs_;
	uint32 interfaces_orders_timeout_;

	float shutdown_time_;
	float shutdown_waitTickTime_;

	float callback_timeout_; // callback default timeout (seconds)
	float thread_timeout_; // default timeout (seconds)

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
