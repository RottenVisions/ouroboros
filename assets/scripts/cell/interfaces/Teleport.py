# -*- coding: utf-8 -*-
import Ouroboros
import SpaceContext
from OURODebug import *

class Teleport:
	def __init__(self):
		pass

	def teleportSpace(self, spaceUType, position, direction, context):
		"""
		defined.
		Transfer to a scene
		"""
		assert self.base != None
		self.lastSpaceUType = self.spaceUType

		inputContext = SpaceContext.createContext(self, spaceUType)
		if type(context) == dict:
			inputContext.update(context)

		self.getSpaces().teleportSpace(self.base, spaceUType, position, direction, inputContext)

	#--------------------------------------------------------------------------------------------
	#                              Callbacks
	#--------------------------------------------------------------------------------------------
	def onTeleportSpaceCB(self, spaceCellEntityCall, spaceUType, position, direction):
		"""
		defined.
		Baseapp returns the callback of teleportSpace
		"""
		DEBUG_MSG("Teleport::onTeleportSpaceCB: %i spaceID=%s, spaceUType=%i, pos=%s, dir=%s." % \
					(self.id, spaceCellEntityCall.id, spaceUType, position, direction))


		self.getCurrSpaceBase().onLeave(self.id)
		self.teleport(spaceCellEntityCall, position, direction)

	def onTeleportSuccess(self, nearbyEntity):
		"""
		Ouroboros method.
		"""
		DEBUG_MSG("Teleport::onTeleportSuccess: %s" % (nearbyEntity))
		self.getCurrSpaceBase().onEnter(self.base)
		self.spaceUType = self.getCurrSpace().spaceUType

	def onDestroy(self):
		"""
		Entity destruction
		"""
		self.getCurrSpaceBase().logoutSpace(self.id)
