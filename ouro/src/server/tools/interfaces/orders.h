// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_INTERFACES_ORDERS_H
#define OURO_INTERFACES_ORDERS_H

#include "common/common.h"
#include "common/memorystream.h"
#include "thread/threadtask.h"
#include "helper/debug_helper.h"
#include "network/address.h"
#include "network/endpoint.h"

namespace Ouroboros{ 

class Orders
{
public:
	enum State
	{
		STATE_FAILED = 0,
		STATE_SUCCESS = 1
	};
	
	Orders();
	virtual ~Orders();
	
	Network::Address address;
	std::string ordersID;
	CALLBACK_ID cbid;
	DBID dbid;
	COMPONENT_ID baseappID;
	COMPONENT_ID dbmgrID;
	State state;
	
	std::string postDatas;
	std::string getDatas;

	uint64 timeout;
};

class OrdersCharge : public Orders
{
public:
	OrdersCharge();
	virtual ~OrdersCharge();
};

}

#endif // OURO_INTERFACES_ORDERS_H
