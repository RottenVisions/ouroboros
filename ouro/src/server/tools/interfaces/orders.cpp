// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com
#include "network/bundle.h"
#include "interfaces.h"
#include "orders.h"


namespace Ouroboros{

//-------------------------------------------------------------------------------------
Orders::Orders():
address(),
ordersID(),
cbid(0),
dbid(0),
baseappID(UNKNOWN_COMPONENT_TYPE),
dbmgrID(UNKNOWN_COMPONENT_TYPE),
state(Orders::STATE_FAILED),
postDatas(),
getDatas(),
timeout(0)
{
}

//-------------------------------------------------------------------------------------
Orders::~Orders()
{
}

//-------------------------------------------------------------------------------------
OrdersCharge::OrdersCharge()
{
}

//-------------------------------------------------------------------------------------
OrdersCharge::~OrdersCharge()
{
	//INFO_MSG(fmt::format("OrdersCharge::~OrdersCharge({0:p}): orders={1}\n", 
	//	(void*)this, ordersID));
}

//-------------------------------------------------------------------------------------
}
