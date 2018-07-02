// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#include "history_event.h"	
#include "network/message_handler.h"

namespace Ouroboros{	


//-------------------------------------------------------------------------------------
HistoryEvent::HistoryEvent(HistoryEventID id, const Network::MessageHandler& msgHandler, uint32 msglen):
id_(id),
msglen_(msglen),
msgHandler_(msgHandler)
{
}

//-------------------------------------------------------------------------------------
HistoryEvent::~HistoryEvent()
{
}

//-------------------------------------------------------------------------------------
EventHistory::EventHistory()
{
}

//-------------------------------------------------------------------------------------
EventHistory::~EventHistory()
{
}

//-------------------------------------------------------------------------------------
bool EventHistory::add(HistoryEvent* phe)
{
	return true;
}

//-------------------------------------------------------------------------------------
bool EventHistory::remove(HistoryEvent* phe)
{
	return true;
}

//-------------------------------------------------------------------------------------
}
