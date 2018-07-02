// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_CELLAPP_INIT_PROGRESS_HANDLER_H
#define OURO_CELLAPP_INIT_PROGRESS_HANDLER_H

// common include
#include "helper/debug_helper.h"
#include "common/common.h"

namespace Ouroboros{

class InitProgressHandler : public Task
{
public:
	InitProgressHandler(Network::NetworkInterface & networkInterface);
	~InitProgressHandler();
	
	bool process();

private:
	Network::NetworkInterface & networkInterface_;
	int delayTicks_;
};


}

#endif // OURO_CELLAPP_INIT_PROGRESS_HANDLER_H
