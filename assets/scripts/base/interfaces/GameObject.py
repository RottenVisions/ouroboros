# -*- coding: utf-8 -*-
import Ouroboros
from OURODebug import *


class GameObject:
	"""
	The base interface class of the server game object
	"""

	def __init__(self):
		pass

	def getScriptName(self):
		return self.__class__.__name__

	def destroySelf(self):
		"""
		virtual method
		"""
		if self.cell is not None:
			# Destroy the cell entity
			self.destroyCellEntity()
			return

		# Destroy base
		self.destroy()

	def getSpaces(self):
		"""
		Get the scene manager
		"""
		return Ouroboros.globalData["Spaces"]

	# --------------------------------------------------------------------------------------------
	#                              Callbacks
	# --------------------------------------------------------------------------------------------
	def onTimer(self, tid, userArg):
		"""
		Ouroboros method.
		Engine callback timer trigger
		"""
		# DEBUG_MSG("%s::onTimer: %i, tid:%i, arg:%i" % (self.getScriptName(), self.id, tid, userArg))
		if self.isDestroyed:
			self.delTimer(tid)
			return

	def onGetCell(self):
		"""
		Ouroboros method.
		The cell part of the entity was created successfully.
		"""
		# DEBUG_MSG("%s::onGetCell: %i" % (self.getScriptName(), self.id))
		pass

	def onLoseCell(self):
		"""
		Ouroboros method.
		The entity part of the cell is lost
		"""
		DEBUG_MSG("%s::onLoseCell: %i" % (self.getScriptName(), self.id))
		self.destroySelf()

	def onRestore(self):
		"""
		Ouroboros method.
		The cell part of the entity is successfully restored.
		"""
		DEBUG_MSG("%s::onRestore: %s" % (self.getScriptName(), self.cell))


