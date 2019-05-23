// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_SYNC_ENTITY_STREAM_TEMPLATE_HANDLER_H
#define OURO_SYNC_ENTITY_STREAM_TEMPLATE_HANDLER_H

// common include
#include "helper/debug_helper.h"
#include "common/common.h"
// #define NDEBUG
// windows include	
#if OURO_PLATFORM == PLATFORM_WIN32
#else
// linux include
#endif

namespace Ouroboros{

class SyncEntityStreamTemplateHandler : public Task
{
public:
	SyncEntityStreamTemplateHandler(Network::NetworkInterface & networkInterface);
	~SyncEntityStreamTemplateHandler();
	
	bool process();
private:
	Network::NetworkInterface & networkInterface_;

};


}

#endif // OURO_SYNC_ENTITY_STREAM_TEMPLATE_HANDLER_H
