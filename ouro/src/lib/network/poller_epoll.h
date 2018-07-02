// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_EPOLL_POLLER_H
#define OURO_EPOLL_POLLER_H

#include "event_poller.h"

#if OURO_PLATFORM != PLATFORM_WIN32
#define HAS_EPOLL
#endif

namespace Ouroboros { 
namespace Network
{

#ifdef HAS_EPOLL
class EpollPoller : public EventPoller
{
public:
	EpollPoller(int expectedSize = 10);
	virtual ~EpollPoller();

	int getFileDescriptor() const { return epfd_; }

protected:
	virtual bool doRegisterForRead(int fd)
		{ return this->doRegister(fd, true, true); }

	virtual bool doRegisterForWrite(int fd)
		{ return this->doRegister(fd, false, true); }

	virtual bool doDeregisterForRead(int fd)
		{ return this->doRegister(fd, true, false); }

	virtual bool doDeregisterForWrite(int fd)
		{ return this->doRegister(fd, false, false); }

	virtual int processPendingEvents(double maxWait);

	bool doRegister(int fd, bool isRead, bool isRegister);

private:

	int epfd_;
};
#endif // HAS_EPOLL

}
}
#endif // OURO_EPOLL_POLLER_H
