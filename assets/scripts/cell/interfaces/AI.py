# -*- coding: utf-8 -*-
import Ouroboros
import ServerConstantsDefine
import time
import random
import GlobalDefine
from OURODebug import *
from abilitybases.AbilityCastObject import AbilityCastObject

import data_entities

__TERRITORY_AREA__ = 30.0

class AI:
	def __init__(self):
		self.enable()

	def initEntity(self):
		"""
		virtual method.
		"""
		pass

	def checkInTerritory(self):
		"""
		virtual method.
		Check if you are in an active territory
		"""
		ret = self.position.distTo(self.spawnPos) <= __TERRITORY_AREA__
		if not ret:
			INFO_MSG("%s::checkInTerritory: %i is False." % (self.getScriptName(), self.id))

		return ret

	def addTerritory(self):
		"""
		Add territory
		Certain entities entering the territorial scope will be considered enemies
		"""
		assert self.territoryControllerID == 0 and "territoryControllerID != 0"
		trange = __TERRITORY_AREA__ / 2.0
		self.territoryControllerID = self.addProximity(trange, 0, 0)

		if self.territoryControllerID <= 0:
			ERROR_MSG("%s::addTerritory: %i, range=%i, is error!" % (self.getScriptName(), self.id, trange))
		else:
			INFO_MSG("%s::addTerritory: %i range=%i, id=%i." % (self.getScriptName(), self.id, trange, self.territoryControllerID))

	def delTerritory(self):
		"""
		Delete territory
		"""
		if self.territoryControllerID > 0:
			self.cancelController(self.territoryControllerID)
			self.territoryControllerID = 0
			INFO_MSG("%s::delTerritory: %i" % (self.getScriptName(), self.id))

	def enable(self):
		"""
		Activate entity
		"""
		self.heartBeatTimerID = \
		self.addTimer(random.randint(0, 1), 1, ServerConstantsDefine.TIMER_TYPE_HEARTBEAT)				# Heartbeat timer, once every 1 second

	def disable(self):
		"""
		Prohibit this entity from doing anything
		"""
		self.delTimer(self.heartBeatTimerID)
		self.heartBeatTimerID = 0

	def think(self):
		"""
		virtual method.
		"""
		if self.isState(GlobalDefine.ENTITY_STATE_FREE):
			self.onThinkFree()
		elif self.isState(GlobalDefine.ENTITY_STATE_FIGHT):
			self.onThinkFight()
		else:
			self.onThinkOther()

		#if not self.isWitnessed:
			#self.disable()

	def choiceTarget(self):
		"""
		Choose an enemy from the hate list
		"""
		if len(self.enemyLog) > 0:
			self.targetID = self.enemyLog[0]
		else:
			self.targetID = 0

	def setTarget(self, entityID):
		"""
		Set target
		"""
		self.targetID = entityID
		self.onTargetChanged()

	#--------------------------------------------------------------------------------------------
	#                              Callbacks
	#--------------------------------------------------------------------------------------------
	def onHeardTimer(self):
		"""
		Entity heartbeat
		"""
		self.think()

	def onTargetChanged(self):
		"""
		virtual method.
		Goal change
		"""
		pass

	def onWitnessed(self, isWitnessed):
		"""
		Ouroboros method.
		Whether this entity is observed by the player, this interface is mainly to provide the server with some performance optimization work.
		Under normal circumstances, when some entities are not observed by any client, they do not need to do any work, using this interface
		Any behavior of this entity can be activated or stopped at the appropriate time.
		@param isWitnessed	: When false, the entity is out of observation by any observer.
		"""
		INFO_MSG("%s::onWitnessed: %i isWitnessed=%i." % (self.getScriptName(), self.id, isWitnessed))

		if isWitnessed:
			self.enable()

	def onThinkFree(self):
		"""
		virtual method.
		Think when idle
		"""
		if self.territoryControllerID <= 0:
			self.addTerritory()

		self.randomWalk(self.spawnPos)

	def onThinkFight(self):
		"""
		virtual method.
		Think during battle
		"""
		if self.territoryControllerID > 0:
			self.delTerritory()

		self.checkEnemys()

		if self.targetID <= 0:
			return

		dragon = (self.modelID == 20002001)

		# The simple implementation of the demo, if it is a dragon,
		# the attack distance is relatively far, the attack distance should call different abilities to determine
		attackMaxDist = 2.0
		if dragon:
			attackMaxDist = 20.0

		entity = Ouroboros.entities.get(self.targetID)

		if entity.position.distTo(self.position) > attackMaxDist:
			runSpeed = self.getDatas()["runSpeed"]
			if runSpeed != self.moveSpeed:
				self.moveSpeed = float(runSpeed)
			self.gotoPosition(entity.position, attackMaxDist - 0.2)
			return
		else:
			self.resetSpeed()

			abilityID = 4
			if dragon:
				abilityID = 7000101

			self.abilityTarget(abilityID, entity.id)

	def onThinkOther(self):
		"""
		virtual method.
		Other times think
		"""
		pass

	def onForbidChanged_(self, forbid, isInc):
		"""
		virtual method.
		Entity prohibits condition change
		@param isInc		:	Is it an increase?
		"""
		pass

	def onStateChanged_(self, oldstate, newstate):
		"""
		virtual method.
		The entity state has changed.
		"""
		if self.isState(GlobalDefine.ENTITY_STATE_DEAD):
			if self.isMoving:
				self.stopMotion()

	def onSubStateChanged_(self, oldSubState, newSubState):
		"""
		virtual method.
		Substate changed
		"""
		#INFO_MSG("%i oldSubstate=%i to newSubstate=%i" % (self.id, oldSubState, newSubState))
		pass

	def onFlagsChanged_(self, flags, isInc):
		"""
		virtual method.
		"""
		pass

	def onEnterTrap(self, entityEntering, range_xz, range_y, controllerID, userarg):
		"""
		Ouroboros method.
		Have an entity into the trap
		"""
		if controllerID != self.territoryControllerID:
			return

		#This effectively ignores other enemies, and only proceeds if the trapped entity is a player (Avatar)
		if entityEntering.isDestroyed or entityEntering.getScriptName() != "Avatar" or entityEntering.isDead():
			return

		if not self.isState(GlobalDefine.ENTITY_STATE_FREE):
			return

		DEBUG_MSG("%s::onEnterTrap: %i entityEntering=(%s)%i, range_xz=%s, range_y=%s, controllerID=%i, userarg=%i" % \
						(self.getScriptName(), self.id, entityEntering.getScriptName(), entityEntering.id, \
						range_xz, range_y, controllerID, userarg))

		self.addEnemy(entityEntering.id, 0)

	def onLeaveTrap(self, entityLeaving, range_xz, range_y, controllerID, userarg):
		"""
		Ouroboros method.
		Have an entity to leave the trap
		"""
		if controllerID != self.territoryControllerID:
			return

		if entityLeaving.isDestroyed or entityLeaving.getScriptName() != "Avatar" or entityLeaving.isDead():
			return

		INFO_MSG("%s::onLeaveTrap: %i entityLeaving=(%s)%i." % (self.getScriptName(), self.id, \
				entityLeaving.getScriptName(), entityLeaving.id))

	def onAddEnemy(self, entityID):
		"""
		virtual method.
		Enemy enters the list
		"""
		if not self.isState(GlobalDefine.ENTITY_STATE_FIGHT):
			self.changeState(GlobalDefine.ENTITY_STATE_FIGHT)

		if self.targetID == 0:
			self.setTarget(entityID)

	def onRemoveEnemy(self, entityID):
		"""
		virtual method.
		Delete enemy
		"""
		if self.targetID == entityID:
			self.onLoseTarget()

	def onLoseTarget(self):
		"""
		Enemy lost
		"""
		INFO_MSG("%s::onLoseTarget: %i target=%i, enemyLogSize=%i." % (self.getScriptName(), self.id, \
				self.targetID, len(self.enemyLog)))

		self.targetID = 0

		if len(self.enemyLog) > 0:
			self.choiceTarget()

	def onEnemyEmpty(self):
		"""
		virtual method.
		The enemy list is empty
		"""
		INFO_MSG("%s::onEnemyEmpty: %i" % (self.getScriptName(), self.id))

		if not self.isState(GlobalDefine.ENTITY_STATE_FREE):
			self.changeState(GlobalDefine.ENTITY_STATE_FREE)

		self.backSpawnPos()

	def onTimer(self, tid, userArg):
		"""
		Ouroboros method.
		Engine callback timer trigger
		"""
		DEBUG_MSG("%s::onTimer: %i, tid:%i, arg:%i" % (self.getScriptName(), self.id, tid, userArg))
		if ServerConstantsDefine.TIMER_TYPE_HEARTBEAT == userArg:
			self.onHeardTimer()
