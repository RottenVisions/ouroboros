// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com


#include "eventhistory_stats.h"
#include "profile_handler.h"

namespace Ouroboros { 

//-------------------------------------------------------------------------------------
EventHistoryStats::EventHistoryStats(std::string name):
stats_(),
name_(name)
{
}

//-------------------------------------------------------------------------------------
EventHistoryStats::~EventHistoryStats()
{
}

//-------------------------------------------------------------------------------------
void EventHistoryStats::trackEvent(const std::string& type, const std::string& name, uint32 size, const char* flags)
{
	std::string fullname = type + flags + name;
	
	if(size >= PACKET_MAX_SIZE_TCP)
	{
		if(size < NETWORK_MESSAGE_MAX_SIZE)
		{
			WARNING_MSG(fmt::format("EventHistoryStats::trackEvent[{}]: message size({}) >= PACKET_MAX_SIZE_TCP({}).\n",
				fullname, size, PACKET_MAX_SIZE_TCP));
		}
		else
		{
			ERROR_MSG(fmt::format("EventHistoryStats::trackEvent[{}]: message size({}) > NETWORK_MESSAGE_MAX_SIZE({}).\n",
				fullname, size, NETWORK_MESSAGE_MAX_SIZE));
		}
	}

	STATS::iterator iter = stats_.find(fullname);
	if(iter == stats_.end())
	{
		stats_[name].name = fullname;
		stats_[name].size += size;
		stats_[name].count++;
		EventProfileHandler::triggerEvent(*this, stats_[name], size);
		return;
	}

	iter->second.size += size;
	iter->second.count++;
	
	EventProfileHandler::triggerEvent(*this, iter->second, size);
}

//-------------------------------------------------------------------------------------
}
