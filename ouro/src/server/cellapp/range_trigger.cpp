// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#include "range_trigger.h"
#include "coordinate_system.h"
#include "entity_coordinate_node.h"
#include "range_trigger_node.h"

#ifndef CODE_INLINE
#include "range_trigger.inl"
#endif

namespace Ouroboros{	

//-------------------------------------------------------------------------------------
RangeTrigger::RangeTrigger(CoordinateNode* origin, float xz, float y):
range_xz_(fabs(xz)),
range_y_(fabs(y)),
origin_(origin),
positiveBoundary_(NULL),
negativeBoundary_(NULL),
removing_(false)
{
}

//-------------------------------------------------------------------------------------
RangeTrigger::~RangeTrigger()
{
	uninstall();
}

//-------------------------------------------------------------------------------------
bool RangeTrigger::reinstall(CoordinateNode* pCoordinateNode)
{
	uninstall();
	origin_ = pCoordinateNode;
	return install();
}

//-------------------------------------------------------------------------------------
bool RangeTrigger::install()
{
	if(positiveBoundary_ == NULL)
		positiveBoundary_ = new RangeTriggerNode(this, 0, 0, true);
	else
		positiveBoundary_->range(0.0f, 0.0f);

	if(negativeBoundary_ == NULL)
		negativeBoundary_ = new RangeTriggerNode(this, 0, 0, false);
	else
		negativeBoundary_->range(0.0f, 0.0f);

	positiveBoundary_->addFlags(COORDINATE_NODE_FLAG_INSTALLING);
	negativeBoundary_->addFlags(COORDINATE_NODE_FLAG_INSTALLING);

	origin_->pCoordinateSystem()->insert(positiveBoundary_);
	origin_->pCoordinateSystem()->insert(negativeBoundary_);
	
	/*
	Note: Here you must first install negativeBoundary_ and then install positiveBoundary_. If you change the order, it will cause a View BUG. For example, when an entity enterView is triggered, the entity entering the View is destroyed.
	At this time, when the entity is destroyed, the View event is not triggered, and the View event that is not triggered causes the destroyed entity referenced in the View list of other entities to be an invalid pointer.

	The reasons are as follows:
	Since the priority is always installed in positiveBoundary_, and the boundary causes another entity to enter the View during the installation process, then it may be destroyed in the process, and another boundary negativeBoundary_ is not installed yet.
	When the node is deleted, the node's xx is set to -FLT_MAX, which causes it to leave in the direction of negativeBoundary_, so the positiveBoundary_ cannot check the boundary and will not trigger the View leaving event.
	*/
	negativeBoundary_->old_xx(-FLT_MAX);
	negativeBoundary_->old_yy(-FLT_MAX);
	negativeBoundary_->old_zz(-FLT_MAX);
	negativeBoundary_->range(-range_xz_, -range_y_);
	negativeBoundary_->old_range(-range_xz_, -range_y_);
	negativeBoundary_->update();

	// update may cause the entity to be destroyed indirectly causing it to be reset. In this case, the installation should fail.
	if (!negativeBoundary_)
		return false;

	negativeBoundary_->removeFlags(COORDINATE_NODE_FLAG_INSTALLING);

	positiveBoundary_->old_xx(FLT_MAX);
	positiveBoundary_->old_yy(FLT_MAX);
	positiveBoundary_->old_zz(FLT_MAX);

	positiveBoundary_->range(range_xz_, range_y_);
	positiveBoundary_->old_range(range_xz_, range_y_);
	positiveBoundary_->update();

	if (positiveBoundary_)
	{
		positiveBoundary_->removeFlags(COORDINATE_NODE_FLAG_INSTALLING);
		return true;
	}

	return false;
}

//-------------------------------------------------------------------------------------
bool RangeTrigger::uninstall()
{
	if (removing_)
		return false;

	removing_ = true;
	if(positiveBoundary_ && positiveBoundary_->pCoordinateSystem())
	{
		positiveBoundary_->pCoordinateSystem()->remove(positiveBoundary_);
		positiveBoundary_->onTriggerUninstall();
	}

	if(negativeBoundary_ && negativeBoundary_->pCoordinateSystem())
	{
		negativeBoundary_->pCoordinateSystem()->remove(negativeBoundary_);
		negativeBoundary_->onTriggerUninstall();
	}
	
	// There is no need to release the node here, and the release of the node is handed over to CoordinateSystem.
	positiveBoundary_ = NULL;
	negativeBoundary_ = NULL;
	removing_ = false;
	return true;
}

//-------------------------------------------------------------------------------------
void RangeTrigger::onNodePassX(RangeTriggerNode* pRangeTriggerNode, CoordinateNode* pNode, bool isfront)
{
	if(pNode == origin())
		return;

	bool wasInZ = pRangeTriggerNode->wasInZRange(pNode);
	bool isInZ = pRangeTriggerNode->isInZRange(pNode);

	// If the Z-axis condition changes, the Z-axis is judged again, and the priority is zyx, so that only one enter or leave can be guaranteed.
	if(wasInZ != isInZ)
		return;

	bool wasIn = false;
	bool isIn = false;

	// Must check other axes at the same time, if the node x axis is in range, theoretically the other axes are also in range
	if(CoordinateSystem::hasY)
	{
		bool wasInY = pRangeTriggerNode->wasInYRange(pNode);
		bool isInY = pRangeTriggerNode->isInYRange(pNode);

		if(wasInY != isInY)
			return;

		wasIn = pRangeTriggerNode->wasInXRange(pNode) && wasInY && wasInZ;
		isIn = pRangeTriggerNode->isInXRange(pNode) && isInY && isInZ;
	}
	else
	{
		wasIn = pRangeTriggerNode->wasInXRange(pNode) && wasInZ;
		isIn = pRangeTriggerNode->isInXRange(pNode) && isInZ;
	}

	// Ignore if the situation has not changed
	if(wasIn == isIn)
		return;

	if(isIn)
	{
		this->onEnter(pNode);
	}
	else
	{
		this->onLeave(pNode);
	}
}

//-------------------------------------------------------------------------------------
void RangeTrigger::onNodePassY(RangeTriggerNode* pRangeTriggerNode, CoordinateNode* pNode, bool isfront)
{
	if(pNode == origin() || !CoordinateSystem::hasY)
		return;

	bool wasInZ = pRangeTriggerNode->wasInZRange(pNode);
	bool isInZ = pRangeTriggerNode->isInZRange(pNode);

	// If the Z-axis condition changes, the Z-axis is judged again, and the priority is zyx, so that only one enter or leave can be guaranteed.
	if(wasInZ != isInZ)
		return;

	bool wasInY = pRangeTriggerNode->wasInYRange(pNode);
	bool isInY = pRangeTriggerNode->isInYRange(pNode);

	if(wasInY == isInY)
		return;

	// Must check other axes at the same time, if the node x axis is in range, theoretically the other axes are also in range
	bool wasIn = pRangeTriggerNode->wasInXRange(pNode) && wasInY && wasInZ;
	bool isIn = pRangeTriggerNode->isInXRange(pNode) && isInY && isInZ;

	if(wasIn == isIn)
		return;

	if(isIn)
	{
		this->onEnter(pNode);
	}
	else
	{
		this->onLeave(pNode);
	}
}

//-------------------------------------------------------------------------------------
void RangeTrigger::onNodePassZ(RangeTriggerNode* pRangeTriggerNode, CoordinateNode* pNode, bool isfront)
{
	if(pNode == origin())
		return;

	if(CoordinateSystem::hasY)
	{
		bool wasInZ = pRangeTriggerNode->wasInZRange(pNode);
		bool isInZ = pRangeTriggerNode->isInZRange(pNode);

		if(wasInZ == isInZ)
			return;

		bool wasIn = pRangeTriggerNode->wasInXRange(pNode) && 
			pRangeTriggerNode->wasInYRange(pNode) && 
			wasInZ;

		bool isIn = pRangeTriggerNode->isInXRange(pNode) && 
			pRangeTriggerNode->isInYRange(pNode) && 
			isInZ;

		if(wasIn == isIn)
			return;

		if(isIn)
		{
			this->onEnter(pNode);
		}
		else
		{
			this->onLeave(pNode);
		}
	}
	else
	{
		bool wasInZ = pRangeTriggerNode->wasInZRange(pNode);
		bool isInZ = pRangeTriggerNode->isInZRange(pNode);

		if(wasInZ == isInZ)
			return;

		bool wasIn = pRangeTriggerNode->wasInXRange(pNode) && wasInZ;
		bool isIn = pRangeTriggerNode->isInXRange(pNode) && isInZ;

		if(wasIn == isIn)
			return;

		if(isIn)
		{
			this->onEnter(pNode);
		}
		else
		{
			this->onLeave(pNode);
		}
	}
}

//-------------------------------------------------------------------------------------
void RangeTrigger::update(float xz, float y)
{
	float old_range_xz_ = range_xz_;
	float old_range_y_ = range_y_;

	range(xz, y);

	if (positiveBoundary_)
	{
		positiveBoundary_->range(range_xz_, range_y_);
		positiveBoundary_->old_range(old_range_xz_, old_range_y_);
		positiveBoundary_->update();
	}

	if (negativeBoundary_)
	{
		negativeBoundary_->range(-range_xz_, -range_y_);
		negativeBoundary_->old_range(-old_range_xz_, -old_range_y_);
		negativeBoundary_->update();
	}
}

//-------------------------------------------------------------------------------------
}
