// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

/*
	IDAllocate(Distributor)
		Used to assign a unique id managed by this allocator. Use this allocator must ensure that an application can only use
		The same id allocator to get the id is unique.

		If it is an unsigned int type, the allocator will always allocate upwards, and will reach the maximum value of the type.
		From the beginning of the turn and from 0 on top of the cumulative allocation, it will look from the list, if the current ID to be allocated is not found in the list
		Then this id will be assigned.

		usage:
		IDAllocate<ENTITY_ID>* m_IDAllocPtr = new IDAllocate<ENTITY_ID>;
		// Assign an id
		m_IDAllocPtr->alloc()
		// Recycling an id
		m_IDAllocPtr->reclaim()

	IDServer(server)
		This is mainly to provide the allocation of entityID between the entire server group, he is mainly used by baseappmgr, each IDserver
		When requesting ID, the server will assign a unique ID segment to the client. Then the client can use this segment.
		All unique ids are generated and assigned freely.

		usage:
		IDServer<ENTITY_ID>* m_idServer = new IDServer<ENTITY_ID>(1, 400);
		// Get an id segment and transfer it to IDClient
		std::pair< unsigned int, unsigned int > idRange = m_idServer->allocRange();
		g_socketStreamIDClient->send(idRange.first, idRange.second);

	IDClient(Clients)
		This module cooperates with IDServer for id application and receiving. 

		usage:
		IDClient<ENTITY_ID>* m_idClient = new IDClient<ENTITY_ID>;
		// Add ID segment sent by IDServer
		m_idClient->onAddRange(id_begin, id_end);
		// Assign an id
		m_idClient->alloc()
*/
#ifndef OURO_IDALLOCATE_H
#define OURO_IDALLOCATE_H

#include "helper/debug_helper.h"
#include "common/common.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <queue>


namespace Ouroboros{

class ServerApp;

// Use an iteration number directly, zero if the number overflows the type size so use an unsigned type
// Applicable for temporary allocation of id, and return soon so that there is no ID conflict
template<typename T>
class IDAllocate
{
public:
	IDAllocate(): last_id_(0)
	{
	}

	virtual ~IDAllocate()
	{
	}

	/**
		Assign an id
	*/
	T alloc(void)
	{
		T t = ++last_id_;
		if(t == 0)
			t = ++last_id_;

		return t;
	}

	/**
		Recycling an id
	*/
	virtual void reclaim(T id)
	{
	}

	T lastID() const{ return last_id_; }
	void lastID(T v){ last_id_ = v; }

protected:
	// Last application ID
	T last_id_;
};

// The allocated id will be stored in the list and will be obtained from the next use.
template< typename T >
class IDAllocateFromList : public IDAllocate<T>
{
public:
	IDAllocateFromList(): IDAllocate<T>()
	{
	}

	~IDAllocateFromList()
	{
	}

	/**
		Assign an id
	*/
	T alloc(void)
	{
		if(id_list_.size() > 0)
		{
			T n = id_list_.front();
			id_list_.pop();
			return n;
		}

		T t = ++IDAllocate<T>::last_id_;
		if(t == 0)
			t = ++IDAllocate<T>::last_id_;

		return t;
	}

	/**
		Recycling an id
	*/
	void reclaim(T id)
	{
		id_list_.push(id);
	}

protected:
	// Id list, all IDs exist in this list
	typename std::queue< T > id_list_;
};


template< typename T >
class IDServer
{
public:
	IDServer(T id_begin, T range_step):
	last_id_range_begin_(id_begin),
	range_step_(range_step)
	{
	}

	~IDServer()
	{
	}

	/**
		Assign an id segment
	*/
	std::pair< T, T > allocRange(void)
	{
		INFO_MSG(fmt::format("IDServer::allocRange: {}-{}.\n",
			last_id_range_begin_, (last_id_range_begin_ + range_step_)));

		std::pair< T, T > p = std::make_pair(last_id_range_begin_, last_id_range_begin_ + range_step_);
		last_id_range_begin_ += range_step_;
		return p;
	}

	void set_range_step(T range_step) {
		range_step_ = range_step;
	}

protected:
	// The starting position of the last application ID segment
	T last_id_range_begin_;

	// One segment length of id segment
	T range_step_;
};

template< typename T >
class IDClient
{
public:
	IDClient():
	  last_id_range_begin_(0),
	last_id_range_end_(0),
	requested_idserver_alloc_(false)
	{
	}

	/**
		Destruction does not notify IDServer to recycle, please user to carry out maintenance in this area
	*/
	virtual ~IDClient()
	{
	}

	bool hasReqServerAlloc() const {
		return requested_idserver_alloc_;
	}

	void setReqServerAllocFlag(bool has){
		requested_idserver_alloc_ = has;
	}

	size_t size()
	{
		size_t nCount = last_id_range_end_ - last_id_range_begin_;
		if(nCount <= 0)
		{
			// See if there is a cached ID segment (will apply to the server for caching here when id is running low)
			if(id_list_.size() > 0)
			{
				std::pair< T, T > n = id_list_.front();
				last_id_range_begin_ = n.first;
				last_id_range_end_ = n.second;
				id_list_.pop();
				nCount = last_id_range_end_ - last_id_range_begin_;
			}
		}

		return nCount;
	}

	/**
		Check if the entityID is sufficient
		Note: The number of IDs used in a tick should not exceed ID_ENOUGH_LIMIT
	*/
	virtual void onAlloc(void) {
	};

	/**
		idserver One id segment assigned
	*/
	void onAddRange(T id_begin, T id_end)
	{
		INFO_MSG(fmt::format("IDClient::onAddRange: number of ids increased from {} to {}.\n", id_begin, id_end));
		if(size() <= 0)
		{
			last_id_range_begin_ = id_begin;
			last_id_range_end_ = id_end;
		}
		else
		{
			id_list_.push(std::make_pair(id_begin, id_end));
		}
	}

	/**
		Assign an id
	*/
	T alloc(void)
	{
		OURO_ASSERT(size() > 0 && "IDClient:: alloc:no usable of the id.\n");

		T id = last_id_range_begin_++;

		if(last_id_range_begin_ > last_id_range_end_)
		{
			// See if there is a cached ID segment (will apply to the server for caching here when id is running low)
			if(id_list_.size() > 0)
			{
				std::pair< T, T > n = id_list_.front();
				last_id_range_begin_ = n.first;
				last_id_range_end_ = n.second;
				id_list_.pop();
			}
			else
			{
				last_id_range_begin_ = last_id_range_end_ = 0;
			}
		}

		onAlloc();
		return id;
	}

	/**
		Recycling an id
	*/
	void onReclaim(T id)
	{
	}

protected:
	// Id list, all ID segments exist in this list
	typename std::queue< std::pair< T, T > > id_list_;

	// The starting position of the last application ID segment
	T last_id_range_begin_;
	T last_id_range_end_;

	// Whether ID service ID has been requested
	bool requested_idserver_alloc_;
};

class EntityIDClient : public IDClient<ENTITY_ID>
{
public:
	EntityIDClient();

	virtual ~EntityIDClient()
	{
	}

	virtual void onAlloc(void);

	void pApp(ServerApp* pApp){
		pApp_ = pApp;
	}

protected:
	ServerApp* pApp_;
};

}

#endif // OURO_IDALLOCATE_H
