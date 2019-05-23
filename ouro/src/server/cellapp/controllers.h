// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_CONTROLLERS_H
#define OURO_CONTROLLERS_H

#include "controller.h"	

namespace Ouroboros{

class Controllers
{
public:
	Controllers(ENTITY_ID entityID);
	~Controllers();

	bool add(KBEShared_ptr<Controller> pController);
	bool remove(KBEShared_ptr<Controller> pController);
	bool remove(uint32 id);
	
	void clear();

	uint32 freeID() { return ++lastid_; }

	typedef std::map<uint32, KBEShared_ptr< Controller > > CONTROLLERS_MAP;
	CONTROLLERS_MAP& objects() { return objects_; }

	void addToStream(Ouroboros::MemoryStream& s);
	void createFromStream(Ouroboros::MemoryStream& s);

private:
	CONTROLLERS_MAP objects_;

	uint32 lastid_;

	ENTITY_ID entityID_;
};

}
#endif // OURO_CONTROLLERS_H
