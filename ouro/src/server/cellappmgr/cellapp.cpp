// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com
#include "cellapp.h"

namespace Ouroboros{

//-------------------------------------------------------------------------------------
Cellapp::Cellapp():
numEntities_(0),
load_(0.f),
isDestroyed_(false),
watchers_(),
spaces_(),
initProgress_(0.f),
flags_(APP_FLAGS_NONE),
globalOrderID_(0),
groupOrderID_(0)
{
}

//-------------------------------------------------------------------------------------
Cellapp::~Cellapp()
{
}


//-------------------------------------------------------------------------------------
}
