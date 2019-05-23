
#include "Property.h"
#include "KBDebug.h"

namespace Ouroboros
{

Property::Property():
	name(TEXT("")),
	pUtype(NULL),
	properUtype(0),
	properFlags(0),
	aliasID(-1),
	defaultValStr(TEXT("")),
	pDefaultVal(NULL)
{
}

Property::~Property()
{
	OURO_SAFE_RELEASE(pDefaultVal);
}

}