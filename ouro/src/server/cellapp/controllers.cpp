// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#include "controllers.h"
#include "cellapp.h"
#include "entity.h"
#include "helper/profile.h"
#include "common/memorystream.h"

#include "proximity_controller.h"
#include "moveto_point_handler.h"
#include "moveto_entity_handler.h"
#include "navigate_handler.h"

namespace Ouroboros{


//-------------------------------------------------------------------------------------
Controllers::Controllers(ENTITY_ID entityID):
lastid_(0),
entityID_(entityID)
{
}

//-------------------------------------------------------------------------------------
Controllers::~Controllers()
{
	clear();
}

//-------------------------------------------------------------------------------------
void Controllers::clear()
{
	objects_.clear();
	lastid_ = 0;
}

//-------------------------------------------------------------------------------------
bool Controllers::add(OUROShared_ptr<Controller> pController)
{
	uint32 id = pController->id();
	if(id == 0)
	{
		id = freeID();
	}
	else
	{
		// Refresh id counter
		if(lastid_ < id)
			lastid_ = id;
	}

	objects_[id] = pController;
	pController->id(id);
	pController->pControllers(this);

	if(objects_.size() > 32)
	{
		WARNING_MSG(fmt::format("Controllers::add: size = {}.\n", objects_.size()));
	}

	return true;
}

//-------------------------------------------------------------------------------------
bool Controllers::remove(OUROShared_ptr<Controller> pController)
{
	return remove(pController->id());
}

//-------------------------------------------------------------------------------------
bool Controllers::remove(uint32 id)
{
	CONTROLLERS_MAP::iterator iter = objects_.find(id);
	if (iter == objects_.end())
		return true;

	// To be a reference to prevent the controller from destructing and causing some problems in the case where the erase is not completed and then enter here to perform erase.
	OUROShared_ptr< Controller > pController = iter->second;
	objects_.erase(iter);
	return pController != NULL;
}

//-------------------------------------------------------------------------------------
void Controllers::addToStream(Ouroboros::MemoryStream& s)
{
	uint32 size = (uint32)objects_.size();
	s << lastid_ << size;

	CONTROLLERS_MAP::iterator iter = objects_.begin();
	for(; iter != objects_.end(); ++iter)
	{
		uint8 itype = (uint8)iter->second->type();
		s << itype;
		iter->second->addToStream(s);
	}
}

//-------------------------------------------------------------------------------------
void Controllers::createFromStream(Ouroboros::MemoryStream& s)
{
	uint32 size = 0;
	s >> lastid_ >> size;

	Entity* pEntity = Cellapp::getSingleton().findEntity(entityID_);
	OURO_ASSERT(pEntity);

	for(uint32 i=0; i<size; ++i)
	{
		uint8 itype;
		s >> itype;

		Controller::ControllerType type = (Controller::ControllerType)itype;

		OUROShared_ptr<Controller> pController;

		switch(type)
		{
		case Controller::CONTROLLER_TYPE_PROXIMITY:
			pController = OUROShared_ptr<Controller>(new ProximityController(pEntity));
			break;
		case Controller::CONTROLLER_TYPE_ROTATE:
		case Controller::CONTROLLER_TYPE_MOVE:
		default:
			OURO_ASSERT(false);
			break;
		};

		if(pController == NULL)
			continue;

		pController->type(type);
		pController->createFromStream(s);

		add(pController);
	}
}

//-------------------------------------------------------------------------------------
}
