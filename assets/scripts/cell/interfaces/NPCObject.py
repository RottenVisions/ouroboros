# -*- coding: utf-8 -*-
import Ouroboros
from OURODebug import *
from interfaces.GameObject import GameObject

class NPCObject(GameObject):
	"""
	All non-role entity interface classes
	"""
	def __init__(self):
		GameObject.__init__(self)

	#--------------------------------------------------------------------------------------------
	#                              Callbacks
	#--------------------------------------------------------------------------------------------
	def onDestroy(self):
		"""
		Entity destruction
		"""
		if self.spawnID > 0:
			spawner = Ouroboros.entities.get(self.spawnID)
			if spawner:
				spawner.onEntityDestroyed(self.entityNO)
