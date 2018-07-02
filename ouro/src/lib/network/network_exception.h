// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_NETWORKNUB_EXCEPTION_H
#define OURO_NETWORKNUB_EXCEPTION_H

#include "common/common.h"
#include "common/timer.h"
#include "helper/debug_helper.h"
#include "network/common.h"
#include "network/address.h"

namespace Ouroboros { 
namespace Network
{
class NetworkException
{
public:
	NetworkException(Reason reason, const Address & addr = Address::NONE);
	virtual ~NetworkException() {};
	Reason reason() const;
	bool getAddress(Address & addr) const;

private:
	Reason		reason_;
	Address address_;
};

inline NetworkException::NetworkException(Reason reason, const Address & addr) :
	reason_(reason),
	address_(addr)
{}


inline Reason NetworkException::reason() const
{
	return reason_;
}


inline bool NetworkException::getAddress(Address & addr) const
{
	addr = address_;
	return address_ != Address::NONE;
}

}
}

#endif // OURO_NETWORKNUB_EXCEPTION_H
