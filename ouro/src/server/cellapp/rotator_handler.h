// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_RotatorHandler_H
#define OURO_RotatorHandler_H

#include "controller.h"
#include "updatable.h"
#include "pyscript/scriptobject.h"	
#include "math/math.h"	


namespace Ouroboros{

class RotatorHandler : public Updatable
{
public:
	RotatorHandler(OUROShared_ptr<Controller> pController, const Direction3D& destDir, float velocity, PyObject* userarg);

	RotatorHandler();
	virtual ~RotatorHandler();

	void addToStream(Ouroboros::MemoryStream& s);
	void createFromStream(Ouroboros::MemoryStream& s);

	virtual bool update();

	virtual const Direction3D& destDir();
	virtual bool requestTurnOver();
	void pController(OUROShared_ptr<Controller> pController){ pController_ = pController; }

	float velocity() const {
		return velocity_;
	}

	void velocity(float v) {
		velocity_ = v;
	}

protected:
	Direction3D destDir_;
	float velocity_;
	PyObject* pyuserarg_;
	OUROShared_ptr<Controller> pController_;
};
 
}
#endif // OURO_MOVETOENTITYHANDLER_H

