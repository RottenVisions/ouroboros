# -*- coding: utf-8 -*-
import Ouroboros
import abilities
import GlobalConst
import ServerConstantsDefine
from OURODebug import *

class AbilityBox:
	def __init__(self):
		pass

	def onTimer(self, tid, userArg):
		pass

	def hasAbility(self, abilityID):
		"""
		"""
		return abilityID in self.abilities

	#--------------------------------------------------------------------------------------------
	#                              defined
	#--------------------------------------------------------------------------------------------
	def requestPull(self, exposed):
		"""
		exposed
		"""
		if self.id != exposed:
			return

		DEBUG_MSG("AbilityBox::requestPull: %i abilities=%i" % (self.id, len(self.abilities)))
		for abilityID in self.abilities:
			self.client.onAddAbility(abilityID)

	def addAbility(self, abilityID):
		"""
		defined method.
		"""
		self.abilities.append(abilityID)

	def removeAbility(self, abilityID):
		"""
		defined method.
		"""
		self.abilities.remove(abilityID)

	def useTargetAbility(self, srcEntityID, abilityID, targetID):
		"""
		exposed.
		Cast a ability on a target entity
		"""
		if srcEntityID != self.id:
			return

		self.abilityTarget(abilityID, targetID)
