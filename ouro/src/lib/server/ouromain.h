// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_OUROMAIN_H
#define OURO_OUROMAIN_H

#include "helper/memory_helper.h"

#include "serverapp.h"
#include "common/common.h"
#include "common/ourokey.h"
#include "common/stringconv.h"
#include "helper/debug_helper.h"
#include "network/event_dispatcher.h"
#include "network/message_handler.h"
#include "network/network_interface.h"
#include "server/components.h"
#include "server/machine_infos.h"
#include "resmgr/resmgr.h"

#if OURO_PLATFORM == PLATFORM_WIN32
#include "helper/crashhandler.h"
#endif

namespace Ouroboros{

inline void START_MSG(const char * name, uint64 appuid)
{
	MachineInfos machineInfo;

	std::string s = (fmt::format("---- {} "
			"Version: {}. "
			"ScriptVersion: {}. "
			"Protocol: {}. "
			"Config: {} {}. "
			"Built: {} {}. "
			"AppID: {}. "
			"UID: {}. "
			"PID: {} ----\n",
		name, OUROVersion::versionString(), OUROVersion::scriptVersionString(),
		Network::MessageHandlers::getDigestStr(),
		OURO_CONFIG, OURO_ARCH, __TIME__, __DATE__,
		appuid, getUserUID(), getProcessPID()));

	INFO_MSG(s);

#if OURO_PLATFORM == PLATFORM_WIN32
	printf("%s", s.c_str());
#endif

	s = (fmt::format("Server {}: {} with {} RAM\n",
		machineInfo.machineName(),
		machineInfo.cpuInfo(),
		machineInfo.memInfo()));

	INFO_MSG(s);

#if OURO_PLATFORM == PLATFORM_WIN32
	printf("%s\n", s.c_str());
#endif

}

inline void loadConfig()
{
	Resmgr::getSingleton().initialize();

	// "../../res/server/ouroboros_defaults.xml"
	g_ouroSrvConfig.loadConfig("server/ouroboros_defaults.xml");

	// "../../../assets/res/server/ouroboros.xml"
	g_ouroSrvConfig.loadConfig("server/ouroboros.xml");
}

inline void setEvns()
{
	std::string scomponentGroupOrder = "0";
	std::string scomponentGlobalOrder = "0";
	std::string scomponentID = "0";

	if(g_componentGroupOrder > 0)
	{
		int32 icomponentGroupOrder = g_componentGroupOrder;
		scomponentGroupOrder = Ouroboros::StringConv::val2str(icomponentGroupOrder);
	}

	if(g_componentGlobalOrder > 0)
	{
		int32 icomponentGlobalOrder = g_componentGlobalOrder;
		scomponentGlobalOrder = Ouroboros::StringConv::val2str(icomponentGlobalOrder);
	}

	{
		uint64 v = g_componentID;
		scomponentID = Ouroboros::StringConv::val2str(v);
	}

	setenv("OURO_COMPONENTID", scomponentID.c_str(), 1);
	setenv("OURO_BOOTIDX_GLOBAL", scomponentGlobalOrder.c_str(), 1);
	setenv("OURO_BOOTIDX_GROUP", scomponentGroupOrder.c_str(), 1);
}

template <class SERVER_APP>
int ouroMainT(int argc, char * argv[], COMPONENT_TYPE componentType,
	int32 extlisteningTcpPort_min = -1, int32 extlisteningTcpPort_max = -1,
	int32 extlisteningUdpPort_min = -1, int32 extlisteningUdpPort_max = -1, const char * extlisteningInterface = "",
	int32 intlisteningPort = 0, const char * intlisteningInterface = "")
{
	setEvns();
	startLeakDetection(componentType, g_componentID);

	g_componentType = componentType;
	DebugHelper::initialize(componentType);

	INFO_MSG( "-----------------------------------------------------------------------------------------\n\n\n");

	OUROKey ourokey(Resmgr::getSingleton().matchPath("key/") + "ouroboros_public.key",
		Resmgr::getSingleton().matchPath("key/") + "ouroboros_private.key");

	Resmgr::getSingleton().print();

	Network::EventDispatcher dispatcher;
	DebugHelper::getSingleton().pDispatcher(&dispatcher);

	const ChannelCommon& channelCommon = g_ouroSrvConfig.channelCommon();

	Network::g_SOMAXCONN = g_ouroSrvConfig.tcp_SOMAXCONN(g_componentType);

	Network::NetworkInterface networkInterface(&dispatcher,
		extlisteningTcpPort_min, extlisteningTcpPort_max, extlisteningUdpPort_min, extlisteningUdpPort_max, extlisteningInterface,
		channelCommon.extReadBufferSize, channelCommon.extWriteBufferSize,
		(intlisteningPort != -1) ? htons(intlisteningPort) : -1, intlisteningInterface,
		channelCommon.intReadBufferSize, channelCommon.intWriteBufferSize);

	DebugHelper::getSingleton().pNetworkInterface(&networkInterface);

	g_ouroSrvConfig.updateInfos(true, componentType, g_componentID,
			networkInterface.intTcpAddr(), networkInterface.extTcpAddr(), networkInterface.extUdpAddr());

	if(getUserUID() <= 0)
	{
		WARNING_MSG(fmt::format("invalid UID({}) <= 0, please check UID for environment!\n", getUserUID()));
	}

	Components::getSingleton().initialize(&networkInterface, componentType, g_componentID);

	SERVER_APP app(dispatcher, networkInterface, componentType, g_componentID);
	Components::getSingleton().findLogger();
	START_MSG(COMPONENT_NAME_EX(componentType), g_componentID);

	if(!app.initialize())
	{
		ERROR_MSG("app::initialize(): initialization failed!\n");

		Components::getSingleton().finalise();
		app.finalise();

		// If there is still the log is not synchronized, it will continue to complete before the end
		DebugHelper::getSingleton().finalise();

#if OURO_PLATFORM == PLATFORM_WIN32
		// Wait a few seconds for users to see information on the window
		Beep(587, 500);
		Ouroboros::sleep(5000);
#endif
		return -1;
	}

	INFO_MSG(fmt::format("---- {} is running ----\n", COMPONENT_NAME_EX(componentType)));

#if OURO_PLATFORM == PLATFORM_WIN32
	printf("[INFO]: %s", (fmt::format("---- {} is running ----\n", COMPONENT_NAME_EX(componentType))).c_str());
#endif

	int ret = app.run();

	Components::getSingleton().finalise();
	app.finalise();
	INFO_MSG(fmt::format("{}({}) has shut down.\n", COMPONENT_NAME_EX(componentType), g_componentID));

	// If there is still the log is not synchronized, it will continue to complete before the end
	DebugHelper::getSingleton().finalise();
	return ret;
}

inline void parseMainCommandArgs(int argc, char* argv[])
{
	if(argc < 2)
	{
		return;
	}

	for(int argIdx=1; argIdx<argc; ++argIdx)
	{
		std::string cmd = argv[argIdx];

		std::string findcmd = "--cid=";
		std::string::size_type fi1 = cmd.find(findcmd);
		if(fi1 != std::string::npos)
		{
			cmd.erase(fi1, findcmd.size());
			if(cmd.size() > 0)
			{
				COMPONENT_ID cid = 0;
				try
				{
					StringConv::str2value(cid, cmd.c_str());
					g_componentID = cid;
				}
				catch(...)
				{
					ERROR_MSG("parseCommandArgs: --cid=? invalid, no set! type is uint64\n");
				}
			}

			continue;
		}

		findcmd = "--gus=";
		fi1 = cmd.find(findcmd);
		if(fi1 != std::string::npos)
		{
			cmd.erase(fi1, findcmd.size());
			if(cmd.size() > 0)
			{
				int32 gus = 0;
				try
				{
					StringConv::str2value(gus, cmd.c_str());

					OURO_ASSERT(gus <= 65535);
					g_genuuid_sections = gus;
				}
				catch(...)
				{
					ERROR_MSG("parseCommandArgs: --gus=? invalid, no set! type is uint16\n");
				}
			}

			continue;
		}

		findcmd = "--hide=";
		fi1 = cmd.find(findcmd);
		if (fi1 != std::string::npos)
		{
			cmd.erase(fi1, findcmd.size());
			if (cmd.size() > 0)
			{
				int32 hide = 0;
				try
				{
					StringConv::str2value(hide, cmd.c_str());
				}
				catch (...)
				{
					ERROR_MSG("parseCommandArgs: --hide=? invalid, no set! type is int8\n");
				}

				if (hide > 0)
				{
#if OURO_PLATFORM == PLATFORM_WIN32
					TCHAR strTitle[255];
					GetConsoleTitle(strTitle, 255);
					HWND hw = ::FindWindow(L"ConsoleWindowClass", strTitle);
					ShowWindow(hw, SW_HIDE);
#else
#endif
				}
			}

			continue;
		}

		findcmd = "--OURO_ROOT=";
		fi1 = cmd.find(findcmd);
		if (fi1 != std::string::npos)
		{
			cmd.erase(fi1, findcmd.size());
			if (cmd.size() > 0)
			{
				setenv("OURO_ROOT", cmd.c_str(), 1);
			}

			continue;
		}

		findcmd = "--OURO_RES_PATH=";
		fi1 = cmd.find(findcmd);
		if (fi1 != std::string::npos)
		{
			cmd.erase(fi1, findcmd.size());
			if (cmd.size() > 0)
			{
				setenv("OURO_RES_PATH", cmd.c_str(), 1);
			}

			continue;
		}

		findcmd = "--OURO_BIN_PATH=";
		fi1 = cmd.find(findcmd);
		if (fi1 != std::string::npos)
		{
			cmd.erase(fi1, findcmd.size());
			if (cmd.size() > 0)
			{
				setenv("OURO_BIN_PATH", cmd.c_str(), 1);
			}

			continue;
		}
	}
}

#if OURO_PLATFORM == PLATFORM_WIN32
#define OUROBOROS_MAIN																									\
ouroMain(int argc, char* argv[]);																						\
int main(int argc, char* argv[])																						\
{																														\
	loadConfig();																										\
	g_componentID = genUUID64();																						\
	parseMainCommandArgs(argc, argv);																					\
	char dumpname[MAX_BUF] = {0};																						\
	ouro_snprintf(dumpname, MAX_BUF, "%"PRAppID, g_componentID);															\
	Ouroboros::exception::installCrashHandler(1, dumpname);																\
	int retcode = -1;																									\
	THREAD_TRY_EXECUTION;																								\
	retcode = ouroMain(argc, argv);																						\
	THREAD_HANDLE_CRASH;																								\
	return retcode;																										\
}																														\
int ouroMain
#else
#define OUROBOROS_MAIN																									\
ouroMain(int argc, char* argv[]);																						\
int main(int argc, char* argv[])																						\
{																														\
	loadConfig();																										\
	g_componentID = genUUID64();																						\
	parseMainCommandArgs(argc, argv);																					\
	return ouroMain(argc, argv);																							\
}																														\
int ouroMain
#endif
}

#endif // OURO_OUROMAIN_H