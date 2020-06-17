# -*- coding: utf-8 -*-
import Ouroboros
import random
from OURODebug import *
from abilities.base.ActiveAbilty import ActiveAbility

class HealAbility(ActiveAbility):
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
		if self.getID() == 3:
			damage = attack + 10

		receiver.receiveDamage(caster.id, GlobalDefine.SOURCE_TYPE_ABILITY, self.getID(), self.getSchool(), -damage)
