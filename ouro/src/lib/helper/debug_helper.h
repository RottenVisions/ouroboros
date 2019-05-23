// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com


#ifndef OURO_DEBUG_H
#define OURO_DEBUG_H

#include "common/platform.h"
#if OURO_PLATFORM == PLATFORM_WIN32
#pragma warning(disable:4819)
#endif
#include "common/singleton.h"
#include "thread/threadmutex.h"
#include "network/common.h"
#include "network/address.h"

namespace Ouroboros{

namespace Network{
	class Channel;
	class Bundle;
	class EventDispatcher;
	class NetworkInterface;
	class Packet;
}

/** 
	Support uft-8 encoded string output
*/
void vutf8printf(FILE *out, const char *str, va_list* ap);
void utf8printf(FILE *out, const char *str, ...);


#define	OUROLOG_UNKNOWN			0x00000000
#define	OUROLOG_PRINT			0x00000001
#define	OUROLOG_ERROR			0x00000002
#define	OUROLOG_WARNING			0x00000004
#define	OUROLOG_DEBUG			0x00000008
#define	OUROLOG_INFO				0x00000010
#define	OUROLOG_CRITICAL			0x00000020
#define OUROLOG_SCRIPT_INFO		0x00000040
#define OUROLOG_SCRIPT_ERROR		0x00000080
#define OUROLOG_SCRIPT_DEBUG		0x00000100
#define OUROLOG_SCRIPT_WARNING	0x00000200
#define OUROLOG_SCRIPT_NORMAL	0x00000400

#define OUROLOG_TYPES OUROLOG_UNKNOWN | OUROLOG_PRINT | OUROLOG_ERROR | OUROLOG_WARNING | \
	OUROLOG_DEBUG | OUROLOG_INFO | OUROLOG_CRITICAL | OUROLOG_SCRIPT_INFO | OUROLOG_SCRIPT_ERROR | OUROLOG_SCRIPT_DEBUG | \
	OUROLOG_SCRIPT_WARNING | OUROLOG_SCRIPT_NORMAL

const char OUROLOG_TYPE_NAME[][255] = {
	" UNKNOWN",
	"        ",
	"   ERROR",
	" WARNING",
	"   DEBUG",
	"    INFO",
	"CRITICAL",
	"  S_INFO",
	"  S_ERR",
	"  S_DBG",
	"  S_WARN",
	"  S_NORM",
};

inline const char* OUROLOG_TYPE_NAME_EX(uint32 CTYPE)
{									
	if(CTYPE < 0 || ((CTYPE) & (OUROLOG_TYPES)) <= 0)
	{
		return " UNKNOWN";
	}
	
	switch(CTYPE)
	{
	case OUROLOG_PRINT:
		return "        ";
	case OUROLOG_ERROR:
		return "   ERROR";
	case OUROLOG_WARNING:
		return " WARNING";
	case OUROLOG_DEBUG:
		return "   DEBUG";
	case OUROLOG_INFO:
		return "    INFO";
	case OUROLOG_CRITICAL:
		return "CRITICAL";
	case OUROLOG_SCRIPT_INFO:
		return "  S_INFO";
	case OUROLOG_SCRIPT_ERROR:
		return "   S_ERR";
	case OUROLOG_SCRIPT_DEBUG:
		return "   S_DBG";
	case OUROLOG_SCRIPT_WARNING:
		return "  S_WARN";
	case OUROLOG_SCRIPT_NORMAL:
		return "  S_NORM";
	};

	return " UNKNOWN";
}

int OUROLOG_TYPE_MAPPING(int type);

class DebugHelper  : public Singleton<DebugHelper>
{
public:
	DebugHelper();

	~DebugHelper();
	
	static bool isInit() { return getSingletonPtr() != 0; }

	static void initialize(COMPONENT_TYPE componentType);
	static void finalise(bool destroy = false);

	void setFile(std::string funcname, std::string file, uint32 line){
		_currFile = file;
		_currLine = line;
		_currFuncName = funcname;
	}
	
	std::string getLogName();

	void lockthread();
	void unlockthread();
    
	void pNetworkInterface(Network::NetworkInterface* networkInterface);
	void pDispatcher(Network::EventDispatcher* dispatcher);
	
	Network::EventDispatcher* pDispatcher() const{ return pDispatcher_; }
	Network::NetworkInterface* pNetworkInterface() const{ return pNetworkInterface_; }

	void print_msg(const std::string& s);
	void debug_msg(const std::string& s);
	void error_msg(const std::string& s);
	void info_msg(const std::string& s);
	void warning_msg(const std::string& s);
	void critical_msg(const std::string& s);
	void script_info_msg(const std::string& s);
	void script_error_msg(const std::string& s);
	void backtrace_msg();

	void onMessage(uint32 logType, const char * str, uint32 length);

	void registerLogger(Network::MessageID msgID, Network::Address* pAddr);
	void unregisterLogger(Network::MessageID msgID, Network::Address* pAddr);

	void onNoLogger();

	void changeLogger(const std::string& name);
	void closeLogger();  // close logger for fork + execv

	void clearBufferedLog(bool destroy = false);

	void set_errorcolor();
	void set_normalcolor();
	void set_warningcolor();

	void setScriptMsgType(int msgtype);
	void resetScriptMsgType();

	void shouldWriteToSyslog(bool v = true);

	/** 
		Sync log to logger
	*/
	void sync();

	void printBufferedLogs();

	size_t hasBufferedLogPackets() const{ return hasBufferedLogPackets_; }

	Network::Channel* pLoggerChannel();

private:
	FILE* _logfile;
	std::string _currFile, _currFuncName;
	uint32 _currLine;

	Network::Address loggerAddr_;
	Ouroboros::thread::ThreadMutex logMutex;

	std::queue< Network::Bundle* > bufferedLogPackets_;
	size_t hasBufferedLogPackets_;

	Network::NetworkInterface* pNetworkInterface_;
	Network::EventDispatcher* pDispatcher_;

	int scriptMsgType_;

	bool noSyncLog_;

	bool canLogFile_;

	uint64 loseLoggerTime_;

	// Record the main thread ID, used to determine whether it is a child thread output log
	// When the child thread outputs the log, it will be synchronized to the logger when the relevant log is cached to the main thread.
#if OURO_PLATFORM == PLATFORM_WIN32
	DWORD mainThreadID_;
#else
	THREAD_ID mainThreadID_;
#endif

	ObjectPool<MemoryStream> memoryStreamPool_;
	std::queue< MemoryStream* > childThreadBufferedLogPackets_;
};

/*---------------------------------------------------------------------------------
	Debug information output interface
---------------------------------------------------------------------------------*/
#define SCRIPT_INFO_MSG(m)				DebugHelper::getSingleton().script_info_msg((m))							//  ‰≥ˆinfo–≈œ¢
#define SCRIPT_ERROR_MSG(m) DebugHelper::getSingleton().script_error_msg((m)) // Output error message

#define PRINT_MSG(m) DebugHelper::getSingleton().print_msg((m)) // Output any information
#define ERROR_MSG(m) DebugHelper::getSingleton().error_msg((m)) // Output an error
#define DEBUG_MSG(m) DebugHelper::getSingleton().debug_msg((m)) // Output a debug message
#define INFO_MSG(m) DebugHelper::getSingleton().info_msg((m)) // Output an info message
#define WARNING_MSG(m) DebugHelper::getSingleton().warning_msg((m)) // Output a warning message
#define CRITICAL_MSG(m)					DebugHelper::getSingleton().setFile(__FUNCTION__, \
										__FILE__, __LINE__); \
										DebugHelper::getSingleton().critical_msg((m))

/*---------------------------------------------------------------------------------
	Debug macro
---------------------------------------------------------------------------------*/
#ifdef OURO_USE_ASSERTS
void myassert(const char* exp, const char * func, const char * file, unsigned int line);
#define OURO_ASSERT(exp) if(!(exp))myassert(#exp, __FUNCTION__, __FILE__, __LINE__);
#define OURO_REAL_ASSERT assert(0);
#else
#define OURO_ASSERT(exp) assert((exp));
#define OURO_REAL_ASSERT assert(0);
#endif

#ifdef _DEBUG
#define OURO_VERIFY OURO_ASSERT
#else
#define OURO_VERIFY(exp) (exp)
#endif

#define OURO_EXIT(msg) {														\
			CRITICAL_MSG(msg);												\
			OURO_REAL_ASSERT	}												\


}

#endif // OURO_DEBUG_H
