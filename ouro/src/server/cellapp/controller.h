// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_CONTROLLER_H
#define OURO_CONTROLLER_H

// common include
#include "helper/debug_helper.h"
#include "common/common.h"


namespace Ouroboros{

class Entity;
class Controllers;
class MemoryStream;

/*
	Controller, management trap, Vision, etc. Wait
*/
class Controller
{
public:
	enum ControllerType
	{
		CONTROLLER_TYPE_NORMAL = 0,			// Conventional type
		CONTROLLER_TYPE_PROXIMITY = 1,		// Range trigger type
		CONTROLLER_TYPE_MOVE = 2,			// Mobile controller type
		CONTROLLER_TYPE_ROTATE = 3,			// Rotary controller type
	};

	Controller(Controller::ControllerType type, Entity* pEntity, int32 userarg, uint32 id = 0);
	Controller(Entity* pEntity);
	virtual ~Controller();

	uint32 id() { return id_; }
	void id(uint32 v) { id_ = v; }

	int32 userarg() const { return userarg_; }

	Entity* pEntity() const { return pEntity_; }

	void pControllers(Controllers* v) { pControllers_ = v; }

	virtual void destroy();

	Controller::ControllerType type() { return type_; }
	void type(Controller::ControllerType t) { type_ = t; }

	virtual void addToStream(Ouroboros::MemoryStream& s);
	virtual void createFromStream(Ouroboros::MemoryStream& s);

protected:
	uint32 id_;
	Entity* pEntity_;

	int32 userarg_;

	Controllers* pControllers_;

	ControllerType type_;

};

}
#endif // OURO_CONTROLLER_H
