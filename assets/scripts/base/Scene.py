import Ouroboros
from OURODebug import *

class Scene(Ouroboros.Entity):
	"""
	Base part of the Scene
	Note: It is an entity, not a real space. The real space exists in the memory of the cellapp,
	through which the entity is associated and manipulates the space.
	"""

	def __init__(self):
		Ouroboros.Entity.__init__(self)
		# Request to create a cell to cellappmgr and associate it with the entity object
		# The parameter cellappIndex is None, indicating dynamic selection by engine load balancing.
		self.createCellEntityInNewSpace(None)

	def loginToScene(self, entityCall):
		"""
		An entity requests to log in to the scene
		:param entityCall: to enter the entity's entityCall of this scene
		:return:
		"""
		entityCall.createCell(self.cell)
		# Notify the cell part of the scene that someone has come in
		if self.cell is not None:
			self.cell.onEnter(entityCall)

	def logoutScene(self, entityId):
		"""
		A player requests to log out of the scene
		:param entityId: Logout Id
		:return:
		"""
		# Notify the cell part of the scene, someone has left
		if self.cell is not None:
			self.cell.onLeave(entityId)