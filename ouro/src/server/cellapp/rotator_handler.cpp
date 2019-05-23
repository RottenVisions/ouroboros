// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#include "cellapp.h"
#include "entity.h"
#include "rotator_handler.h"
#include "turn_controller.h"

namespace Ouroboros{	


//-------------------------------------------------------------------------------------
RotatorHandler::RotatorHandler(KBEShared_ptr<Controller> pController, const Direction3D& destDir, float velocity, PyObject* userarg):
destDir_(destDir),
velocity_(fabs(velocity)),
pyuserarg_(userarg),
pController_(pController)
{
	updatableName = "RotatorHandler";

	Py_INCREF(userarg);

	static_cast<TurnController*>(pController.get())->pRotatorHandler(this);
	Cellapp::getSingleton().addUpdatable(this);
}

//-------------------------------------------------------------------------------------
RotatorHandler::RotatorHandler() :
destDir_(0.f,0.f,0.f),
velocity_(0.f),
pyuserarg_(NULL),
pController_(KBEShared_ptr<Controller>())
{
	updatableName = "RotatorHandler";
}

//-------------------------------------------------------------------------------------
RotatorHandler::~RotatorHandler()
{
	if (pyuserarg_ != NULL)
	{
		Py_DECREF(pyuserarg_);
	}
}

//-------------------------------------------------------------------------------------
void RotatorHandler::addToStream(Ouroboros::MemoryStream& s)
{
	s << destDir_.dir.x << destDir_.dir.y << destDir_.dir.z << velocity_;
	s.appendBlob(script::Pickler::pickle(pyuserarg_));
}

//-------------------------------------------------------------------------------------
void RotatorHandler::createFromStream(Ouroboros::MemoryStream& s)
{
	s >> destDir_.dir.x >> destDir_.dir.y >> destDir_.dir.z >> velocity_;
	
	std::string val = "";
	s.readBlob(val);
	pyuserarg_ = script::Pickler::unpickle(val);
}

//-------------------------------------------------------------------------------------
bool RotatorHandler::requestTurnOver()
{
	if (pController_)
	{
		if (pController_->pEntity())
			pController_->pEntity()->onTurn(pController_->id(), pyuserarg_);

		// If calling cancelController(id) in onTurn causes the Controller to destruct and cause pController_ to be NULL
		if (pController_)
			pController_->destroy();
	}

	return true;
}

//-------------------------------------------------------------------------------------
const Direction3D& RotatorHandler::destDir()
{
	return destDir_;
}

//-------------------------------------------------------------------------------------
bool RotatorHandler::update()
{
	if(pController_ == NULL)
	{
		delete this;
		return false;
	}
		
	Entity* pEntity = pController_->pEntity();
	Py_INCREF(pEntity);

	const Direction3D& dstDir = destDir();
	Direction3D currDir = pEntity->direction();

	// get the difference
	float deltaYaw = dstDir.yaw() - currDir.yaw();

	if (deltaYaw > OURO_PI)
				deltaYaw = (float)((double)deltaYaw - OURO_2PI/*Since our radians represent a range of -PI ~ PI, prevent overflow here.*/);
	else if (deltaYaw < -OURO_PI)
		deltaYaw = (float)((double)deltaYaw + OURO_2PI);

	if (fabs(deltaYaw) < velocity_)
	{
		deltaYaw = 0.f;
		currDir.yaw(dstDir.yaw());
	}
	else if (fabs(deltaYaw) > velocity_)
	{
		deltaYaw = KBEClamp(deltaYaw, -velocity_, velocity_);
		currDir.yaw(currDir.yaw() + deltaYaw);
	}

	if (currDir.yaw() > OURO_PI)
		currDir.yaw((float((double)currDir.yaw() - OURO_2PI)));
	else if (currDir.yaw() < -OURO_PI)
		currDir.yaw((float((double)currDir.yaw() + OURO_2PI)));

	// Set the new location and orientation of the entity
	if (pController_)
		pEntity->setPositionAndDirection(pEntity->position(), currDir);

	// return true if the destination is reached
	if (fabs(deltaYaw) < 0.0001f && requestTurnOver())
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

