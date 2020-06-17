# -*- coding: utf-8 -*-
import random
import math
import time
import Ouroboros
from abilitybases.AbilityCastObject import AbilityCastObject
import data_entities
from OURODebug import *
from interfaces.Combat import Combat
from interfaces.Ability import Ability
from interfaces.Motion import Motion
from interfaces.State import State
from interfaces.AnimationState import AnimationState
from interfaces.AI import AI
from interfaces.NPCObject import NPCObject
from interfaces.Flags import Flags

class Enemy(Ouroboros.Entity,
			NPCObject,
			State,
			AnimationState,
			Motion,
			Combat,
			Ability,
			AI):
	def __init__(self):
		Ouroboros.Entity.__init__(self)
		NPCObject.__init__(self)
		State.__init__(self)
		AnimationState.__init__(self)
		Motion.__init__(self)
		Combat.__init__(self)
		Ability.__init__(self)
		AI.__init__(self)

		# The layer where the entity is located can be set up with multiple different navmesh layers to find the way. Here, 20002001 is the dragon in the warring-demo.
		# The 0th floor is the ground, and the 1st floor is the pathfinding layer that ignores the building.
		'''if self.modelID == 20002001:
			self.layer = 1'''

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
		return AI.checkInTerritory(self)

	def isEnemy(self):
		"""
		virtual method.
		"""
		return True

	def dropNotify(self, itemId, itemCount):
		'''datas = data_entities.datas.get(40001003)

		if datas is None:
			ERROR_MSG("SpawnPoint::spawn:%i not found." % 40001003)
			return

		params = {
			"uid" : datas["id"],
			"utype" : datas["etype"],
			"modelID" : datas["modelID"],
			"dialogID" : datas["dialogID"],
			"name" : datas["name"],
			"descr" : datas.get("descr", ''),
			"itemId" : itemId,
			"itemCount" : itemCount,
		}

		e = Ouroboros.createEntity("DroppedItem", self.spaceID, tuple(self.position), tuple(self.direction), params)'''

	#--------------------------------------------------------------------------------------------
	#                              Callbacks
	#--------------------------------------------------------------------------------------------
	def onTimer(self, tid, userArg):
		"""
		Ouroboros method.
		Engine callback timer trigger
		"""
		DEBUG_MSG("%s::onTimer: %i, tid:%i, arg:%i" % (self.getScriptName(), self.id, tid, userArg))
		NPCObject.onTimer(self, tid, userArg)
		Ability.onTimer(self, tid, userArg)
		AI.onTimer(self, tid, userArg)
		#AnimationState.onTimer(self, tid, userArg)
		#AnimationState.onMotionChanged(self, self.isMoving)

	def onWitnessed(self, isWitnessed):
		"""
		Ouroboros method.
		Whether this entity is observed by the player, this interface is mainly to provide the server with some performance optimization work.
		Under normal circumstances, when some entities are not observed by any client, they do not need to do any work, using this interface
		Any behavior of this entity can be activated or stopped at the appropriate time.
		@param isWitnessed	: When false, the entity is out of observation by any observer.
		"""
		AI.onWitnessed(self, isWitnessed)

	def onForbidChanged_(self, forbid, isInc):
		"""
		virtual method.
		Entity prohibits condition change
		@param isInc		:	Is it an increase?
		"""
		State.onForbidChanged_(self, forbid, isInc)
		AI.onForbidChanged_(self, forbid, isInc)

	def onStateChanged_(self, oldstate, newstate):
		"""
		virtual method.
		The entity state has changed.
		"""
		State.onStateChanged_(self, oldstate, newstate)
		AI.onStateChanged_(self, oldstate, newstate)
		NPCObject.onStateChanged_(self, oldstate, newstate)
		AnimationState.onStateChanged_(self, oldstate, newstate)

	def onSubStateChanged_(self, oldSubState, newSubState):
		"""
		virtual method.
		Substate changed
		"""
		State.onSubStateChanged_(self, oldSubState, newSubState)
		AI.onSubStateChanged_(self, oldSubState, newSubState)
		AnimationState.onSubStateChanged_(self, oldSubState, newSubState)

	def onFlagsChanged_(self, flags, isInc):
		"""
		virtual method.
		"""
		Flags.onFlagsChanged_(self, flags, isInc)
		AI.onFlagsChanged_(self, flags, isInc)

	def onEnterTrap(self, entity, range_xz, range_y, controllerID, userarg):
		"""
		Ouroboros method.
		Engine callback into trap trigger
		"""
		AI.onEnterTrap(self, entity, range_xz, range_y, controllerID, userarg)

	def onLeaveTrap(self, entity, range_xz, range_y, controllerID, userarg):
		"""
		Ouroboros method.
		Engine callback leaves trap trigger
		"""
		AI.onLeaveTrap(self, entity, range_xz, range_y, controllerID, userarg)

	def onAddEnemy(self, entityID):
		"""
		virtual method.
		Enemy enters the list
		"""
		AI.onAddEnemy(self, entityID)
		Combat.onAddEnemy(self, entityID)

	def onRemoveEnemy(self, entityID):
		"""
		virtual method.
		Delete enemy
		"""
		AI.onRemoveEnemy(self, entityID)
		Combat.onRemoveEnemy(self, entityID)

	def onEnemyEmpty(self):
		"""
		virtual method.
		The enemy list is empty
		"""
		AI.onEnemyEmpty(self)
		Combat.onEnemyEmpty(self)

	def onDestroy(self):
		"""
		Entity destruction
		"""
		NPCObject.onDestroy(self)
		Combat.onDestroy(self)
