// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_SCRIPT_TIMERS_H
#define OURO_SCRIPT_TIMERS_H
#include "common/common.h"
#include "common/timer.h"

namespace Ouroboros
{

class MemoryStream;
class ServerApp;

class ScriptTimers
{
public:
	ScriptTimers();
	~ScriptTimers();

	static void initialize(ServerApp & app);
	static void finalise(ServerApp & app);

	ScriptID addTimer(float initialOffset, float repeatOffset, int userArg,
			TimerHandler * pHandler);
	bool delTimer(ScriptID timerID);

	void releaseTimer(TimerHandle handle);

	void cancelAll();

	ScriptID getIDForHandle(TimerHandle handle) const;

	bool isEmpty() const	{ return map_.empty(); }

	typedef std::map<ScriptID, TimerHandle> Map;

	ScriptTimers::Map& map(){ return map_; }

	void directAddTimer(ScriptID tid, TimerHandle handle);

private:
	
	ScriptID getNewID();
	Map::const_iterator findTimer(TimerHandle handle) const;
	Map::iterator findTimer(TimerHandle handle);

	Map map_;
};


namespace ScriptTimersUtil
{
	ScriptID addTimer( ScriptTimers ** ppTimers,
		float initialOffset, float repeatOffset, int userArg,
		TimerHandler * pHandler );

	bool delTimer( ScriptTimers * pTimers, ScriptID timerID );
	void releaseTimer( ScriptTimers ** ppTimers, TimerHandle handle );
	void cancelAll( ScriptTimers * pTimers );

	ScriptID getIDForHandle( ScriptTimers * pTimers,
			TimerHandle handle );
}

}
#endif 
