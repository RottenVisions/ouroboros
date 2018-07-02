// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_SCRIPT_MATH_H
#define OURO_SCRIPT_MATH_H

#include "common/common.h"
#include "scriptobject.h"
#include "vector2.h"
#include "vector3.h"
#include "vector4.h"

namespace Ouroboros{ namespace script{ namespace math {

/** Install math module */
bool installModule(const char* moduleName);
bool uninstallModule();

}
}
}

#endif // OURO_SCRIPT_MATH_H
