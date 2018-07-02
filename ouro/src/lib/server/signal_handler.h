// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_SIGNAL_HANDLER_H
#define OURO_SIGNAL_HANDLER_H

#include "common/common.h"
#include "common/tasks.h"
#include "common/singleton.h"

	
namespace Ouroboros{
class ServerApp;

class SignalHandler
{
public:
	virtual void onSignalled(int sigNum) = 0;
};

class SignalHandlers : public Singleton<SignalHandlers>, public Task
{
public:
	SignalHandlers();
	~SignalHandlers();
	
	SignalHandler* addSignal(int sigNum, 
		SignalHandler* pSignalHandler, int flags = 0);
	
	SignalHandler* delSignal(int sigNum);
	
	void clear();
	
	void onSignalled(int sigNum);
	
	virtual bool process();

	void attachApp(ServerApp* app);

	ServerApp* getApp(){ return papp_; }

private:
	typedef std::map<int, SignalHandler*> SignalHandlerMap;
	SignalHandlerMap singnalHandlerMap_;
	
	ServerApp* papp_;
	uint8 rpos_, wpos_;
	int signalledArray_[256];

};

#define g_ouroSignalHandlers SignalHandlers::getSingleton()
}
#endif // OURO_SIGNAL_HANDLER_H
