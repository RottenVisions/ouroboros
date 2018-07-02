// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com


#include "forward_messagebuffer.h"
#include "network/bundle.h"
#include "network/channel.h"
#include "network/event_dispatcher.h"
#include "network/network_interface.h"

namespace Ouroboros {
OURO_SINGLETON_INIT(ForwardComponent_MessageBuffer);
OURO_SINGLETON_INIT(ForwardAnywhere_MessageBuffer);
//-------------------------------------------------------------------------------------
ForwardComponent_MessageBuffer::ForwardComponent_MessageBuffer(Network::NetworkInterface & networkInterface) :
	Task(),
	networkInterface_(networkInterface),
	start_(false)
{
	// dispatcher().addTask(this);
}

//-------------------------------------------------------------------------------------
ForwardComponent_MessageBuffer::~ForwardComponent_MessageBuffer()
{
	//dispatcher().cancelTask(this);
	clear();
}

//-------------------------------------------------------------------------------------
void ForwardComponent_MessageBuffer::clear()
{
	MSGMAP::iterator iter = pMap_.begin();
	for(; iter != pMap_.end(); ++iter)
	{
		std::vector<ForwardItem*>::iterator itervec = iter->second.begin();

		for(; itervec != iter->second.end(); ++itervec)
		{
			SAFE_RELEASE((*itervec)->pBundle);
			SAFE_RELEASE((*itervec)->pHandler);
			SAFE_RELEASE((*itervec));
		}
	}

	pMap_.clear();
}

//-------------------------------------------------------------------------------------
Network::EventDispatcher & ForwardComponent_MessageBuffer::dispatcher()
{
	return networkInterface_.dispatcher();
}

//-------------------------------------------------------------------------------------
void ForwardComponent_MessageBuffer::push(COMPONENT_ID componentID, ForwardItem* pHandler)
{
	if(!start_)
	{
		dispatcher().addTask(this);
		start_ = true;
	}

	pMap_[componentID].push_back(pHandler);
}

//-------------------------------------------------------------------------------------
bool ForwardComponent_MessageBuffer::process()
{
	if(pMap_.size() <= 0)
	{
		start_ = false;
		return false;
	}

	MSGMAP::iterator iter = pMap_.begin();
	for(; iter != pMap_.end(); )
	{
		Components::ComponentInfos* cinfos = Components::getSingleton().findComponent(iter->first);
		if(cinfos == NULL || cinfos->pChannel == NULL)
			return true;

		// If it is mgr component needs to determine whether it has been initialized
		if(g_componentType == CELLAPPMGR_TYPE || g_componentType == BASEAPPMGR_TYPE)
		{
			if(cinfos->state != COMPONENT_STATE_RUN)
				return true;
		}

		if(iter->second.size() == 0)
		{
			pMap_.erase(iter++);
		}
		else
		{
			int icount = 5;

			std::vector<ForwardItem*>::iterator itervec = iter->second.begin();

			for(; itervec != iter->second.end(); )
			{
				if (!(*itervec)->isOK())
					return true;

				cinfos->pChannel->send((*itervec)->pBundle);
				(*itervec)->pBundle = NULL;

				if((*itervec)->pHandler != NULL)
				{
					(*itervec)->pHandler->process();
					SAFE_RELEASE((*itervec)->pHandler);
				}

				SAFE_RELEASE((*itervec));

				itervec = iter->second.erase(itervec);

				if(--icount <= 0)
					return true;
			}

			DEBUG_MSG(fmt::format("ForwardComponent_MessageBuffer::process(): size:{}.\n", iter->second.size()));
			iter->second.clear();
			++iter;
		}
	}

	return true;
}

//-------------------------------------------------------------------------------------
ForwardAnywhere_MessageBuffer::ForwardAnywhere_MessageBuffer(Network::NetworkInterface & networkInterface, COMPONENT_TYPE forwardComponentType) :
	Task(),
	networkInterface_(networkInterface),
	forwardComponentType_(forwardComponentType),
	start_(false)
{
	// dispatcher().addTask(this);
}

//-------------------------------------------------------------------------------------
ForwardAnywhere_MessageBuffer::~ForwardAnywhere_MessageBuffer()
{
	//dispatcher().cancelTask(this);

	clear();
}

//-------------------------------------------------------------------------------------
void ForwardAnywhere_MessageBuffer::clear()
{
	std::vector<ForwardItem*>::iterator iter = pBundles_.begin();
	for(; iter != pBundles_.end(); )
	{
		SAFE_RELEASE((*iter)->pBundle);
		SAFE_RELEASE((*iter)->pHandler);
	}

	pBundles_.clear();
}

//-------------------------------------------------------------------------------------
Network::EventDispatcher & ForwardAnywhere_MessageBuffer::dispatcher()
{
	return networkInterface_.dispatcher();
}

//-------------------------------------------------------------------------------------
void ForwardAnywhere_MessageBuffer::push(ForwardItem* pHandler)
{
	if(!start_)
	{
		dispatcher().addTask(this);
		start_ = true;
	}

	pBundles_.push_back(pHandler);
}

//-------------------------------------------------------------------------------------
bool ForwardAnywhere_MessageBuffer::process()
{
	if(pBundles_.size() <= 0)
	{
		start_ = false;
		return false;
	}

	Components::COMPONENTS& cts = Components::getSingleton().getComponents(forwardComponentType_);
	size_t idx = 0;

	if(cts.size() > 0)
	{
		bool hasEnabled = (g_componentType != CELLAPPMGR_TYPE && g_componentType != BASEAPPMGR_TYPE);

		Components::COMPONENTS::iterator ctiter = cts.begin();
		for(; ctiter != cts.end(); ++ctiter)
		{
			// All component channels must be set, and wait if they are not.
			if((*ctiter).pChannel == NULL)
				return true;

			if((*ctiter).state == COMPONENT_STATE_RUN)
				hasEnabled = true;
		}

		// There must be a process available
		if(!hasEnabled)
			return true;

		// Up to 5 ticks per tick
		int icount = 5;

		std::vector<ForwardItem*>::iterator iter = pBundles_.begin();
		for (; iter != pBundles_.end(); ++iter)
		{
			if ((*iter)->isOK())
				break;
		}

		// All ForwardItems must be in the ok state
		// When not in ok? For example, the ForwardItem in cellappmgr needs to wait for the initialization of the cellapp to complete before the ok
		if (iter == pBundles_.end())
			return true;

		for(; iter != pBundles_.end(); )
		{
			Network::Channel* pChannel = NULL;

			if(g_componentType != CELLAPPMGR_TYPE && g_componentType != BASEAPPMGR_TYPE)
			{
				pChannel = cts[idx++].pChannel;
				if(idx >= cts.size())
					idx = 0;
			}
			else
			{
				while(pChannel == NULL)
				{
					if(cts[idx].state != COMPONENT_STATE_RUN)
					{
						if(++idx >= cts.size())
							idx = 0;

						continue;
					}

					pChannel = cts[idx++].pChannel;
					if(idx >= cts.size())
						idx = 0;
				}
			}

			pChannel->send((*iter)->pBundle);
			(*iter)->pBundle = NULL;

			if((*iter)->pHandler != NULL)
			{
				(*iter)->pHandler->process();
				SAFE_RELEASE((*iter)->pHandler);
			}

			SAFE_RELEASE((*iter));

			iter = pBundles_.erase(iter);

			if(--icount <= 0)
				return true;
		}

		DEBUG_MSG(fmt::format("ForwardAnywhere_MessageBuffer::process(): size:{}.\n", pBundles_.size()));
		start_ = false;
		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------
}
