/*
This source file is part of Ouroboros
For the latest info, see http://www.ouroboros.org/

Copyright (c) 2008-2018 Ouroboros.

Ouroboros is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Ouroboros is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.
 
You should have received a copy of the GNU Lesser General Public License
along with Ouroboros.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef OURO_SPACE_ENTITY_H
#define OURO_SPACE_ENTITY_H
	
#include "entity.h"
#include "common/common.h"
#include "helper/debug_helper.h"
	
namespace Ouroboros{

class Space : public Entity
{
		/** Subclassing populates some py operations into derived classes*/
	BASE_SCRIPT_HREADER(Space, Entity)

public:
	Space(ENTITY_ID id, const ScriptDefModule* pScriptModule);
	~Space();
	
protected:
};

}


#ifdef CODE_INLINE
#include "space.inl"
#endif

#endif // OURO_SPACE_ENTITY_H
