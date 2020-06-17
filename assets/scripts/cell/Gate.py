# -*- coding: utf-8 -*-
import random
import math
import time
import ServerConstantsDefine
import data_spaces
import Ouroboros
from OURODebug import *
from interfaces.GameObject import GameObject

class Gate(Ouroboros.Entity, GameObject):
	"""
	This is a portal entity when the player enters the area of the portal "self.addProximity(5.0, 0, 0)".
	Portal sends the player to the designated place
	"""
	def __init__(self):
		Ouroboros.Entity.__init__(self)
		GameObject.__init__(self)

		self.addTimer(1, 0, ServerConstantsDefine.TIMER_TYPE_HEARTBEAT)				# Heartbeat timer, once every 1 second

	#--------------------------------------------------------------------------------------------
	#                              Callbacks
	#--------------------------------------------------------------------------------------------
	def onTimer(self, tid, userArg):
		"""
		Ouroboros method.
		Engine callback timer trigger
		"""
		#DEBUG_MSG("%s::onTimer: %i, tid:%i, arg:%i" % (self.getScriptName(), self.id, tid, userArg))
		if ServerConstantsDefine.TIMER_TYPE_HEARTBEAT == userArg:
			self.onHeardTimer()

		GameObject.onTimer(self, tid, userArg)

	def onHeardTimer(self):
		"""
		Entity heartbeat
		"""
		self.addProximity(5.0, 0, 0)

	def onEnterTrap(self, entityEntering, range_xz, range_y, controllerID, userarg):
		"""
		Ouroboros method.
		Have an entity to enter the trap
		"""
		if entityEntering.isDestroyed or entityEntering.getScriptName() != "Avatar":
			return

		DEBUG_MSG("%s::onEnterTrap: %i entityEntering=(%s)%i, range_xz=%s, range_y=%s, controllerID=%i, userarg=%i" % \
						(self.getScriptName(), self.id, entityEntering.getScriptName(), entityEntering.id, \
						range_xz, range_y, controllerID, userarg))

		if self.uid == 40001002: # currspace - teleport
			spaceData = data_spaces.data.get(entityEntering.spaceUType)
			entityEntering.teleport(None, spaceData["spawnPos"], tuple(self.direction))
		else:					 # teleport to xxspace
			if entityEntering.spaceUType == 3:
				gotoSpaceUType = 4
			else:
				gotoSpaceUType = 3

			spaceData = data_spaces.data.get(gotoSpaceUType)
			entityEntering.teleportSpace(gotoSpaceUType, spaceData["spawnPos"], tuple(self.direction), {})

	def onLeaveTrap(self, entityLeaving, range_xz, range_y, controllerID, userarg):
		"""
		Ouroboros method.
		Have an entity to leave the trap
		"""
		if entityLeaving.isDestroyed or entityLeaving.getScriptName() != "Avatar":
			return

		INFO_MSG("%s::onLeaveTrap: %i entityLeaving=(%s)%i." % (self.getScriptName(), self.id, \
				entityLeaving.getScriptName(), entityLeaving.id))
