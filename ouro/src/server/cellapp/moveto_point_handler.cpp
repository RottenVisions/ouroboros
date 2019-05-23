// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#include "cellapp.h"
#include "entity.h"
#include "moveto_point_handler.h"	
#include "move_controller.h"	

namespace Ouroboros{	


//-------------------------------------------------------------------------------------
MoveToPointHandler::MoveToPointHandler(KBEShared_ptr<Controller>& pController, int layer, const Position3D& destPos, 
											 float velocity, float distance, bool faceMovement, 
											bool moveVertically, PyObject* userarg):
destPos_(destPos),
velocity_(velocity),
faceMovement_(faceMovement),
moveVertically_(moveVertically),
pyuserarg_(userarg),
distance_(distance),
pController_(pController),
layer_(layer),
isDestroyed_(false)
{
	updatableName = "MoveToPointHandler";

	Py_INCREF(userarg);

	//std::static_pointer_cast<MoveController>(pController)->pMoveToPointHandler(this);
	static_cast<MoveController*>(pController.get())->pMoveToPointHandler(this);
	Cellapp::getSingleton().addUpdatable(this);
}

//-------------------------------------------------------------------------------------
MoveToPointHandler::MoveToPointHandler():
destPos_(),
velocity_(0.f),
faceMovement_(false),
moveVertically_(false),
pyuserarg_(NULL),
distance_(0.f),
layer_(0),
isDestroyed_(false)
{
	updatableName = "MoveToPointHandler";

	Cellapp::getSingleton().addUpdatable(this);
}

//-------------------------------------------------------------------------------------
MoveToPointHandler::~MoveToPointHandler()
{
	if(pyuserarg_ != NULL)
	{
		Py_DECREF(pyuserarg_);
	}

	//DEBUG_MSG(fmt::format("MoveToPointHandler::~MoveToPointHandler(): {}\n", (void*)this));
}

//-------------------------------------------------------------------------------------
void MoveToPointHandler::addToStream(Ouroboros::MemoryStream& s)
{
	// uint8 utype = type();

	s << /*utype <<*/ destPos_.x << destPos_.y << destPos_.z << velocity_ << faceMovement_ << moveVertically_ <<
		distance_ << layer_;

	s.appendBlob(script::Pickler::pickle(pyuserarg_));
}

//-------------------------------------------------------------------------------------
void MoveToPointHandler::createFromStream(Ouroboros::MemoryStream& s)
{
	s >> /*utype <<*/ destPos_.x >> destPos_.y >> destPos_.z >> velocity_ >> faceMovement_ >> moveVertically_ >>
		distance_ >> layer_;

	std::string val = "";
	s.readBlob(val);

	pyuserarg_ = script::Pickler::unpickle(val);
}

//-------------------------------------------------------------------------------------
bool MoveToPointHandler::requestMoveOver(const Position3D& oldPos)
{
	if(pController_)
	{
		if(pController_->pEntity())
			pController_->pEntity()->onMoveOver(pController_->id(), layer_, oldPos, pyuserarg_);

		// If calling cancelController(id) in onMoveOver will cause the MoveController to destruct pController_ to be NULL
		pController_->destroy();
	}

	return true;
}

//-------------------------------------------------------------------------------------
bool MoveToPointHandler::update()
{
	if (isDestroyed_)
	{
		delete this;
		return false;
	}
	
	Entity* pEntity = pController_->pEntity();
	Py_INCREF(pEntity);

	const Position3D& dstPos = destPos();
	Position3D currpos = pEntity->position();
	Position3D currpos_backup = currpos;
	Direction3D direction = pEntity->direction();

	Vector3 movement = dstPos - currpos;
	if (!moveVertically_) movement.y = 0.f;
	
	bool ret = true;
	float dist_len = KBEVec3Length(&movement);

	if (dist_len < velocity_ + distance_)
	{
		float y = currpos.y;

		if (distance_ > 0.0f)
		{
			// unitized vector
			KBEVec3Normalize(&movement, &movement); 
				
			if(dist_len > distance_)
			{
				movement *= distance_;
				currpos = dstPos - movement;
			}
		}
		else
		{
			currpos = dstPos;
		}

		if (!moveVertically_)
			currpos.y = y;

		ret = false;
	}
	else
	{
		// unitized vector
		KBEVec3Normalize(&movement, &movement); 

				// move Place
		movement *= velocity_;
		currpos += movement;
	}
	
	// Do you need to change your orientation?
	if (faceMovement_)
	{
		if (movement.x != 0.f || movement.z != 0.f)
			direction.yaw(movement.yaw());

		if (movement.y != 0.f)
			direction.pitch(movement.pitch());
	}
	
	// Set the new location and orientation of the entity
	if(!isDestroyed_)
		pEntity->setPositionAndDirection(currpos, direction);

	// non-navigate can't be sure it's on the ground
	if(!isDestroyed_)
		pEntity->isOnGround(isOnGround());

	// notification script
	if(!isDestroyed_)
		pEntity->onMove(pController_->id(), layer_, currpos_backup, pyuserarg_);

	// If it is stopped during the onMove process, or if it reaches its destination, it will be destroyed directly and will return false.
	if (isDestroyed_ || 
		(!ret && requestMoveOver(currpos_backup)))
	{
		Py_DECREF(pEntity);
		delete this;
		return false;
	}

	Py_DECREF(pEntity);
	return true;
}

//-------------------------------------------------------------------------------------
}

