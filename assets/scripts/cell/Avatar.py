# -*- coding: utf-8 -*-
import Ouroboros
import GlobalDefine
import ServerConstantsDefine
import Helper

import data_entities
import data_avatar_initial

from OURODebug import *
from interfaces.GameObject import GameObject
from interfaces.AnimationState import AnimationState
from interfaces.Combat import Combat
from interfaces.Ability import Ability
from interfaces.Teleport import Teleport
#from interfaces.Dialog import Dialog
from interfaces.State import State
from interfaces.Motion import Motion
from interfaces.AbilityBox import AbilityBox
from interfaces.Aura import Aura
from interfaces.AuraBox import AuraBox

class Avatar(Ouroboros.Entity,
				GameObject,
				State,
				AnimationState,
				Motion,
			 	AbilityBox,
			 	Aura,
			 	AuraBox,
				Combat,
			 	Ability,
				Teleport):
				#Dialog):
	'''
	Cell Implementation
	'''
	def __init__(self):
		Ouroboros.Entity.__init__(self)
		GameObject.__init__(self)
		State.__init__(self)
		AnimationState.__init__(self)
		Motion.__init__(self)
		AbilityBox.__init__(self)
		AuraBox.__init__(self)
		Ability.__init__(self)
		Teleport.__init__(self)
		Combat.__init__(self)

		# Dialog.__init__(self)

		# Set the fastest speed allowed per second, the speed will be pulled back
		self.topSpeed = self.moveSpeed + 15.0
		# self.topSpeedY = 10.0

		self.setDefaultData()
		self.resetProperties()
		self.updateBaseProperties()
		self.resetEntity()
		self.onEnable()

	def isPlayer(self):
		"""
		Virtual method.
		"""
		return True

	def getPlayerName(self):
		return self.playerName

	def dropNotify(self, itemId, UUid, itemCount):
		'''data = data_entities.data.get(40001003)

		if data is None:
			ERROR_MSG("SpawnPoint::spawn:%i not found." % 40001003)
			return

		params = {
			"uid": data["id"],
			"utype": data["etype"],
			"modelID": data["modelID"],
			"dialogID": data["dialogID"],
			"name": data["name"],
			"descr": data.get("descr", ''),
			"itemId": itemId,
			"itemCount": itemCount,
		}

		e = Ouroboros.createEntity("DroppedItem", self.spaceID, tuple(self.position), tuple(self.direction), params)

		self.client.dropItem_re(itemId, UUid)'''

	def setDefaultData(self):
		roleType = Helper.getAvatarGlobalProperty(self.id, 'roleType')
		self.HP_Max = int(data_avatar_initial.data[roleType]["hpMax"])
		self.EG_Max = int(data_avatar_initial.data[roleType]["egMax"])

	def resetProperties(self):
		pass
		#self.attack_Max = self.strength * 2
		#self.attack_Min = self.strength * 1
		#self.defence = int(self.will / 4)
		#self.HP_Max = self.endurance * 10

	def equipNotify(self, itemId):
		self.equipWeapon = itemId

	def updateBaseProperties(self):
		self.client.onUpdateBaseProperties(self.HP, self.HP_Max, self.EG, self.EG_Max)

	# --------------------------------------------------------------------------------------------
	#                              Custom
	# --------------------------------------------------------------------------------------------

	def onEnable(self):
		self.heartbeatTimer = self.addTimer(0, ServerConstantsDefine.TICK_TYPE_HEARTBEAT,
											ServerConstantsDefine.TIMER_TYPE_HEARTBEAT)
		self.abilityTimer = self.addTimer(0, ServerConstantsDefine.TICK_TYPE_ABILITY,
										  ServerConstantsDefine.TIMER_TYPE_ABILITY_TICK)
		self.auraTimer = self.addTimer(0, ServerConstantsDefine.TICK_TYPE_AURA,
									   ServerConstantsDefine.TIMER_TYPE_AURA_TICK)

	def onDisable(self):
		pass

	def onHeardTimer(self):
		"""
		Entity heartbeat
		"""
		if self.isDead():
			self.resetEntity()

	def resetEntity(self):
		if len(self.auras) > 0:
			self.removeAllAuras()
	# --------------------------------------------------------------------------------------------
	#                              Callbacks
	# --------------------------------------------------------------------------------------------
	def onTimer(self, tid, userArg):
		"""
		Ouroboros method.
		Engine callback timer trigger
		"""
		# DEBUG_MSG("%s::onTimer: %i, tid:%i, arg:%i" % (self.getScriptName(), self.id, tid, userArg))
		GameObject.onTimer(self, tid, userArg)
		#Ability.onTimer(self, tid, userArg)

		if ServerConstantsDefine.TIMER_TYPE_HEARTBEAT == userArg:
			self.onHeardTimer()

		if ServerConstantsDefine.TIMER_TYPE_AURA_TICK == userArg:
			AuraBox.onTimer(self, tid, userArg)

		if ServerConstantsDefine.TIMER_TYPE_ABILITY_TICK == userArg:
			AbilityBox.onTimer(self, tid, userArg)

	def onGetWitness(self):
		"""
		Ouroboros method.
		Bind an observer(Client)
		"""
		DEBUG_MSG("Avatar::onGetWitness: %i." % self.id)

	def onLoseWitness(self):
		"""
		Ouroboros method.
		Unbind an observer(Client)
		"""
		DEBUG_MSG("Avatar::onLoseWitness: %i." % self.id)

	def onDestroy(self):
		"""
		Ouroboros method.
		Entity destruction
		"""
		DEBUG_MSG("Avatar::onDestroy: %i." % self.id)
		Teleport.onDestroy(self)
		Combat.onDestroy(self)

	# Requests
	def reqLevel(self):
		if self.client:
			self.client.onReqLevel(self.level)

	def reqAbilities(self):
		if self.client:
			self.client.onReqAbilities(self.abilities)

	def reqAbilityPoints(self):
		if self.client:
			self.client.onReqAbilityPoints(self.abilityPoints)

	def revive(self, exposed, type):
		"""
		defined.
		resurrection
		"""
		if exposed != self.id:
			return

		DEBUG_MSG("Avatar::revive: %i, type=%i." % (self.id, type))

		# Return to the city
		if type == 0:
			pass

		self.fullPower()
		self.resetEntity()
		self.changeState(GlobalDefine.ENTITY_STATE_FREE)

	def onAddEnemy(self, entityID):
		"""
		virtual method.
		Enemy enters the list
		"""
		if not self.isState(GlobalDefine.ENTITY_STATE_FIGHT):
			self.changeState(GlobalDefine.ENTITY_STATE_FIGHT)

	def onEnemyEmpty(self):
		"""
		virtual method.
		The enemy list is empty
		"""
		self.changeState(GlobalDefine.ENTITY_STATE_FREE)