import Ouroboros;
from OURODebug import *

class Scene(Ouroboros.Entity):
	"""
	The cell part of the Scene
	A space on the cell represents an abstract space
	The scene is an entity representation of the abstract space for easy control
	"""
	def __init__(self):
		Ouroboros.Entity.__init__(self)
		# Stored in globalData for easy access
		Ouroboros.globalData["scene"] = self.base

	def onEnter(self, entityCall):
		"""
		Entering the Scene
		Called by the base part of Scene, the cell part is notified, and Entity enters this scenario.
		:param entityCall:
		:return:
		"""
		DEBUG_MSG('Scene[%i]::onEnter: entityID = %i.' % (self.id, entityCall.id))

	def onLeave(self, entityId):
		"""
		Leaving the Scene
		Called by the base part of Scene, the cell part is notified, and Entity leaves the scene.
		:param entityId:
		:return:
		"""
		DEBUG_MSG('Scene[%i]::onLeave: entityID = %i.' % (self.id, entityId))

	def onDestroy(self):
		"""
		Called when an entity is destroyed
		:return:
		"""
		#When the cell part of Scene is destroyed, the corresponding data of globalData is removed.
		del Ouroboros.globalData["scene"]
		# API: destroy the space where the scene entity is located
		# When the cell part of the Scene is destroyed, it indicates that the corresponding Space space cannot be managed.
		# You need to request the engine to destroy the Space.
		self.destroySpace()