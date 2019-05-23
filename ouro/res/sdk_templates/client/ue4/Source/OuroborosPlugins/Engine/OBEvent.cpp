
#include "OBEvent.h"
#include "KBDebug.h"

TMap<FString, TArray<OBEvent::EventObj>> OBEvent::events_;
TArray<OBEvent::FiredEvent*>	OBEvent::firedEvents_;
bool OBEvent::isPause_ = false;

OBEvent::OBEvent()
{
}

OBEvent::~OBEvent()
{
}

void OBEvent::clear()
{
	events_.Empty();
	clearFiredEvents();
}

void OBEvent::clearFiredEvents()
{
	while (firedEvents_.Num() > 0)
	{
		FiredEvent* event = firedEvents_.Pop();
		event->args->ConditionalBeginDestroy();
		delete event;
	}
}

bool OBEvent::registerEvent(const FString& eventName, const FString& funcName, TFunction<void(const UOBEventData*)> func, void* objPtr)
{
	TArray<EventObj>* eo_array = NULL;
	TArray<EventObj>* eo_array_find = events_.Find(eventName);

	if (!eo_array_find)
	{
		events_.Add(eventName, TArray<EventObj>());
		eo_array = &(*events_.Find(eventName));
	}
	else
	{
		eo_array = &(*eo_array_find);
	}

	EventObj eo;
	eo.funcName = funcName;
	eo.method = func;
	eo.objPtr = objPtr;
	eo_array->Add(eo);
	return true;
}

bool OBEvent::deregister(void* objPtr, const FString& eventName, const FString& funcName)
{
	TArray<EventObj>* eo_array_find = events_.Find(eventName);
	if (!eo_array_find || (*eo_array_find).Num() == 0)
	{
		return false;
	}

	// traverse from the back to avoid the problem of deleting in the middle
	for (int i = (*eo_array_find).Num() - 1; i >= 0; --i)
	{
		EventObj& item = (*eo_array_find)[i];
		if (objPtr == item.objPtr && (funcName.Len() == 0 || funcName == item.funcName))
		{
			(*eo_array_find).RemoveAt(i, 1);
		}
	}

	removeFiredEvent(objPtr, eventName, funcName);

	return true;
}

bool OBEvent::deregister(void* objPtr)
{
	for (auto& item : events_)
	{
		deregister(objPtr, item.Key, TEXT(""));
	}

	return true;
}

void OBEvent::fire(const FString& eventName, UOBEventData* pEventData)
{
	TArray<EventObj>* eo_array_find = events_.Find(eventName);
	if (!eo_array_find || (*eo_array_find).Num() == 0)
	{
		//SCREEN_WARNING_MSG("OBEvent::fire(): event(%s) not found!", *eventName);
		return;
	}

	pEventData->eventName = eventName;

	for (auto& item : (*eo_array_find))
	{
		if (!isPause_)
		{
			item.method(pEventData);
			pEventData->ConditionalBeginDestroy();
		} 
		else
		{
			FiredEvent* event = new FiredEvent;
			event->evt = item;
			event->eventName = eventName;
			event->args = pEventData;
			firedEvents_.Emplace(event);
		}
	}

	//GetWorld()->ForceGarbageCollection(true);
}

void OBEvent::pause()
{
	isPause_ = true;
}

void OBEvent::resume()
{
	isPause_ = false;
	while (firedEvents_.Num() > 0)
	{
		FiredEvent* event = firedEvents_.Pop();
		event->evt.method(event->args);
		event->args->ConditionalBeginDestroy();
		delete event;
	}
}

void OBEvent::removeFiredEvent(void* objPtr, const FString& eventName /*= TEXT("")*/, const FString& funcName /*= TEXT("")*/)
{
	while (true)
	{
		bool found = false;
		for (auto item : firedEvents_)
		{
			bool ret = eventName.Len() == 0 && funcName.Len() == 0 || item->eventName == eventName && (funcName.Len() == 0 || item->evt.funcName == funcName);
			if (ret && item->evt.objPtr == objPtr)
			{
				firedEvents_.Remove(item);
				item->args->ConditionalBeginDestroy();
				delete item;
				found = true;
				break;
			}
		}

		if (!found)
			break;
	}
}
