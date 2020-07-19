# -*- coding: utf-8 -*-
import Ouroboros
import random
import GlobalDefine
from OURODebug import *
from abilities.base.ActiveAbilty import ActiveAbility

class AttackAbility(ActiveAbility):
	def __init__(self):
		ActiveAbility.__init__(self)

	def canUse(self, caster, scObject):
		"""
		virtual method.
		Can use
		@param caster: Entity with Ability
		@param receiver: Entity
		"""
		return ActiveAbility.canUse(self, caster, scObject)

	def use(self, caster, scObject):
		"""
		virtual method.
		Use abilities
		@param caster: Entity with Ability
		@param receiver: Entity
		"""
		return ActiveAbility.use(self, caster, scObject)

	def receive(self, caster, receiver):
		"""
		virtual method.
		Can do something for the subject
		"""
		attack = random.randint(caster.attack_Min, caster.attack_Max)
		defence = receiver.defence
		if self.getID() == 1:
			damage = attack - defence + 10
		elif self.getID() == 2:
			damage = attack - defence + 30
		elif self.getID() == 4:
			damage = attack - defence + 30
		elif self.getID() == 5:
			damage = attack - defence + 40
		elif self.getID() == 6:
			damage = attack - defence + 30

		if damage < 0:
			damage = 0
		receiver.receiveDamage(receiver.id, caster.id, GlobalDefine.SOURCE_TYPE_ABILITY, self.getID(), self.getIcon(), self.getSchool(), damage)
		if self.getID() == 6: #Bloodsucking, add blood to yourself
			caster.receiveDamage(receiver.id, caster.id, GlobalDefine.SOURCE_TYPE_ABILITY, self.getID(), self.getIcon(), self.getSchool(), -int(damage*0.1))
