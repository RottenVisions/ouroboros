// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_ENTITY_COORDINATE_NODE_H
#define OURO_ENTITY_COORDINATE_NODE_H

#include "coordinate_node.h"
#include "math/math.h"

namespace Ouroboros{

class Entity;

class EntityCoordinateNode : public CoordinateNode
{
public:
	EntityCoordinateNode(Entity* pEntity);
	virtual ~EntityCoordinateNode();

	/**
		(extended coordinates)
		x && z is implemented by different applications (from different locations)
	*/
	virtual float xx() const;
	virtual float yy() const;
	virtual float zz() const;

	virtual void update();

	Entity* pEntity() const { return pEntity_; }
	void pEntity(Entity* pEntity) { pEntity_ = pEntity; }

	bool addWatcherNode(CoordinateNode* pNode);
	void onAddWatcherNode(CoordinateNode* pNode);
	
	bool delWatcherNode(CoordinateNode* pNode);

	static void entitiesInRange(std::vector<Entity*>& foundEntities, CoordinateNode* rootNode, 
		const Position3D& orginPos, float radius, int entityUType = -1);

	virtual void onRemove();

protected:
	void clearDelWatcherNodes();

protected:
	Entity* pEntity_;

	typedef std::vector<CoordinateNode*> WATCHER_NODES;
	WATCHER_NODES watcherNodes_;
	int delWatcherNodeNum_;

	int entityNodeUpdating_;
};

}

#endif
