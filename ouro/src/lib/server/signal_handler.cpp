// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com


#include "signal_handler.h"
#include "helper/debug_helper.h"
#include "server/serverapp.h"

namespace Ouroboros{
OURO_SINGLETON_INIT(SignalHandlers);

const int SIGMIN = 1;
const int SIGMAX = SIGSYS;

const char * SIGNAL_NAMES[] = 
{
	NULL,
	"SIGHUP",
	"SIGINT",
	"SIGQUIT",
	"SIGILL",
	"SIGTRAP",
	"SIGABRT",
	"SIGBUS",
	"SIGFPE",
	"SIGKILL",
	"SIGUSR1",
	"SIGSEGV",
	"SIGUSR2",
	"SIGPIPE",
	"SIGALRM",
	"SIGTERM",
	"SIGSTKFLT",
	"SIGCHLD",
	"SIGCONT",
	"SIGSTOP",
	"SIGTSTP",
	"SIGTTIN",
	"SIGTTOU",
	"SIGURG",
	"SIGXCPU",
	"SIGXFSZ",
	"SIGVTALRM",
	"SIGPROF",
	"SIGWINCH",
	"SIGIO",
	"SIGPWR",
	"SIGSYS"
};

SignalHandlers g_signalHandlers;

std::string SIGNAL2NAMES(int signum)
{
	if (signum >= SIGMIN && signum <= SIGMAX)
	{
		return SIGNAL_NAMES[signum];
	}

	return fmt::format("unknown({})", signum);
}

void signalHandler(int signum)
{
	printf("SignalHandlers: receive sigNum %d.\n", signum);
	g_signalHandlers.onSignalled(signum);
};

//-------------------------------------------------------------------------------------
SignalHandlers::SignalHandlers():
singnalHandlerMap_(),
papp_(NULL),
rpos_(0),
wpos_(0)
{
}

//-------------------------------------------------------------------------------------
SignalHandlers::~SignalHandlers()
{
}

//-------------------------------------------------------------------------------------
void SignalHandlers::attachApp(ServerApp* app)
{ 
	papp_ = app; 
	app->dispatcher().addTask(this);
}

//-------------------------------------------------------------------------------------	
bool SignalHandlers::ignoreSignal(int sigNum)
{
#if OURO_PLATFORM != PLATFORM_WIN32
	if (signal(sigNum, SIG_IGN) == SIG_ERR)
		return false;
#endif

	return true;
}

//-------------------------------------------------------------------------------------	
SignalHandler* SignalHandlers::addSignal(int sigNum, 
	SignalHandler* pSignalHandler, int flags)
{
	// Allow to be reset
	// SignalHandlerMap::iterator iter = singnalHandlerMap_.find(sigNum);
	// OURO_ASSERT(iter == singnalHandlerMap_.end());

	singnalHandlerMap_[sigNum] = pSignalHandler;

#if OURO_PLATFORM != PLATFORM_WIN32
	struct sigaction action;
	action.sa_handler = &signalHandler;
	sigfillset( &(action.sa_mask) );

	if (flags & SA_SIGINFO)
	{
		ERROR_MSG( "ServerApp::installSingnal: "
				"SA_SIGINFO is not supported, ignoring\n" );
		flags &= ~SA_SIGINFO;
	}

	action.sa_flags = flags;

	::sigaction( sigNum, &action, NULL );
#endif

	return pSignalHandler;
}
	
//-------------------------------------------------------------------------------------	
SignalHandler* SignalHandlers::delSignal(int sigNum)
{
	SignalHandlerMap::iterator iter = singnalHandlerMap_.find(sigNum);
	OURO_ASSERT(iter != singnalHandlerMap_.end());
	SignalHandler* pSignalHandler = iter->second;
	singnalHandlerMap_.erase(iter);
	return pSignalHandler;
}
	
//-------------------------------------------------------------------------------------	
void SignalHandlers::clear()
{
	singnalHandlerMap_.clear();
}

//-------------------------------------------------------------------------------------	
void SignalHandlers::onSignalled(int sigNum)
{
	// don't allocate memory
	OURO_ASSERT(wpos_ != 0XFF);
	signalledArray_[wpos_++] = sigNum;
}

//-------------------------------------------------------------------------------------	
bool SignalHandlers::process()
{
	if (wpos_ == 0)
		return true;

	DEBUG_MSG(fmt::format("SignalHandlers::process: rpos={}, wpos={}.\n", rpos_, wpos_));

#if OURO_PLATFORM != PLATFORM_WIN32
	/* If the signal has more than 255 trigger demand, you can open the comment, it will block all signals and other signals after the execution is completed, and then change the signallededArray_ to the signal set type.
	if (wpos_ == 1 && signalledArray_[0] == SIGALRM)
		return true;

	sigset_t mask, old_mask;
	sigemptyset(&mask);
	sigemptyset(&old_mask);

	sigfillset(&mask);

	// Shield signal
	sigprocmask(SIG_BLOCK, &mask, &old_mask);
	*/
#endif

	while (rpos_ < wpos_)
	{
		int sigNum = signalledArray_[rpos_++];

#if OURO_PLATFORM != PLATFORM_WIN32
		//if (SIGALRM == sigNum)
		//	continue;
#endif

		SignalHandlerMap::iterator iter1 = singnalHandlerMap_.find(sigNum);
		if (iter1 == singnalHandlerMap_.end())
		{
			DEBUG_MSG(fmt::format("SignalHandlers::process: sigNum {} unhandled, singnalHandlerMap({}).\n",
				SIGNAL2NAMES(sigNum), singnalHandlerMap_.size()));

			continue;
		}

		DEBUG_MSG(fmt::format("SignalHandlers::process: sigNum {} handle. singnalHandlerMap({})\n", SIGNAL2NAMES(sigNum), singnalHandlerMap_.size()));
		iter1->second->onSignalled(sigNum);
	}

	rpos_ = 0;
	wpos_ = 0;

#if OURO_PLATFORM != PLATFORM_WIN32
	// restore the mask
	/*
	sigprocmask(SIG_SETMASK, &old_mask, NULL);

	addSignal(SIGALRM, NULL);

	// Get the current signal mask
	sigprocmask(0, NULL, &mask);

	// Unblock SIGALRM
	sigdelset(&mask, SIGALRM);

	// Wait with this mask
	ualarm(1, 0);

	// Re-trigger the signal that was missed during the period
	sigsuspend(&mask);

	delSignal(SIGALRM);
	*/
#endif

	return true;
}

//-------------------------------------------------------------------------------------		
}
