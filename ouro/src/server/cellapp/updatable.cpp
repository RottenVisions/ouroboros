// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#include "updatable.h"	
namespace Ouroboros{	


//-------------------------------------------------------------------------------------
Updatable::Updatable():
removeIdx(-1),
updatableName("Updatable")
{
}

//-------------------------------------------------------------------------------------
Updatable::~Updatable()
{
	removeIdx = -1;
}

//-------------------------------------------------------------------------------------
}
