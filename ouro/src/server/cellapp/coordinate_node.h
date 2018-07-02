// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_COORDINATE_NODE_H
#define OURO_COORDINATE_NODE_H

#include "helper/debug_helper.h"
#include "common/common.h"

namespace Ouroboros{

#define COORDINATE_NODE_FLAG_UNKNOWN				0x00000000
#define COORDINATE_NODE_FLAG_ENTITY					0x00000001		// An Entity node
#define COORDINATE_NODE_FLAG_TRIGGER				0x00000002		// A trigger node
#define COORDINATE_NODE_FLAG_HIDE					0x00000004		// Hidden nodes (other nodes are not visible)
#define COORDINATE_NODE_FLAG_REMOVING				0x00000008		// Delete the node
#define COORDINATE_NODE_FLAG_REMOVED				0x00000010		// Delete node
#define COORDINATE_NODE_FLAG_PENDING				0x00000020		// This type of node is in the update operation.
#define COORDINATE_NODE_FLAG_ENTITY_NODE_UPDATING	0x00000040		// The entity node is performing an update operation
#define COORDINATE_NODE_FLAG_INSTALLING				0x00000080		// Node is installing operation
#define COORDINATE_NODE_FLAG_POSITIVE_BOUNDARY		0x00000100		// Node is the positive boundary of the trigger
#define COORDINATE_NODE_FLAG_NEGATIVE_BOUNDARY		0x00000200		// Node is the negative boundary of the trigger

#define COORDINATE_NODE_FLAG_HIDE_OR_REMOVED		(COORDINATE_NODE_FLAG_REMOVED | COORDINATE_NODE_FLAG_HIDE)

class CoordinateSystem;
class CoordinateNode
{
public:
	CoordinateNode(CoordinateSystem* pCoordinateSystem = NULL);
	virtual ~CoordinateNode();

	INLINE void flags(uint32 v);
	INLINE uint32 flags() const;
	INLINE void addFlags(uint32 v);
	INLINE void removeFlags(uint32 v);
	INLINE bool hasFlags(uint32 v) const;

	/**
		(The coordinates of the node itself)
		x && z is implemented by different applications(Get from different places)
	*/
	virtual float x() const { return x_; }
	virtual float y() const { return y_; }
	virtual float z() const { return z_; }

	virtual void x(float v) { x_ = v; }
	virtual void y(float v) { y_ = v; }
	virtual void z(float v) { z_ = v; }

	/**
		(Extended coordinates)
		x && z is implemented by different applications(Get from different places)
	*/
	virtual float xx() const { return 0.f; }
	virtual float yy() const { return 0.f; }
	virtual float zz() const { return 0.f; }

	void old_xx(float v) { old_xx_ = v; }
	void old_yy(float v) { old_yy_ = v; }
	void old_zz(float v) { old_zz_ = v; }

	float old_xx() const { return old_xx_; }
	float old_yy() const { return old_yy_; }
	float old_zz() const { return old_zz_; }

	int8 weight() const { return weight_; }

	virtual void resetOld() {
		old_xx_ = xx();
		old_yy_ = yy();
		old_zz_ = zz();
	}

	std::string c_str();

	void debugX();
	void debugY();
	void debugZ();

	INLINE void pCoordinateSystem(CoordinateSystem* p);
	INLINE CoordinateSystem* pCoordinateSystem() const;

	INLINE bool isDestroying() const {
		return hasFlags(COORDINATE_NODE_FLAG_REMOVING);
	}

	INLINE bool isDestroyed() const {
		return hasFlags(COORDINATE_NODE_FLAG_REMOVED);
	}

	/**
		Get the front and back pointers of the linked list
	*/
	INLINE CoordinateNode* pPrevX() const;
	INLINE CoordinateNode* pNextX() const;
	INLINE CoordinateNode* pPrevY() const;
	INLINE CoordinateNode* pNextY() const;
	INLINE CoordinateNode* pPrevZ() const;
	INLINE CoordinateNode* pNextZ() const;

	/**
		Set the front and back pointers of the linked list
	*/
	INLINE void pPrevX(CoordinateNode* pNode);
	INLINE void pNextX(CoordinateNode* pNode);
	INLINE void pPrevY(CoordinateNode* pNode);
	INLINE void pNextY(CoordinateNode* pNode);
	INLINE void pPrevZ(CoordinateNode* pNode);
	INLINE void pNextZ(CoordinateNode* pNode);

	/**
		A node changes through this node
		@isfront: Move forward or backward
	*/
	virtual void onNodePassX(CoordinateNode* pNode, bool isfront);
	virtual void onNodePassY(CoordinateNode* pNode, bool isfront);
	virtual void onNodePassZ(CoordinateNode* pNode, bool isfront);

	virtual void onRemove();

	/**
		Parent delete
	*/
	virtual void onParentRemove(CoordinateNode* pParentNode) {
	}

	/**
		When the node changes, you need to update it in the list
		Related locations and other information
	*/
	virtual void update();

#ifdef _DEBUG
	void descr(const std::string& str) { descr_ = str; }
	virtual const char* descr() { return descr_.c_str(); }
#else
	void descr(const std::string& str){}
	virtual const char* descr(){ return ""; }
#endif

protected:
	// Front and back of the linked list
	CoordinateNode* pPrevX_;
	CoordinateNode* pNextX_;
	CoordinateNode* pPrevY_;
	CoordinateNode* pNextY_;
	CoordinateNode* pPrevZ_;
	CoordinateNode* pNextZ_;

	CoordinateSystem* pCoordinateSystem_;

	float x_, y_, z_;
	float old_xx_, old_yy_, old_zz_;

	int8 weight_;

#ifdef _DEBUG
	std::string descr_;
#endif

	uint32 flags_;
};

}

#ifdef CODE_INLINE
#include "coordinate_node.inl"
#endif
#endif
