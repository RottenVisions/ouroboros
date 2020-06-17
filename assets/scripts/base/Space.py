# -*- coding: utf-8 -*-
import Ouroboros
import random
import ServerConstantsDefine
import copy
import math
from OURODebug import *
from interfaces.GameObject import GameObject
import data_entities
import data_spaces
import data_spaces_spawns
import xml.etree.ElementTree as etree


class Space(Ouroboros.Entity, GameObject):
	"""
	An entity that can manipulate the real space on the cellapp
	Note: It is an entity, not a real space. The real space exists in the memory of the cellapp, through which the entity is associated and manipulates the space.
	"""

	def __init__(self):
		Ouroboros.Entity.__init__(self)
		GameObject.__init__(self)
		self.createCellEntityInNewSpace(None)

		self.spaceUTypeB = self.cellData["spaceUType"]

		self.spaceResName = data_spaces.data.get(self.spaceUTypeB)['resPath']

		# The total number of entities created on this map
		self.tmpCreateEntityDatas = copy.deepcopy(data_spaces_spawns.data.get(self.spaceUTypeB, []))

		self.avatars = {}
		self.createSpawnPointDatas()

	def createSpawnPointDatas(self):
		"""
		"""
		res = r"scripts\data\spawn_points\%s_spawn_points.xml" % (self.spaceResName.replace("\\", "/").split("/")[-1])

		if (len(self.spaceResName) == 0 or not Ouroboros.hasRes(res)):
			DEBUG_MSG("No Spawn Point Data found in: %s" % (res))
			return

		res = Ouroboros.getResFullPath(res)

		tree = etree.parse(res)
		root = tree.getroot()

		DEBUG_MSG("Space::createSpawnPointData: %s" % (res))

		for child in root:
			positionNode = child[0][0]
			directionNode = child[0][1]
			scaleNode = child[0][2]

			scale = int(((float(scaleNode[0].text) + float(scaleNode[1].text) + float(scaleNode[2].text)) / 3.0) * 10)
			position = (float(positionNode[0].text), float(positionNode[1].text), float(positionNode[2].text))
			direction = [float(directionNode[0].text) / 360 * (math.pi * 2),
						 float(directionNode[1].text) / 360 * (math.pi * 2),
						 float(directionNode[2].text) / 360 * (math.pi * 2)]

			if direction[0] - math.pi > 0.0:
				direction[0] -= math.pi * 2
			if direction[1] - math.pi > 0.0:
				direction[1] -= math.pi * 2
			if direction[2] - math.pi > 0.0:
				direction[2] -= math.pi * 2

			self.tmpCreateEntityDatas.append([int(child.attrib['name']), \
											  position, \
											  direction, \
											  scale, \
											  ])

	def spawnOnTimer(self, tid):
		"""
		Birth monster
		"""
		DEBUG_MSG("spawn timer %s" % (len(self.tmpCreateEntityDatas)))
		if len(self.tmpCreateEntityDatas) <= 0:
			self.delTimer(tid)
			return

		data = self.tmpCreateEntityDatas.pop(0)

		if data is None:
			ERROR_MSG("Space::onTimer: spawn %i is error!" % data[0])

		Ouroboros.createEntityAnywhere("SpawnPoint",
									  {"spawnEntityNO": data[0], \
									   "position"		: data[1], \
									   "direction"			: data[2], \
									   "modelScale"		: data[3], \
									   "createToCell"		: self.cell})

		DEBUG_MSG("Created [%s]" % (data[0]))

	def loginToSpace(self, avatarEntityCall, context):
		"""
		Defined method.
		A player requests to log in to this space
		"""
		avatarEntityCall.createCell(self.cell)
		self.onEnter(avatarEntityCall)

	def logoutSpace(self, entityID):
		"""
		Defined method.
		A player requests to log out of this space
		"""
		self.onLeave(entityID)

	def teleportSpace(self, entityCall, position, direction, context):
		"""
		Defined method.
		Request to enter a space
		"""
		entityCall.cell.onTeleportSpaceCB(self.cell, self.spaceUTypeB, position, direction)

	def onTimer(self, tid, userArg):
		"""
		Ouroboros method.
		Engine callback timer trigger
		"""
		# DEBUG_MSG("%s::onTimer: %i, tid:%i, arg:%i" % (self.getScriptName(), self.id, tid, userArg))
		if ServerConstantsDefine.TIMER_TYPE_SPACE_SPAWN_TICK == userArg:
			self.spawnOnTimer(tid)

		GameObject.onTimer(self, tid, userArg)

	def onEnter(self, entityCall):
		"""
		Defined method.
		Entering the scene
		"""
		self.avatars[entityCall.id] = entityCall

		if self.cell is not None:
			self.cell.onEnter(entityCall)

	def onLeave(self, entityID):
		"""
		Defined method.
		Leave the scene
		"""
		if entityID in self.avatars:
			del self.avatars[entityID]

		if self.cell is not None:
			self.cell.onLeave(entityID)

	def onLoseCell(self):
		"""
		Ouroboros method.
		The entity part of the cell is lost
		"""
		Ouroboros.globalData["Spaces"].onSpaceLoseCell(self.spaceUTypeB, self.spaceKey)
		GameObject.onLoseCell(self)

	def onGetCell(self):
		"""
		Ouroboros method.
		The cell part of the entity was created successfully.
		"""
		DEBUG_MSG("Space::onGetCell: %i" % self.id)
		self.addTimer(0.1, 0.1, ServerConstantsDefine.TIMER_TYPE_SPACE_SPAWN_TICK)
		Ouroboros.globalData["Spaces"].onSpaceGetCell(self.spaceUTypeB, self, self.spaceKey)
		GameObject.onGetCell(self)


