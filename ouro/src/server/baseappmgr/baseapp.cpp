// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com
#include "baseapp.h"

namespace Ouroboros{

//-------------------------------------------------------------------------------------
Baseapp::Baseapp():
numEntitys_(0),
numProxices_(0),
load_(0.f),
isDestroyed_(false),
watchers_(),
initProgress_(0.f),
flags_(APP_FLAGS_NONE)
{
}

//-------------------------------------------------------------------------------------
Baseapp::~Baseapp()
{
}


//-------------------------------------------------------------------------------------
}
