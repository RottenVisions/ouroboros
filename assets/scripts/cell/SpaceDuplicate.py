# -*- coding: utf-8 -*-
import Ouroboros
import random
from OURODebug import *
from Space import Space
import data_entities
import data_spaces
import ServerConstantsDefine

class SpaceDuplicate(Space):
	def __init__(self):
		Space.__init__(self)

		self.avatars = {}

		self.addTimer(30, 10, ServerConstantsDefine.TIMER_TYPE_HEARTBEAT)

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
			self.onCheckDestroyTimer()

		Space.onTimer(self, tid, userArg)

	def onCheckDestroyTimer(self):
		if len(self.avatars) > 0:
			return

		# Destroy if no one is there
		DEBUG_MSG("SpaceDuplicate::onCheckDestroyTimer: %i" % (self.id))
		self.destroy()

	def onEnter(self, entityCall):
		"""
		Defined method.
		Entering the scene
		"""
		self.avatars[entityCall.id] = entityCall
		Space.onEnter(self, entityCall)

	def onLeave(self, entityID):
		"""
		Defined method.
		Leave the scene
		"""
		if entityID in self.avatars:
			del self.avatars[entityID]

		Space.onLeave(self, entityID)
