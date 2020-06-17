# -*- coding: utf-8 -*-
import Ouroboros
import abilities
import GlobalConst
import GlobalDefine
import ServerConstantsDefine
from OURODebug import *

class AuraBox:
	def __init__(self):
		self.auraData = []

	def addAura(self, aura):
		"""
		defined method.
		"""
		if not aura.getID() in self.auras:
			self.auras.append(aura.getID())
		if not aura in self.auraData:
			self.auraData.append(aura)


	def removeAura(self, aura):
		"""
		defined method.
		"""
		if aura.getID() in self.auras:
			self.auras.remove(aura.getID())
		if aura in self.auraData:
			self.auraData.remove(aura)

	def removeAllAuras(self):
		for aura in self.auraData:
			self.removeAura(aura)
			self.client.onAuraStatusUpdate(aura.getID(), aura.getIcon(), aura.getDescription(),
										   GlobalDefine.AURA_UPDATE_REMOVED, 0)

	def hasAura(self, auraID):
		"""
		"""
		return auraID in self.auras

	def onTimer(self, tid, userArg):
		#Run one timer for all auras this entity contains
		for aura in self.auraData:
			#If an aura is active and bound, run its timer
			if aura.getActiveState():
				aura.onTimer(tid, userArg)
			#Aura has expired, remove it
			else:
				self.removeAura(aura)
				self.client.onAuraStatusUpdate(aura.getID(), aura.getIcon(), aura.getDescription(), GlobalDefine.AURA_UPDATE_FINISHED, 0)

	def applyTargetAura(self, auraID, targetID, sourceID = -1):
		"""
		exposed.
		Cast a aura on a target entity
		"""
		if auraID in self.auras:
			aura = self.getAura(auraID)
			if aura.getStackable():
				self.applyStackTargetAura(aura, targetID)
			self.refreshTargetAura(aura, targetID)

		newAura = self.attachTargetAura(auraID, targetID, sourceID)

		if newAura is not None:
			self.addAura(newAura)
			return True

		return False

	def removeTargetAura(self):
		pass

	def getActiveAurasAsString(self):
		return ', '.join(map(str, self.auras))