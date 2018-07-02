// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_PLATFORM_H
#define OURO_PLATFORM_H

// common include
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <cstring>
#include <vector>
#include <map>
#include <list>
#include <set>
#include <deque>
#include <limits>
#include <algorithm>
#include <utility>
#include <functional>
#include <cctype>
#include <iterator>
#include "common/strutil.h"
// windows include
#if defined( __WIN32__ ) || defined( WIN32 ) || defined( _WIN32 )
#pragma warning(disable:4996)
#pragma warning(disable:4819)
#pragma warning(disable:4049)
#pragma warning(disable:4217)
#include <io.h>
#include <time.h>
//#define FD_SETSIZE 1024
#ifndef WIN32_LEAN_AND_MEAN
#include <winsock2.h>		// Must be included before windows.h, otherwise network module compiles error
#include <mswsock.h>
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <unordered_map>
#include <functional>
#include <memory>
#define _SCL_SECURE_NO_WARNINGS
#else
// linux include
#include <errno.h>
#include <float.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <iconv.h>
#include <langinfo.h>   /* CODESET */
#include <stdint.h>
#include <signal.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <tr1/unordered_map>
#include <tr1/functional>
#include <tr1/memory>
#include <linux/types.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/resource.h>
#include <linux/errqueue.h>
#endif

#include <signal.h>

#if !defined( _WIN32 )
# include <pwd.h>
#else
#define SIGHUP	1
#define SIGINT	2
#define SIGQUIT 3
#define SIGUSR1 10
#define SIGPIPE 13
#define SIGCHLD 17
#define SIGSYS	32
#endif

/** Defining the engine name space */
namespace Ouroboros
{

/** Defining engine byte order */
#define OUROBOROS_LITTLE_ENDIAN							0
#define OUROBOROS_BIG_ENDIAN								1
#if !defined(OUROBOROS_ENDIAN)
#  if defined (USE_BIG_ENDIAN)
#    define OUROBOROS_ENDIAN OUROBOROS_BIG_ENDIAN
#  else
#    define OUROBOROS_ENDIAN OUROBOROS_LITTLE_ENDIAN
#  endif
#endif


// current platform and compiler
#define PLATFORM_WIN32			0
#define PLATFORM_UNIX			1
#define PLATFORM_APPLE			2

#define UNIX_FLAVOUR_LINUX		1
#define UNIX_FLAVOUR_BSD		2
#define UNIX_FLAVOUR_OTHER		3
#define UNIX_FLAVOUR_OSX		4

#if defined( __WIN32__ ) || defined( WIN32 ) || defined( _WIN32 )
#  define OURO_PLATFORM PLATFORM_WIN32
#elif defined( __INTEL_COMPILER )
#  define OURO_PLATFORM PLATFORM_INTEL
#elif defined( __APPLE_CC__ )
#  define OURO_PLATFORM PLATFORM_APPLE
#else
#  define OURO_PLATFORM PLATFORM_UNIX
#endif

#define COMPILER_MICROSOFT 0
#define COMPILER_GNU	   1
#define COMPILER_BORLAND   2
#define COMPILER_INTEL     3
#define COMPILER_CLANG     4

#ifdef _MSC_VER
#  define OURO_COMPILER COMPILER_MICROSOFT
#elif defined( __INTEL_COMPILER )
#  define OURO_COMPILER COMPILER_INTEL
#elif defined( __BORLANDC__ )
#  define OURO_COMPILER COMPILER_BORLAND
#elif defined( __GNUC__ )
#  define OURO_COMPILER COMPILER_GNU
#elif defined( __clang__ )
#  define OURO_COMPILER COMPILER_CLANG

#else
#  pragma error "FATAL ERROR: Unknown compiler."
#endif

#if OURO_PLATFORM == PLATFORM_UNIX || OURO_PLATFORM == PLATFORM_APPLE
#ifdef HAVE_DARWIN
#define OURO_PLATFORM_TEXT "MacOSX"
#define UNIX_FLAVOUR UNIX_FLAVOUR_OSX
#else
#ifdef USE_KQUEUE
#define OURO_PLATFORM_TEXT "FreeBSD"
#define UNIX_FLAVOUR UNIX_FLAVOUR_BSD
#else
#ifdef USE_KQUEUE_DFLY
#define OURO_PLATFORM_TEXT "DragonFlyBSD"
#define UNIX_FLAVOUR UNIX_FLAVOUR_BSD
#else
#define OURO_PLATFORM_TEXT "Linux"
#define UNIX_FLAVOUR UNIX_FLAVOUR_LINUX
#endif
#endif
#endif
#endif

#if OURO_PLATFORM == PLATFORM_WIN32
#define OURO_PLATFORM_TEXT "Win32"
#endif

#ifndef OURO_CONFIG
#ifdef _DEBUG
#define OURO_CONFIG "Debug"
#else
#define OURO_CONFIG "Release"
#endif
#endif

#ifndef X64
#if defined( _WIN64 ) || defined( __x86_64__ ) || defined( __amd64 ) || defined( __LP64__ )
#define X64
#endif
#endif

#ifdef X64
#define OURO_ARCH "X64"
#else
#define OURO_ARCH "X86"
#endif

/*---------------------------------------------------------------------------------
	Type definition
---------------------------------------------------------------------------------*/
#ifndef TCHAR
#ifdef _UNICODE
	typedef wchar_t												TCHAR;
#else
	typedef char												TCHAR;
#endif
#endif

typedef unsigned char											uchar;
typedef unsigned short											ushort;
typedef unsigned int											uint;
typedef unsigned long											ulong;

#define charptr													char*
#define const_charptr											const char*
#define PyObject_ptr											PyObject*

#define OUROShared_ptr											std::tr1::shared_ptr
#define OUROUnordered_map										std::tr1::unordered_map

/* Use correct types for x64 platforms, too */
#if OURO_COMPILER != COMPILER_GNU
typedef signed __int64											int64;
typedef signed __int32											int32;
typedef signed __int16											int16;
typedef signed __int8											int8;
typedef unsigned __int64										uint64;
typedef unsigned __int32										uint32;
typedef unsigned __int16										uint16;
typedef unsigned __int8											uint8;
typedef INT_PTR													intptr;
typedef UINT_PTR        										uintptr;
#define PRI64													"lld"
#define PRIu64													"llu"
#define PRIx64													"llx"
#define PRIX64													"llX"
#define PRIzu													"lu"
#define PRIzd													"ld"
#define PRTime													PRI64
#else
typedef int64_t													int64;
typedef int32_t													int32;
typedef int16_t													int16;
typedef int8_t													int8;
typedef uint64_t												uint64;
typedef uint32_t												uint32;
typedef uint16_t												uint16;
typedef uint8_t													uint8;
typedef uint16_t												WORD;
typedef uint32_t												DWORD;

#ifdef _LP64
typedef int64													intptr;
typedef uint64													uintptr;

#ifndef PRI64
#define PRI64													"ld"
#endif

#ifndef PRIu64
#define PRIu64													"lu"
#endif

#ifndef PRIx64
#define PRIx64													"lx"
#endif

#ifndef PRIX64
#define PRIX64													"lX"
#endif

#ifndef PRTime
#define PRTime													PRI64
#endif

#else
typedef int32													intptr;
typedef uint32													uintptr;

#ifndef PRI64
#define PRI64													"lld"
#endif

#ifndef PRIu64
#define PRIu64													"llu"
#endif

#ifndef PRIx64
#define PRIx64													"llx"
#endif

#ifndef PRIX64
#define PRIX64													"llX"
#endif

#ifndef PRTime
#define PRTime													"ld"
#endif

#endif

#ifndef PRIzd
#define PRIzd													"zd"
#endif

#ifndef PRIzu
#define PRIzu													"zu"
#endif

#endif

#define PRAppID													PRIu64
#define PRDBID													PRIu64

typedef uint16													ENTITY_TYPE;											// The entity's category type definition supports 0-65535 categories
typedef int32													ENTITY_ID;												// The type of entityID
typedef uint32													SPACE_ID;												// The id of a space
typedef uint32													CALLBACK_ID;											// a callback id assigned by CallbackMgr
typedef uint64													COMPONENT_ID;											// The id of a server component
typedef int32													COMPONENT_ORDER;										// The start order of a component
typedef int32													COMPONENT_GUS;											// Genuuid_sections of a component generate interval segments of random numbers
typedef	uint32													TIMER_ID;												// The id type of a timer
typedef uint8													ENTITYCALL_CALL_TYPE;									// The type of call category delivered by entityCall
typedef uint32													GAME_TIME;
typedef uint32													GameTime;
typedef int32													ScriptID;
typedef uint32													ArraySize;												// The size of any array uses this description
typedef uint64													DBID;													// An index in the database is used as an ID
typedef uint32													CELL_ID;
typedef OUROUnordered_map< std::string, std::string >			SPACE_DATA;												// Data stored in space

#if OURO_PLATFORM == PLATFORM_WIN32
	#define IFNAMSIZ											16
	typedef UINT_PTR											OUROSOCKET;
#ifndef socklen_t
	typedef	int													socklen_t;
#endif
	typedef unsigned short										u_int16_t;
	typedef unsigned long										u_int32_t;

#ifndef IFF_UP
	enum
	{
		IFF_UP													= 0x1,
		IFF_BROADCAST											= 0x2,
		IFF_DEBUG												= 0x4,
		IFF_LOOPBACK											= 0x8,
		IFF_POINTOPOINT											= 0x10,
		IFF_NOTRAILERS											= 0x20,
		IFF_RUNNING												= 0x40,
		IFF_NOARP												= 0x80,
		IFF_PROMISC												= 0x100,
		IFF_MULTICAST											= 0x1000
	};
#endif
#else
	typedef int													OUROSOCKET;
#endif

/*---------------------------------------------------------------------------------
	Will be multithreaded on multiple platforms
---------------------------------------------------------------------------------*/
#if OURO_PLATFORM == PLATFORM_WIN32
	#define THREAD_ID											HANDLE
	#define THREAD_SINGNAL										HANDLE
	#define THREAD_SINGNAL_INIT(x)								x = CreateEvent(NULL, TRUE, FALSE, NULL)
	#define THREAD_SINGNAL_DELETE(x)							CloseHandle(x)
	#define THREAD_SINGNAL_SET(x)								SetEvent(x)
	#define THREAD_MUTEX										CRITICAL_SECTION
	#define THREAD_MUTEX_INIT(x)								InitializeCriticalSection(&x)
	#define THREAD_MUTEX_DELETE(x)								DeleteCriticalSection(&x)
	#define THREAD_MUTEX_LOCK(x)								EnterCriticalSection(&x)
	#define THREAD_MUTEX_UNLOCK(x)								LeaveCriticalSection(&x)
#else
	#define THREAD_ID											pthread_t
	#define THREAD_SINGNAL										pthread_cond_t
	#define THREAD_SINGNAL_INIT(x)								pthread_cond_init(&x, NULL)
	#define THREAD_SINGNAL_DELETE(x)							pthread_cond_destroy(&x)
	#define THREAD_SINGNAL_SET(x)								pthread_cond_signal(&x);
	#define THREAD_MUTEX										pthread_mutex_t
	#define THREAD_MUTEX_INIT(x)								pthread_mutex_init (&x, NULL)
	#define THREAD_MUTEX_DELETE(x)								pthread_mutex_destroy(&x)
	#define THREAD_MUTEX_LOCK(x)								pthread_mutex_lock(&x)
	#define THREAD_MUTEX_UNLOCK(x)								pthread_mutex_unlock(&x)
#endif

/*---------------------------------------------------------------------------------
	Cross-platform macro definition
---------------------------------------------------------------------------------*/
#if 0
#define ARRAYCLR(v)					memset((v), 0x0, sizeof(v))
#define MEMCLR(v)					memset(&(v), 0x0, sizeof(v))
#define MEMCLRP(v)					memset((v), 0x0, sizeof(*v))
#endif

#define ARRAYSZ(v)					(sizeof(v) / sizeof(v[0]))
#define ARRAY_SIZE(v)				(sizeof(v) / sizeof(v[0]))

#if 0
#define offsetof(type, field)		((uint32)&(((type *)NULL)->field))
#ifndef FIELD_OFFSET
#define FIELD_OFFSET(type, field)	offsetof(type, field)
#endif
#ifndef FIELD_SIZE
#define FIELD_SIZE(type, field)		(sizeof(((type *)NULL)->field))
#endif
#endif

#define OURO_LITTLE_ENDIAN
/*#define OURO_BIG_ENDIAN*/

#ifdef OURO_LITTLE_ENDIAN
/* accessing individual bytes (int8) and words (int16) within
 * words and long words (int32).
 * Macros ending with W deal with words, L macros deal with longs
 */
/// Returns the high byte of a word.
#define HIBYTEW(b)		(((b) & 0xff00) >> 8)
/// Returns the low byte of a word.
#define LOBYTEW(b)		( (b) & 0xff)

/// Returns the high byte of a long.
#define HIBYTEL(b)		(((b) & 0xff000000L) >> 24)
/// Returns the low byte of a long.
#define LOBYTEL(b)		( (b) & 0xffL)

/// Returns byte 0 of a long.
#define BYTE0L(b)		( (b) & 0xffL)
/// Returns byte 1 of a long.
#define BYTE1L(b)		(((b) & 0xff00L) >> 8)
/// Returns byte 2 of a long.
#define BYTE2L(b)		(((b) & 0xff0000L) >> 16)
/// Returns byte 3 of a long.
#define BYTE3L(b)		(((b) & 0xff000000L) >> 24)

/// Returns the high word of a long.
#define HIWORDL(b)		(((b) & 0xffff0000L) >> 16)
/// Returns the low word of a long.
#define LOWORDL(b)		( (b) & 0xffffL)

/**
 *	This macro takes a dword ordered 0123 and reorder it to 3210.
 */
#define SWAP_DW(a)	  ( (((a) & 0xff000000)>>24) |	\
						(((a) & 0xff0000)>>8) |		\
						(((a) & 0xff00)<<8) |		\
						(((a) & 0xff)<<24) )

#else
/* big endian macros go here */
#endif

#if defined(_WIN32)

#undef min
#define min min
#undef max
#define max max

template <class T>
inline const T & min( const T & a, const T & b )
{
	return b < a ? b : a;
}

template <class T>
inline const T & max( const T & a, const T & b )
{
	return a < b ? b : a;
}

#define OURO_MIN min
#define OURO_MAX max

#define NOMINMAX

#else

#define OURO_MIN std::min
#define OURO_MAX std::max

#endif

// Maximum length of all name strings
#define MAX_NAME 256

// The maximum length of the ip string
#define MAX_IP 256

// Conventional buf length
#define MAX_BUF 256

// Conventional buf length
#define SQL_BUF 65535

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

// Get the last error description generated by the system
inline char* ouro_strerror(int ierrorno = 0)
{
#if OURO_PLATFORM == PLATFORM_WIN32
	if(ierrorno == 0)
		ierrorno = GetLastError();

	static char lpMsgBuf[256] = {0};

	/*
	FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		ierrorno,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		1024,
		NULL
	);
	*/
	ouro_snprintf(lpMsgBuf, 256, "errorno=%d",  ierrorno);
	return lpMsgBuf;
#else
	if(ierrorno != 0)
		return strerror(ierrorno);
	return strerror(errno);
#endif
}

inline int ouro_lasterror()
{
#if OURO_PLATFORM == PLATFORM_WIN32
	return GetLastError();
#else
	return errno;
#endif
}

/** Get user UID */
inline int32 getUserUID()
{
	static int32 iuid = 0;

	if(iuid == 0)
	{
#if OURO_PLATFORM == PLATFORM_WIN32
		// VS2005:
		#if _MSC_VER >= 1400
			char uid[16];
			size_t sz;
			iuid = getenv_s( &sz, uid, sizeof( uid ), "UID" ) == 0 ? atoi( uid ) : 0;

		// VS2003:
		#elif _MSC_VER < 1400
			char * uid = getenv( "UID" );
			iuid = uid ? atoi( uid ) : 0;
		#endif
#else
	// Linux:
		char * uid = getenv( "UID" );
		iuid = uid ? atoi( uid ) : getuid();
#endif
	}

	return iuid;
}

/** Get username */
inline const char * getUsername()
{
#if OURO_PLATFORM == PLATFORM_WIN32
	DWORD dwSize = MAX_NAME;
	wchar_t wusername[MAX_NAME];
	::GetUserNameW(wusername, &dwSize);

	static char username[MAX_NAME];
	memset(username, 0, MAX_NAME);

	if(dwSize > 0)
	{
		size_t outsize = 0;
		strutil::wchar2char((wchar_t*)&wusername, &outsize);

		if(outsize == 0)
		{
			// May be a Chinese name, not a Chinese name
			strcpy(username, "error_name");
		}
	}

	return username;
#else
	char * pUsername = cuserid( NULL );
	return pUsername ? pUsername : "";
#endif
}

/** Get process ID */
inline int32 getProcessPID()
{
#if OURO_PLATFORM != PLATFORM_WIN32
	return getpid();
#else
	return (int32) GetCurrentProcessId();
#endif
}

/** Get system time (accurate to milliseconds) */
#if OURO_PLATFORM == PLATFORM_WIN32
	inline uint32 getSystemTime()
	{
		// Note that this function can only be correctly maintained on windows for 49 days.
		return ::GetTickCount();
	};
#else
	inline uint32 getSystemTime()
	{
		struct timeval tv;
		struct timezone tz;
		gettimeofday(&tv, &tz);
		return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
	};
#endif

/** Get 2 system time difference */
inline uint32 getSystemTimeDiff(uint32 oldTime, uint32 newTime)
{
    // Prevent getSystemTime() from overflowing
    if (oldTime > newTime)
    {
        return (uint32)((int64)0xFFFFFFFF + 1 - (int64)oldTime) + newTime;
    }

	return newTime - oldTime;
}

/* get system time */
inline void ouro_timeofday(long *sec, long *usec)
{
#if defined(__unix)
	struct timeval time;
	gettimeofday(&time, NULL);
	if (sec) *sec = time.tv_sec;
	if (usec) *usec = time.tv_usec;
#else
	static long mode = 0, addsec = 0;
	BOOL retval;
	static int64 freq = 1;
	int64 qpc;
	if (mode == 0) {
		retval = QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
		freq = (freq == 0) ? 1 : freq;
		retval = QueryPerformanceCounter((LARGE_INTEGER*)&qpc);
		addsec = (long)time(NULL);
		addsec = addsec - (long)((qpc / freq) & 0x7fffffff);
		mode = 1;
	}
	retval = QueryPerformanceCounter((LARGE_INTEGER*)&qpc);
	retval = retval * 2;
	if (sec) *sec = (long)(qpc / freq) + addsec;
	if (usec) *usec = (long)((qpc % freq) * 1000000 / freq);
#endif
}

/* get clock in millisecond 64 */
inline int64 ouro_clock64(void)
{
	long s, u;
	int64 value;
	ouro_timeofday(&s, &u);
	value = ((int64)s) * 1000 + (u / 1000);
	return value;
}

inline uint32 ouro_clock()
{
	return (uint32)(ouro_clock64() & 0xfffffffful);
}

/* Generate a 64-bit uuid
*/
extern COMPONENT_ORDER g_componentGlobalOrder;
extern COMPONENT_ORDER g_componentGroupOrder;

extern COMPONENT_GUS g_genuuid_sections;

inline uint64 genUUID64()
{
	static uint64 tv = (uint64)(time(NULL));
	uint64 now = (uint64)(time(NULL));

	static uint16 lastNum = 0;

	if(now != tv)
	{
		tv = now;
		lastNum = 0;
	}

	if(g_genuuid_sections <= 0)
	{
		// Timestamp 32 bits, random number 16 bits, 16-bit iteration number (maximum 65535-1)
		static uint32 rnd = 0;
		if(rnd == 0)
		{
			srand(getSystemTime());
			rnd = (uint32)(rand() << 16);
		}

		assert(lastNum < 65535 && "genUUID64(): overflow!");

		return (tv << 32) | rnd | lastNum++;
	}
	else
	{
		// Timestamp 32 bits, app group ID 16 bits, 16-bit iteration number (maximum 65535-1)
		static uint32 sections = g_genuuid_sections << 16;

		assert(lastNum < 65535 && "genUUID64(): overflow!");

		return (tv << 32) | sections | lastNum++;
	}
}

/** sleep Cross-platform */
#if OURO_PLATFORM == PLATFORM_WIN32
	inline void sleep(uint32 ms)
	{
		::Sleep(ms);
	}
#else
	inline void sleep(uint32 ms)
	{
	  struct timeval tval;
	  tval.tv_sec	= ms / 1000;
	  tval.tv_usec	= ( ms * 1000) % 1000000;
	  select(0, NULL, NULL, NULL, &tval);
	}
#endif

/** Determine whether the platform is little-endian */
inline bool isPlatformLittleEndian()
{
   int n = 1;
   return *((char*)&n) ? true : false;
}

/** Setting environment variables */
#if OURO_PLATFORM == PLATFORM_WIN32
	inline void setenv(const std::string& name, const std::string& value, int overwrite)
	{
		_putenv_s(name.c_str(), value.c_str());
	}
#else
	// Linux directly using setenv
#endif

}
#endif // OURO_PLATFORM_H
