// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com


#include "cellapp.h"
#include "spacememory.h"	
#include "entity.h"
#include "space_viewer.h"
#include "network/network_interface.h"
#include "network/event_dispatcher.h"
#include "network/address.h"
#include "network/network_stats.h"
#include "network/bundle.h"
#include "network/message_handler.h"
#include "common/memorystream.h"
#include "helper/console_helper.h"
#include "helper/profile.h"
#include "server/serverconfig.h"

namespace Ouroboros { 

//-------------------------------------------------------------------------------------
SpaceViewers::SpaceViewers():
reportLimitTimerHandle_(),
spaceViews_()
{
}

//-------------------------------------------------------------------------------------
SpaceViewers::~SpaceViewers()
{
	finalise();
}

//-------------------------------------------------------------------------------------
bool SpaceViewers::addTimer()
{
	if (!reportLimitTimerHandle_.isSet())
	{
		reportLimitTimerHandle_ = Cellapp::getSingleton().networkInterface().dispatcher().addTimer(
			1000000 / 10, this);

		return true;
	}

	return false;
}

//-------------------------------------------------------------------------------------
void SpaceViewers::finalise()
{
	clear();
	reportLimitTimerHandle_.cancel();
}

//-------------------------------------------------------------------------------------
void SpaceViewers::updateSpaceViewer(const Network::Address& addr, SPACE_ID spaceID, CELL_ID cellID, bool del)
{
	if (del)
	{
		spaceViews_.erase(addr);
		return;
	}

	SpaceViewer& viewer = spaceViews_[addr];
	viewer.updateViewer(addr, spaceID, cellID);

	addTimer();
}

//-------------------------------------------------------------------------------------
void SpaceViewers::handleTimeout(TimerHandle handle, void * arg)
{
	if (spaceViews_.size() == 0)
	{
		reportLimitTimerHandle_.cancel();
		return;
	}

	std::map< Network::Address, SpaceViewer>::iterator iter = spaceViews_.begin();
	for (; iter != spaceViews_.end(); )
	{
		// If the viewer address is not found, then erase it
		Network::Channel* pChannel = Cellapp::getSingleton().networkInterface().findChannel(iter->second.addr());
		if (pChannel == NULL)
		{
			spaceViews_.erase(iter++);
		}
		else
		{
			iter->second.timeout();
			++iter;
		}
	}
}

//-------------------------------------------------------------------------------------
SpaceViewer::SpaceViewer():
addr_(),
spaceID_(0),
cellID_(0),
viewedEntities(),
updateType_(0),
lastUpdateVersion_(0)
{
}

//-------------------------------------------------------------------------------------
SpaceViewer::~SpaceViewer()
{
}

//-------------------------------------------------------------------------------------
void SpaceViewer::resetViewer()
{
	viewedEntities.clear();
	lastUpdateVersion_ = 0;
}

//-------------------------------------------------------------------------------------
void SpaceViewer::updateViewer(const Network::Address& addr, SPACE_ID spaceID, CELL_ID cellID)
{
	addr_ = addr;

	bool chagnedSpace = spaceID_ != spaceID;

	if (chagnedSpace)
	{
		onChangedSpaceOrCell();
		spaceID_ = spaceID;
	}

	if (cellID_ != cellID)
	{
		if (!chagnedSpace)
			onChangedSpaceOrCell();

		cellID_ = cellID;
	}
}

//-------------------------------------------------------------------------------------
void SpaceViewer::onChangedSpaceOrCell()
{
	resetViewer();
}

//-------------------------------------------------------------------------------------
void SpaceViewer::timeout()
{
	switch (updateType_)
	{
	case 0: // initialize
		initClient();
		break;
	default: // update entity
		updateClient();
	};
}

//-------------------------------------------------------------------------------------
void SpaceViewer::sendStream(MemoryStream* s, int type)
{
	Network::Channel* pChannel = Cellapp::getSingleton().networkInterface().findChannel(addr_);
	if(pChannel == NULL)
	{
		WARNING_MSG(fmt::format("SpaceViewer::sendStream: not found addr({})\n",
			addr_.c_str()));

		return;
	}

	Network::Bundle* pBundle = Network::Bundle::createPoolObject(OBJECTPOOL_POINT);

	ConsoleInterface::ConsoleQuerySpacesHandler msgHandler;
	(*pBundle).newMessage(msgHandler);

	(*pBundle) << g_componentType;
	(*pBundle) << g_componentID;
	(*pBundle) << type;
	(*pBundle).append(s);
	pChannel->send(pBundle);
}

//-------------------------------------------------------------------------------------
void SpaceViewer::initClient()
{
	MemoryStream s;

	// First issue the script ID corresponding to the name of the script module, which is convenient to reduce the amount of synchronization behind the entity. The entity only synchronizes the id.
	const EntityDef::SCRIPT_MODULES& scriptModules = EntityDef::getScriptModules();
	s << (uint32)scriptModules.size();

	EntityDef::SCRIPT_MODULES::const_iterator moduleIter = scriptModules.begin();
	for (; moduleIter != scriptModules.end(); ++moduleIter)
	{
		s << moduleIter->get()->getUType();
		s << moduleIter->get()->getName();
	}

	sendStream(&s, updateType_);

	// change to update entity
	updateType_ = 1;

	lastUpdateVersion_ = 0;
}

//-------------------------------------------------------------------------------------
void SpaceViewer::updateClient()
{
	if (spaceID_ == 0)
		return;

	SpaceMemory* space = SpaceMemorys::findSpace(spaceID_);
	if (space == NULL || !space->isGood())
	{
		return;
	}

	// Up to 500 entities per update
	const int MAX_UPDATE_COUNT = 100;
	int updateCount = 0;

	// Get the difference between this time and the last result, put the difference into the stream and update to the client.
	// The difference includes the new entity and the location change of the existing entity
	MemoryStream s;

	Entities<Entity>* pEntities = Cellapp::getSingleton().pEntities();
	Entities<Entity>::ENTITYS_MAP& entitiesMap = pEntities->getEntities();

	// Check the entities that have been monitored first, for priority updates with lower version numbers
	if (updateCount < MAX_UPDATE_COUNT)
	{
		std::map< ENTITY_ID, ViewEntity >::iterator viewerIter = viewedEntities.begin();
		for (; viewerIter != viewedEntities.end(); )
		{
			if (updateCount >= MAX_UPDATE_COUNT)
				break;

			ViewEntity& viewEntity = viewerIter->second;
			if (viewEntity.updateVersion > lastUpdateVersion_)
			{
				++viewerIter;
				continue;
			}

			Entities<Entity>::ENTITYS_MAP::iterator iter = entitiesMap.find(viewerIter->first);

			// The entity could not be found, indicating that it has been destroyed or ran to another process.
			// If other processes, other processes will update them to the client
			if (iter == entitiesMap.end())
			{
				s << viewerIter->first;
				s << false; // true is updated, false is destroyed

				// remove it from viewedEntities
				viewedEntities.erase(viewerIter++);
			}
			else
			{
				Entity* pEntity = static_cast<Entity*>(iter->second.get());
				if (pEntity->spaceID() != spaceID_)
				{
					// remove it from viewedEntities
					viewedEntities.erase(viewerIter++);
					continue;
				}

				/*
				if (pEntity->cellID() != cellID_)
				{
					// remove it from viewedEntities
					viewedEntities.erase(viewerIter++);
					continue;
				}
				*/

				// There are new entities or entities that have been observed, check for location changes
				// pass if there is no change
				if ((viewEntity.position - pEntity->position()).length() <= 0.0004f &&
					(viewEntity.direction.dir - pEntity->direction().dir).length() <= 0.0004f)
				{
					++viewerIter;
					continue;
				}

				viewEntity.entityID = pEntity->id();
				viewEntity.position = pEntity->position();
				viewEntity.direction = pEntity->direction();
				++viewEntity.updateVersion;

				s << viewEntity.entityID;
				s << true; // true is updated, false is destroyed
				s << pEntity->pScriptModule()->getUType();
				s << viewEntity.position.x << viewEntity.position.y << viewEntity.position.z;
				s << viewEntity.direction.roll() << viewEntity.direction.pitch() << viewEntity.direction.yaw();

				++updateCount;
				++viewerIter;
			}
		}
	}

	// check if there are any new entities
	if (updateCount < MAX_UPDATE_COUNT)
	{
		Entities<Entity>::ENTITYS_MAP::iterator iter = entitiesMap.begin();

		for (; iter != entitiesMap.end(); ++iter)
		{
			if (updateCount >= MAX_UPDATE_COUNT)
				break;

			Entity* pEntity = static_cast<Entity*>(iter->second.get());
			if (pEntity->spaceID() != spaceID_)
				continue;

			/*
			if (pEntity->cellID() != cellID_)
				continue;
			*/

			std::map< ENTITY_ID, ViewEntity >::iterator findIter = viewedEntities.find(pEntity->id());
			ViewEntity& viewEntity = viewedEntities[pEntity->id()];

			if (findIter != viewedEntities.end())
				continue;

			viewEntity.entityID = pEntity->id();
			viewEntity.position = pEntity->position();
			viewEntity.direction = pEntity->direction();
			viewEntity.updateVersion = lastUpdateVersion_ + 1;

			++updateCount;

			s << viewEntity.entityID;
			s << true; // true is updated, false is destroyed
			s << pEntity->pScriptModule()->getUType();
			s << viewEntity.position.x << viewEntity.position.y << viewEntity.position.z;
			s << viewEntity.direction.roll() << viewEntity.direction.pitch() << viewEntity.direction.yaw();
		}
	}

	sendStream(&s, updateType_);

	// If all updates are completed, replace the version number
	if (updateCount < MAX_UPDATE_COUNT)
		++lastUpdateVersion_;
}

//-------------------------------------------------------------------------------------

}
