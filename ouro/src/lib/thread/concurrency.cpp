// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com


#include "concurrency.h"

namespace Ouroboros
{

static void default_op() {}

void (*pMainThreadIdleStartCallback)() = &default_op;
void (*pMainThreadIdleEndCallback)() = &default_op;

}
