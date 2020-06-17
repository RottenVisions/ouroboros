# -*- coding: utf-8 -*-
from OURODebug import *
from interfaces.GameObject import GameObject

import data_spaces

class Space(Ouroboros.Entity,
			GameObject):
	"""
	Game scene, here represents the big map of the world
	"""
	def __init__(self):
		Ouroboros.Entity.__init__(self)
		GameObject.__init__(self)

		# A space represents an abstract space, where geometric resource data is added to the abstract space, if the data is a 3D scene.
		# The space used in navigate pathfinding is the 3D API. If it is 2D geometry data, navigate uses astar pathfinding.
		resPath = data_spaces.data.get(self.spaceUType)['resPath']
		#Ouroboros.addSpaceGeometryMapping(self.spaceID, None, resPath, True, {0 : "srv_novice_village_1.navmesh", 1 : "srv_novice_village_2.navmesh"})
		Ouroboros.addSpaceGeometryMapping(self.spaceID, None, resPath)

		DEBUG_MSG('created space[%d] entityID = %i, res = %s.' % (self.spaceUType, self.id, resPath))

		Ouroboros.globalData["space_%i" % self.spaceID] = self.base

	#--------------------------------------------------------------------------------------------
	#                              Callbacks
	#--------------------------------------------------------------------------------------------
	def onDestroy(self):
		"""
		Ouroboros method.
		"""
		del Ouroboros.globalData["space_%i" % self.spaceID]
		self.destroySpace()

	def onEnter(self, entityCall):
		"""
		Defined method.
		Entering the scene
		"""
		DEBUG_MSG('Space::onEnter space[%d] entityID = %i.' % (self.spaceUType, entityCall.id))

	def onLeave(self, entityID):
		"""
		Defined method.
		Leave the scene
		"""
		DEBUG_MSG('Space::onLeave space[%d] entityID = %i.' % (self.spaceUType, entityID))
