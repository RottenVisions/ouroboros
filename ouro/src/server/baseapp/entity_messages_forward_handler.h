// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_BASE_ENTITY_MESSAGES_FORWARD_HANDLER_H
#define OURO_BASE_ENTITY_MESSAGES_FORWARD_HANDLER_H

#include "helper/debug_helper.h"
#include "common/common.h"

namespace Ouroboros{

class Entity;
class EntityMessagesForwardCellappHandler : public Task
{
public:
	EntityMessagesForwardCellappHandler(Entity* pEntity);
	~EntityMessagesForwardCellappHandler();
	
	bool process();

	void pushMessages(Network::Bundle* pBundle);

	void startForward();
	void stopForward(){ startForward_ = false; }

	bool isStop() const{ return !startForward_; }

private:
	Entity* pEntity_;
	bool completed_;
	std::vector<Network::Bundle*> bufferedSendToCellappMessages_;
	bool startForward_;
	uint64 createTime_;
};

class BaseMessagesForwardClientHandler : public Task
{
public:
	BaseMessagesForwardClientHandler(Entity* pEntity, COMPONENT_ID cellappID);
	~BaseMessagesForwardClientHandler();
	
	bool process();

	void pushMessages(Network::Bundle* pBundle);

	void startForward();
	void stopForward(){ startForward_ = false; }

	bool isStop() const{ return !startForward_; }
	
	COMPONENT_ID cellappID() const {
		return cellappID_;
	}

	void cellappID(COMPONENT_ID cid) {
		cellappID_ = cid;
	}

private:
	Entity* pEntity_;
	bool completed_;
	std::vector<Network::Bundle*> bufferedSendToClientMessages_;
	bool startForward_;
	COMPONENT_ID cellappID_;
	uint64 createTime_;
};

}

#endif // OURO_BASE_ENTITY_MESSAGES_FORWARD_HANDLER_H
