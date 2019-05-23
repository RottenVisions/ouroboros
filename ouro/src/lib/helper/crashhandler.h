// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_CRASHHANDLER_H
#define OURO_CRASHHANDLER_H
	
// common include	
//#define NDEBUG
// windows include	
#ifdef WIN32
#include "common/common.h"
#include <windows.h>
#include <tchar.h>
#include <dbghelp.h>
#include <stdio.h>
#include <crtdbg.h>
#include <time.h> 
#pragma comment (lib, "dbghelp.lib")
#else
// linux include
#include <errno.h>
#endif
	
namespace Ouroboros{ namespace exception {
/** Installation*/
void installCrashHandler(int svnVer, const char* dumpType);

/** Create dump file function*/
void createMiniDump(EXCEPTION_POINTERS* pep ); 

/** Custom minidump callback*/
BOOL CALLBACK dumpCallback(
	PVOID                            pParam, 
	const PMINIDUMP_CALLBACK_INPUT   pInput, 
	PMINIDUMP_CALLBACK_OUTPUT        pOutput 
); 

#ifndef _DEBUG
		/** Write this macro at the very beginning of the code to intercept the crash.*/
	#define THREAD_TRY_EXECUTION int exceptionCode = 0;																												\
								__try{
		
		/** Write this macro at the end of the code to intercept the crash.*/
	#define THREAD_HANDLE_CRASH  }__except(exceptionCode = GetExceptionCode(), Ouroboros::exception::createMiniDump(GetExceptionInformation()),						\
															EXCEPTION_EXECUTE_HANDLER) {																			\
									printf("%x\n", exceptionCode);																									\
									wchar_t msg[512];																												\
									wsprintf(msg, L"Exception happened. Exception code is %x.", exceptionCode);														\
									MessageBox(NULL, msg, L"Exception", MB_OK);																						\
								}
#else
	#define THREAD_TRY_EXECUTION 
	#define THREAD_HANDLE_CRASH 
#endif

}
}
#endif
