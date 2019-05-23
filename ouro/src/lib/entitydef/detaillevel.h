// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com


#ifndef OURO_DETAILLEVEL_H
#define OURO_DETAILLEVEL_H

#include "common/common.h"
#include "helper/debug_helper.h"
#include "pyscript/scriptobject.h"	

namespace Ouroboros{

/** entity Detail level type definition
	The default is 3 levels:
	 Near, medium, far
*/
struct DetailLevel
{
	struct Level
	{
		Level():radius(FLT_MAX), hyst(1.0f){};
		float radius;
		float hyst;

		bool inLevel(float dist)
		{
			if(radius >= dist)
				return true;

			return false;
		}
	};
	
	DetailLevel()
	{
	}

	~DetailLevel()
	{
	}

	Level level[3]; // near, medium, far
};

}


#endif // OURO_DETAILLEVEL_H

