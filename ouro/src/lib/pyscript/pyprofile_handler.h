// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_PYPROFILE_HANDLER_H
#define OURO_PYPROFILE_HANDLER_H

#include "helper/profile_handler.h"

namespace Ouroboros { 

class PyProfileHandler : public ProfileHandler
{
public:
	PyProfileHandler(Network::NetworkInterface & networkInterface, uint32 timinglen, 
		std::string name, const Network::Address& addr);
	virtual ~PyProfileHandler();
	
	void timeout();
	void sendStream(MemoryStream* s);
};

class PyTickProfileHandler : public Task,
                             public ProfileHandler
{
public:
	PyTickProfileHandler(Network::NetworkInterface & networkInterface, uint32 timinglen,
		std::string name, const Network::Address& addr);
	virtual ~PyTickProfileHandler();

	virtual void timeout();
	virtual bool process();
	virtual void sendStream(MemoryStream* s);
};


}

#endif // OURO_PYPROFILE_HANDLER_H
