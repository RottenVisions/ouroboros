# -*- coding: utf-8 -*-
import Ouroboros
import abilities
import GlobalConst
import ServerConstantsDefine
from OURODebug import *
import abilitybases.AbilityCastObject as AbilityCastObject

class Ability:
	def __init__(self):
		pass

	def casting(self, ability, abilityCastObject):
		"""
		casting abilities
		"""
		pass

	def abilityTarget(self, abilityID, targetID):
		"""
		defined.
		Cast a ability on a target entity
		"""
		DEBUG_MSG("Ability::abilityTarget(%i):abilityID=%i, targetID=%i" % (self.id, abilityID, targetID))

		ability = abilities.getAbility(abilityID)
		if ability is None:
			ERROR_MSG("Ability::abilityTarget(%i):abilityID=%i not found" % (self.id, abilityID))
			return

		target = Ouroboros.entities.get(targetID)

		#if target.isNPC():
			#return

		if target is None:
			ERROR_MSG("Ability::abilityTarget(%i):targetID=%i not found" % (self.id, targetID))
			return

		abilityCastObject = AbilityCastObject.createAbilityCastEntity(target)
		ret = ability.canUse(self, abilityCastObject)
		if ret != GlobalConst.GC_OK:
			DEBUG_MSG("Ability::abilityTarget(%i): cannot ability abilityID=%i, targetID=%i, code=%i" % (self.id, abilityID, targetID, ret))
			return

		ability.use(self, abilityCastObject)

	def abilityPosition(self, position):
		pass

	#--------------------------------------------------------------------------------------------
	#                              Callbacks
	#--------------------------------------------------------------------------------------------
	def onTimer(self, tid, userArg):
		"""
		Ouroboros method.
		Engine callback timer trigger
		"""
		pass

