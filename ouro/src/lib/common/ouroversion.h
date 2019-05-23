// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef KBEVERSION_H
#define KBEVERSION_H

#include "common/platform.h"

namespace Ouroboros{
	
#define OURO_VERSION_MAJOR 2
#define OURO_VERSION_MINOR 5
#define OURO_VERSION_PATCH 0


namespace KBEVersion
{
	const std::string & versionString();
	void setScriptVersion(const std::string& ver);
	const std::string & scriptVersionString();
}

}
#endif // KBEVERSION_H
