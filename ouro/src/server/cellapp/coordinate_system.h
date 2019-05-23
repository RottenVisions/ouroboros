// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_COORDINATE_SYSTEM_H
#define OURO_COORDINATE_SYSTEM_H

#include "helper/debug_helper.h"
#include "common/common.h"	

//#define DEBUG_COORDINATE_SYSTEM

namespace Ouroboros{

class CoordinateNode;

class CoordinateSystem
{
public:
	CoordinateSystem();
	~CoordinateSystem();

	/**
		Insert a node into the list
	*/
	bool insert(CoordinateNode* pNode);

	/**
		Remove the node from the list
	*/
	bool remove(CoordinateNode* pNode);
	bool removeReal(CoordinateNode* pNode);
	void removeDelNodes();
	void releaseNodes();

	/**
		When a node changes, it needs to be updated in the list.
		Related location and other information
	*/
	void update(CoordinateNode* pNode);

	/**
		Mobile node
	*/
	void moveNodeX(CoordinateNode* pNode, float px, CoordinateNode* pCurrNode);
	void moveNodeY(CoordinateNode* pNode, float py, CoordinateNode* pCurrNode);
	void moveNodeZ(CoordinateNode* pNode, float pz, CoordinateNode* pCurrNode);

	INLINE CoordinateNode * pFirstXNode() const;
	INLINE CoordinateNode * pFirstYNode() const;
	INLINE CoordinateNode * pFirstZNode() const;

	INLINE bool isEmpty() const;

	INLINE uint32 size() const;

	static bool hasY;

	INLINE void incUpdating();
	INLINE void decUpdating();

private:
	uint32 size_;

	// The first and last pointers of the linked list
	CoordinateNode* first_x_coordinateNode_;
	CoordinateNode* first_y_coordinateNode_;
	CoordinateNode* first_z_coordinateNode_;

	std::list<CoordinateNode*> dels_;
	size_t dels_count_;

	int updating_;

	std::list<CoordinateNode*> releases_;
};

}

#ifdef CODE_INLINE
#include "coordinate_system.inl"
#endif
#endif
