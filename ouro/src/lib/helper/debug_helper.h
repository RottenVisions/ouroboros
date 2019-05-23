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


#define	KBELOG_UNKNOWN			0x00000000
#define	KBELOG_PRINT			0x00000001
#define	KBELOG_ERROR			0x00000002
#define	KBELOG_WARNING			0x00000004
#define	KBELOG_DEBUG			0x00000008
#define	KBELOG_INFO				0x00000010
#define	KBELOG_CRITICAL			0x00000020
#define KBELOG_SCRIPT_INFO		0x00000040
#define KBELOG_SCRIPT_ERROR		0x00000080
#define KBELOG_SCRIPT_DEBUG		0x00000100
#define KBELOG_SCRIPT_WARNING	0x00000200
#define KBELOG_SCRIPT_NORMAL	0x00000400

#define KBELOG_TYPES KBELOG_UNKNOWN | KBELOG_PRINT | KBELOG_ERROR | KBELOG_WARNING | \
	KBELOG_DEBUG | KBELOG_INFO | KBELOG_CRITICAL | KBELOG_SCRIPT_INFO | KBELOG_SCRIPT_ERROR | KBELOG_SCRIPT_DEBUG | \
	KBELOG_SCRIPT_WARNING | KBELOG_SCRIPT_NORMAL

const char KBELOG_TYPE_NAME[][255] = {
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

inline const char* KBELOG_TYPE_NAME_EX(uint32 CTYPE)
{									
	if(CTYPE < 0 || ((CTYPE) & (KBELOG_TYPES)) <= 0)
	{
		return " UNKNOWN";
	}
	
	switch(CTYPE)
	{
	case KBELOG_PRINT:
		return "        ";
	case KBELOG_ERROR:
		return "   ERROR";
	case KBELOG_WARNING:
		return " WARNING";
	case KBELOG_DEBUG:
		return "   DEBUG";
	case KBELOG_INFO:
		return "    INFO";
	case KBELOG_CRITICAL:
		return "CRITICAL";
	case KBELOG_SCRIPT_INFO:
		return "  S_INFO";
	case KBELOG_SCRIPT_ERROR:
		return "   S_ERR";
	case KBELOG_SCRIPT_DEBUG:
		return "   S_DBG";
	case KBELOG_SCRIPT_WARNING:
		return "  S_WARN";
	case KBELOG_SCRIPT_NORMAL:
		return "  S_NORM";
	};

	return " UNKNOWN";
}

int KBELOG_TYPE_MAPPING(int type);

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
#define SCRIPT_INFO_MSG(m)				DebugHelper::getSingleton().script_info_msg((m))							// ���info��Ϣ
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
