// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#include "event.h"
#include "helper/debug_helper.h"

namespace Ouroboros{	

//-------------------------------------------------------------------------------------
EventHandler::EventHandler():
eventHandles_()
{
}

//-------------------------------------------------------------------------------------
EventHandler::~EventHandler()
{
}

//-------------------------------------------------------------------------------------
bool EventHandler::registerHandle(EventHandle* pHhandle)
{
	if(std::binary_search(eventHandles_.begin(), eventHandles_.end(), pHhandle))
	{
		return false;
	}
	
	eventHandles_.push_back(pHhandle);
	return true;
}

//-------------------------------------------------------------------------------------
bool EventHandler::deregisterHandle(EventHandle* pHhandle)
{
	EVENT_HANDLES::iterator iter = eventHandles_.begin();
	for(; iter != eventHandles_.end(); ++iter)
	{
		if((*iter) == pHhandle)
		{
			eventHandles_.erase(iter);
			return true;
		}
	}

	return false;
}

//-------------------------------------------------------------------------------------
void EventHandler::fire(const EventData* lpEventData)
{
	EVENT_HANDLES::iterator iter = eventHandles_.begin();
	for(; iter != eventHandles_.end(); ++iter)
	{
		(*iter)->ouroboros_onEvent(lpEventData);
	}
}

//-------------------------------------------------------------------------------------

}
