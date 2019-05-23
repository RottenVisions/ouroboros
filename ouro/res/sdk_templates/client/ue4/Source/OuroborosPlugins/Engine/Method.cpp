
#include "Method.h"
#include "DataTypes.h"
#include "KBDebug.h"

namespace Ouroboros
{

Method::Method():
	name(TEXT("")),
	methodUtype(0),
	aliasID(-1),
	args()
{
}

Method::~Method()
{
}

}