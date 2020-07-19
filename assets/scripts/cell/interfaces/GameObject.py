# -*- coding: utf-8 -*-
import Ouroboros
import data_spaces
import ServerConstantsDefine
import GlobalDefine
from OURODebug import *

import data_entities
import data_avatar_initial

class GameObject:
	"""
	The base interface class of the server game object
	"""
	def __init__(self):
		pass

	def initEntity(self):
		"""
		Virtual method.
		"""
		pass

	def isPlayer(self):
		"""
		Virtual method.
		"""
		return False

	def isNPC(self):
		"""
		Virtual method.
		"""
		return False

	def isEnemy(self):
		"""
		Virtual method.
		"""
		return False

	def getDatas(self):
		if self.isPlayer():
			return data_avatar_initial.data[self.uid]

		return data_entities.data[self.uid]

	def getScriptName(self):
		return self.__class__.__name__

	def getName(self):
		return data_entities.data[self.uid]['name']

	def getCurrSpaceBase(self):
		"""
		Get the entity baseEntityCall of the current space
		"""
		return Ouroboros.globalData["space_%i" % self.spaceID]

	def getCurrSpace(self):
		"""
		Get the entity of the current space
		"""
		spaceBase = self.getCurrSpaceBase()
		return Ouroboros.entities.get(spaceBase.id, None)

	def getSpaces(self):
		"""
		Get the scene manager
		"""
		return Ouroboros.globalData["Spaces"]

	def startDestroyTimer(self):
		"""
		Virtual method.

		Start destroying entitytimer
		"""
		if self.isState(GlobalDefine.ENTITY_STATE_DEAD) & ~self.isPlayer():
			self.addTimer(5, 0, ServerConstantsDefine.TIMER_TYPE_DESTROY)
			DEBUG_MSG("%s::startDestroyTimer: %i running." % (self.getScriptName(), self.id))

	#--------------------------------------------------------------------------------------------
	#                              Callbacks
	#--------------------------------------------------------------------------------------------
	def onTimer(self, tid, userArg):
		"""
		Ouroboros method.
		Engine callback timer trigger
		"""
		#DEBUG_MSG("%s::onTimer: %i, tid:%i, arg:%i" % (self.getScriptName(), self.id, tid, userArg))
		if ServerConstantsDefine.TIMER_TYPE_DESTROY == userArg:
			self.onDestroyEntityTimer()

	def onStateChanged_(self, oldstate, newstate):
		"""
		Virtual method.
		The entity state has changed.
		"""
		self.startDestroyTimer()

	def onWitnessed(self, isWitnessed):
		"""
		Ouroboros method.
		Whether this entity is observed by the player,
		this interface is mainly to provide the server with some performance optimization work.
		Under normal circumstances, when some entities are not observed by any client, they do not need to do any work, using this interface
		Any behavior of this entity can be activated or stopped at the appropriate time.
		@param isWitnessed	: When false, the entity is out of observation by any observer.
		"""
		DEBUG_MSG("%s::onWitnessed: %i isWitnessed=%i." % (self.getScriptName(), self.id, isWitnessed))

	def onEnterTrap(self, entityEntering, range_xz, range_y, controllerID, userarg):
		"""
		Ouroboros method.
		Engine callback into trap trigger
		"""
		if entityEntering.getScriptName() == "Avatar":
			DEBUG_MSG("%s::onEnterTrap: %i entityEntering=%i, range_xz=%s, range_y=%s, controllerID=%i, userarg=%i" % \
							(self.getScriptName(), self.id, entityEntering.id, range_xz, range_y, controllerID, userarg))

	def onLeaveTrap(self, entityLeaving, range_xz, range_y, controllerID, userarg):
		"""
		Ouroboros method.
		Engine callback leaves trap trigger
		"""
		if entityLeaving.getScriptName() == "Avatar":
			DEBUG_MSG("%s::onLeaveTrap: %i entityLeaving=%i, range_xz=%s, range_y=%s, controllerID=%i, userarg=%i" % \
							(self.getScriptName(), self.id, entityLeaving.id, range_xz, range_y, controllerID, userarg))

	def onRestore(self):
		"""
		Ouroboros method.
		The cell part of the entity is successfully restored.
		"""
		DEBUG_MSG("%s::onRestore: %s" % (self.getScriptName(), self.base))

	def onDestroyEntityTimer(self):
		"""
		Entity delay destroyer timer
		"""
		self.destroy()
