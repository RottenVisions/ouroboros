// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_FORWARD_MESSAGEBUFFER_H
#define OURO_FORWARD_MESSAGEBUFFER_H

#include "common/common.h"
#include "common/tasks.h"
#include "common/singleton.h"
#include "helper/debug_helper.h"
#include "server/components.h"
#include "network/bundle.h"

namespace Ouroboros { 
namespace Network
{
class Bundle;
class NetworkInterface;
class EventDispatcher;
}

/*
	if you don't find any cellapp or baseapp module on the app, cache some messages.
	Wait for a new cellapp or baseapp to join and start forwarding instructions.
*/


/*
	When a message is successfully forwarded, the handler is called to handle the remaining things.
	Need to rewrite process
*/
class ForwardMessageOverHandler
{
public:
	virtual void process() = 0;
	virtual ~ForwardMessageOverHandler(){}
};

class ForwardItem
{
public:
	ForwardItem():
		pBundle(NULL),
		pHandler(NULL)
	{
	}

	virtual ~ForwardItem()
	{
	}

	Network::Bundle* pBundle;
	ForwardMessageOverHandler* pHandler;

	virtual bool isOK(){
		return true;
	}
};

/*
	Forward cached messages to the specified component
*/
class ForwardComponent_MessageBuffer : public Task, 
						public Singleton<ForwardComponent_MessageBuffer>
{
public:
	ForwardComponent_MessageBuffer(Network::NetworkInterface & networkInterface);
	virtual ~ForwardComponent_MessageBuffer();

	Network::EventDispatcher & dispatcher();

	void push(COMPONENT_ID componentID, ForwardItem* pHandler);
	
	bool process();

	virtual void clear();
	
private:
	Network::NetworkInterface & networkInterface_;
	bool start_;
	
	typedef std::vector<ForwardItem*> MSGMAP_ITEM;
	typedef std::map<COMPONENT_ID, MSGMAP_ITEM> MSGMAP;
	MSGMAP pMap_;

};

/*
	Forward cached messages to any component of the same type
*/
class ForwardAnywhere_MessageBuffer : public Task, 
						public Singleton<ForwardAnywhere_MessageBuffer>
{
public:
	ForwardAnywhere_MessageBuffer(Network::NetworkInterface & networkInterface, COMPONENT_TYPE forwardComponentType);
	virtual ~ForwardAnywhere_MessageBuffer();

	Network::EventDispatcher & dispatcher();

	void push(ForwardItem* pHandler);
	
	bool process();

	virtual void clear();
	
private:
	Network::NetworkInterface & networkInterface_;
	COMPONENT_TYPE forwardComponentType_;
	bool start_;
	
	std::vector<ForwardItem*> pBundles_;

};

}

#endif // OURO_FORWARD_MESSAGEBUFFER_H
