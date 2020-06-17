# -*- coding: utf-8 -*-
import random
import math
import time
import Ouroboros
from OURODebug import *
from interfaces.NPCObject import NPCObject
from interfaces.Motion import Motion

class NPC(Ouroboros.Entity, NPCObject, Motion):
	def __init__(self):
		Ouroboros.Entity.__init__(self)
		NPCObject.__init__(self)
		Motion.__init__(self)

	def isNPC(self):
		"""
		Virtual method.
		"""
		return True

	#--------------------------------------------------------------------------------------------
	#                              Callbacks
	#--------------------------------------------------------------------------------------------
	def onTimer(self, tid, userArg):
		"""
		Ouroboros method.
		Engine callback timer trigger
		"""
		#DEBUG_MSG("%s::onTimer: %i, tid:%i, arg:%i" % (self.getScriptName(), self.id, tid, userArg))
		NPCObject.onTimer(self, tid, userArg)

	def onDestroy(self):
		"""
		Entity destruction
		"""
		NPCObject.onDestroy(self)
