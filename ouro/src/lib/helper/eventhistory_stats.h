// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_EVENT_HISTORY_STATS_H
#define OURO_EVENT_HISTORY_STATS_H

#include "common/common.h"

namespace Ouroboros {

/*
	Event History
*/
class EventHistoryStats
{
public:

	struct Stats
	{
		Stats()
		{
			name = "";
			size = 0;
			count = 0;
		}

		std::string name;
		uint32 size;
		uint32 count;
	};

	typedef OUROUnordered_map<std::string, Stats> STATS;

	EventHistoryStats(std::string name);
	~EventHistoryStats();

	void trackEvent(const std::string& type, const std::string& name, uint32 size, const char* flags = ".");

	EventHistoryStats::STATS& stats(){ return stats_; }

	const char* name() const { return name_.c_str(); }
private:
	STATS stats_;

	std::string name_;
};

}

#endif // OURO_EVENT_HISTORY_STATS_H
