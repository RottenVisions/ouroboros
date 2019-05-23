// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_PROFILE_HANDLER_H
#define OURO_PROFILE_HANDLER_H

#include "common/common.h"
#include "common/tasks.h"
#include "common/timer.h"
#include "helper/debug_helper.h"
#include "helper/eventhistory_stats.h"
#include "network/interfaces.h"

namespace Ouroboros { 
namespace Network
{
class NetworkInterface;
class Address;
class MessageHandler;
}

class MemoryStream;

class ProfileHandler : public TimerHandler
{
public:
	ProfileHandler(Network::NetworkInterface & networkInterface, uint32 timinglen, 
		std::string name, const Network::Address& addr);
	virtual ~ProfileHandler();
	
	virtual void timeout() = 0;
	virtual void sendStream(MemoryStream* s) = 0;

	static OUROUnordered_map<std::string, OUROShared_ptr< ProfileHandler > > profiles;

protected:
	virtual void handleTimeout(TimerHandle handle, void * arg);

	Network::NetworkInterface& networkInterface_;

	TimerHandle reportLimitTimerHandle_;
	
	std::string name_;
	
	Network::Address addr_;

	uint32 timinglen_;
};

class CProfileHandler : public Task, 
						public ProfileHandler
{
public:
	CProfileHandler(Network::NetworkInterface & networkInterface, uint32 timinglen, 
		std::string name, const Network::Address& addr);
	virtual ~CProfileHandler();
	
	void timeout();
	void sendStream(MemoryStream* s);
	bool process();

private:
	struct ProfileVal
	{
		// name
		std::string		name;

		// The time after startd.
		TimeStamp		lastTime;
		TimeStamp		diff_lastTime;

		// total time of count_ times
		TimeStamp		sumTime;
		TimeStamp		diff_sumTime;

		// Record the last internal time slice
		TimeStamp		lastIntTime;
		TimeStamp		diff_lastIntTime;

		// count_ times internal total time
		TimeStamp		sumIntTime;
		TimeStamp		diff_sumIntTime;

		uint32			count;
		uint32			diff_count;
	};

	// This ProfileVal only records the initial value of default.profiles at the beginning of the timer
	// Take the difference at the end to get the result
	typedef OUROUnordered_map<std::string,  ProfileVal> PROFILEVALS;
	PROFILEVALS profileVals_;
};

class EventProfileHandler : public ProfileHandler
{
public:
	EventProfileHandler(Network::NetworkInterface & networkInterface, uint32 timinglen, 
		std::string name, const Network::Address& addr);
	virtual ~EventProfileHandler();
	
	void timeout();
	void sendStream(MemoryStream* s);

	void onTriggerEvent(const EventHistoryStats& eventHistory, const EventHistoryStats::Stats& stats, 
		uint32 size);

	static void triggerEvent(const EventHistoryStats& eventHistory, const EventHistoryStats::Stats& stats, 
		uint32 size);

private:
	struct ProfileVal
	{
		ProfileVal()
		{
			name = "";
			size = 0;
			count = 0;
		}

		// name
		std::string		name;

		uint32			size;
		uint32			count;
	};

	// This ProfileVal only records the initial value of default.profiles at the beginning of the timer
	// Take the difference at the end to get the result
	typedef OUROUnordered_map<std::string,  ProfileVal> PROFILEVALS;

	typedef OUROUnordered_map< std::string,  PROFILEVALS > PROFILEVALMAP;
	PROFILEVALMAP profileMaps_;
	
	static std::vector<EventProfileHandler*> eventProfileHandlers_;
	int removeHandle_;
};

class NetworkProfileHandler : public ProfileHandler, public Network::NetworkStatsHandler
{
public:
	NetworkProfileHandler(Network::NetworkInterface & networkInterface, uint32 timinglen, 
		std::string name, const Network::Address& addr);
	virtual ~NetworkProfileHandler();
	
	void timeout();
	void sendStream(MemoryStream* s);

	virtual void onSendMessage(const Network::MessageHandler& msgHandler, int size);
	virtual void onRecvMessage(const Network::MessageHandler& msgHandler, int size);

private:
	struct ProfileVal
	{
		ProfileVal()
		{
			name = "";
			send_size = 0;
			send_count = 0;
			send_avgsize = 0;
			total_send_size = 0;
			total_send_count = 0;

			recv_size = 0;
			recv_count = 0;
			recv_avgsize = 0;
			total_recv_size = 0;
			total_recv_count = 0;
		}

		// name
		std::string		name;

		uint32			send_size;
		uint32			send_avgsize;
		uint32			send_count;

		uint32			total_send_size;
		uint32			total_send_count;

		uint32			recv_size;
		uint32			recv_count;
		uint32			recv_avgsize;

		uint32			total_recv_size;
		uint32			total_recv_count;
	};

	typedef OUROUnordered_map<std::string,  ProfileVal> PROFILEVALS;
	PROFILEVALS profileVals_;
};

}

#endif
