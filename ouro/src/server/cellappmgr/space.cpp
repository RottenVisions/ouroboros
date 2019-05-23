// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#include "space.h"
#include "helper/profile.h"	

namespace Ouroboros{	


//-------------------------------------------------------------------------------------
Space::Space() :
spaceID_(0),
cells_(),
geomappingPath_(),
scriptModuleName_()
{
}

//-------------------------------------------------------------------------------------
Space::~Space()
{
}

//-------------------------------------------------------------------------------------
void Space::updateGeomappingPath(const std::string& geomappingPath)
{
	geomappingPath_ = geomappingPath;
}

//-------------------------------------------------------------------------------------
void Space::updateScriptModuleName(const std::string& scriptModuleName)
{
	scriptModuleName_ = scriptModuleName;
}

//-------------------------------------------------------------------------------------
}
