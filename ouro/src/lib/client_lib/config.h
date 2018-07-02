// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com


#ifndef OURO_CLIENT_CONFIG_H
#define OURO_CLIENT_CONFIG_H

#include "common/common.h"
#if OURO_PLATFORM == PLATFORM_WIN32
#pragma warning (disable : 4996)
#endif

#include "common/singleton.h"
#include "thread/threadmutex.h"
#include "thread/threadguard.h"
#include "xml/xml.h"

namespace Ouroboros{

class Config : public Singleton<Config>
{
public:
	Config();
	~Config();

	bool loadConfig(std::string fileName);

	inline int16 gameUpdateHertz(void) const { return gameUpdateHertz_;}

	uint32 tcp_SOMAXCONN();

	const char* entryScriptFile() const{ return &entryScriptFile_[0]; }

	const char* accountName() const{ return &accountName_[0]; }

	const char* ip() const{ return &ip_[0]; }
	uint32 port() const{ return port_; }

	void writeAccountName(const char* name);

	bool useLastAccountName() const{ return useLastAccountName_; }

	int8 encryptLogin() const { return encrypt_login_; }

	bool isOnInitCallPropertysSetMethods() const { return isOnInitCallPropertysSetMethods_; }

public:
	int16 gameUpdateHertz_;

	uint32 tcp_SOMAXCONN_;									// Listen listener queue maximum

	uint32 port_;											// The port that the component is listening on after running
	char ip_[MAX_BUF];										// Runtime ip address of the component

	char entryScriptFile_[MAX_NAME];						// Components entry script file

	float channelInternalTimeout_;
	float channelExternalTimeout_;

	char accountName_[MAX_NAME];

	int8 encrypt_login_;

	std::string fileName_;

	bool useLastAccountName_;

	uint32 telnet_port;
	std::string telnet_passwd;
	std::string telnet_deflayer;

	bool isOnInitCallPropertysSetMethods_;
};

#define g_ouroConfig Ouroboros::Config::getSingleton()
}
#endif
