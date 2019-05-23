// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com
#ifndef OURO_NAVIGATION_H
#define OURO_NAVIGATION_H

#include "common/common.h"
#include "helper/debug_helper.h"
#include "common/smartpointer.h"
#include "common/singleton.h"
#include "math/math.h"
#include "navigation_handle.h"

namespace Ouroboros
{
/*
	µº∫Ω¿‡
*/
class Navigation : public Singleton<Navigation>
{
public:
	Navigation();
	virtual ~Navigation();
	
	void finalise();

	NavigationHandlePtr loadNavigation(std::string resPath, const std::map< int, std::string >& params);

	bool hasNavigation(std::string resPath);

	bool removeNavigation(std::string resPath);

	NavigationHandlePtr findNavigation(std::string resPath);

private:
	OUROUnordered_map<std::string, NavigationHandlePtr> navhandles_;
	Ouroboros::thread::ThreadMutex mutex_;
};

}
#endif
