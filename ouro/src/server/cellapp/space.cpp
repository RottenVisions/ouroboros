// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#include "cellapp.h"
#include "space.h"
#include "profile.h"

#ifndef CODE_INLINE
#include "space.inl"
#endif

namespace Ouroboros{

SCRIPT_METHOD_DECLARE_BEGIN(Space)
SCRIPT_METHOD_DECLARE_END()

SCRIPT_MEMBER_DECLARE_BEGIN(Space)
SCRIPT_MEMBER_DECLARE_END()

SCRIPT_GETSET_DECLARE_BEGIN(Space)
SCRIPT_GETSET_DECLARE_END()
BASE_SCRIPT_INIT(Space, 0, 0, 0, 0, 0)

//-------------------------------------------------------------------------------------
Space::Space(ENTITY_ID id, const ScriptDefModule* pScriptModule):
Entity(id, pScriptModule, getScriptType(), true)
{

}

//-------------------------------------------------------------------------------------
Space::~Space()
{

}

//-------------------------------------------------------------------------------------
}
