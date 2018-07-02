// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#include "space.h"
#include "spaces.h"
#include "helper/profile.h"	

namespace Ouroboros{	


//-------------------------------------------------------------------------------------
Spaces::Spaces():
spaces_()
{
}

//-------------------------------------------------------------------------------------
Spaces::~Spaces()
{
	spaces_.clear();
}

//-------------------------------------------------------------------------------------
void Spaces::updateSpaceData(SPACE_ID spaceID, const std::string& scriptModuleName, const std::string& geomappingPath, bool deleted)
{
	if (deleted)
	{
		spaces_.erase(spaceID);
		return;
	}

	Space& space = spaces_[spaceID];

	space.setSpaceID(spaceID);
	space.updateGeomappingPath(geomappingPath);
	space.updateScriptModuleName(scriptModuleName);
}

//-------------------------------------------------------------------------------------
}
