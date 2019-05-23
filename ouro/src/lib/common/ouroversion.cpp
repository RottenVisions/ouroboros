// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#include "ouroversion.h"
#include "helper/debug_helper.h"
namespace Ouroboros{
namespace KBEVersion
{
std::string g_versionString;

/**
	½Å±¾²ã°æ±¾
*/
std::string g_scriptVersion = "0.0.0";

const std::string & versionString()
{
	if(g_versionString.size() == 0)
	{
		char buf[MAX_BUF];
		ouro_snprintf(buf, MAX_BUF, "%d.%d.%d", OURO_VERSION_MAJOR, OURO_VERSION_MINOR, OURO_VERSION_PATCH);
		g_versionString = buf;
	}

	return g_versionString;
}

void setScriptVersion(const std::string& ver)
{
	g_scriptVersion = ver;
}

const std::string & scriptVersionString()
{
	return g_scriptVersion;
}


}
} 


