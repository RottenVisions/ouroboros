// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com


#ifndef OURO_SPACE_ENTITY_H
#define OURO_SPACE_ENTITY_H

#include "entity.h"
#include "common/common.h"
#include "helper/debug_helper.h"

namespace Ouroboros{

class Space : public Entity
{
	/** Space Entity */
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
