# -*- coding: utf-8 -*-
import Ouroboros
import auras
import GlobalConst
import GlobalDefine
import ServerConstantsDefine

import aurabases.AuraCastObject as AuraCastObject

from OURODebug import *

class Aura:
	def __init__(self):
		pass

	def attachTargetAura(self, auraID, targetID, sourceID = -1):
		DEBUG_MSG("Aura::attachTargetAura(%i):auraID=%i, targetID=%i, sourceID=%i" % (self.id, auraID, targetID, sourceID))

		aura = auras.getAuraByID(auraID)

		if aura is None:
			ERROR_MSG("Aura::auraTarget(%i):auraID=%i not found" % (self.id, auraID))
			return

		target = Ouroboros.entities.get(targetID)
		aura.setTarget(target)

		if target is None:
			ERROR_MSG("Aura::auraTarget(%i):targetID=%i not found" % (self.id, targetID))
			return

		auraCastObject = AuraCastObject.createAuraCastEntity(target)
		ret = aura.canAttach(self, auraCastObject)

		if ret != GlobalConst.GC_OK:
			DEBUG_MSG("Aura::auraTarget(%i): cannot attach aura auraID=%i, targetID=%i, code=%i" % (
			self.id, auraID, targetID, ret))
			return

		newAura = aura.attach(self, auraCastObject)

		if sourceID is not -1:
			ent = Ouroboros.entities.get(sourceID)
			if ent is not None:
				newAura.setSource(ent)

		if target.allClients:
			target.allClients.onAuraStatusUpdate(aura.getID(), aura.getIcon(), aura.getDescription(),
										  GlobalDefine.AURA_UPDATE_ADDED, aura.getDuration())

		return newAura

	def refreshTargetAura(self, aura, targetID, sourceID = -1):
		target = Ouroboros.entities.get(targetID)
		aura.setTarget(target)

		if target is None:
			ERROR_MSG("Aura::auraRefresh(%i):targetID=%i not found" % (self.id, targetID))
			return

		ret = aura.canAttach(self, aura.scObject)

		if ret != GlobalConst.GC_OK:
			DEBUG_MSG("Aura::auraRefresh(%i): cannot attach aura auraID=%i, targetID=%i, code=%i" % (
			self.id, aura.getID(), targetID, ret))
			return

		aura.refresh(self, aura.scObject)

		if sourceID is not -1:
			ent = Ouroboros.entities.get(sourceID)
			if ent is not None:
				aura.setSource(ent)
				if target.allClients:
					target.allClients.onAuraStatusUpdate(aura.getID(), aura.getIcon(), aura.getDescription(), GlobalDefine.AURA_UPDATE_REFRESHED, aura.getDuration())

	def applyStackTargetAura(self, aura, targetID, sourceID = -1):
		if not aura.getStackable(): return

		target = Ouroboros.entities.get(targetID)
		aura.setTarget(target)

		if target is None:
			ERROR_MSG("Aura::applyStackTargetAura(%i):targetID=%i not found" % (self.id, targetID))
			return

		ret = aura.canAttach(self, aura.scObject)

		if ret != GlobalConst.GC_OK:
			DEBUG_MSG("Aura::auraRefresh(%i): cannot attach aura auraID=%i, targetID=%i, code=%i" % (
			self.id, aura.getID(), targetID, ret))
			return

		aura.applyStack(self, aura.scObject)

		if sourceID is not -1:
			ent = Ouroboros.entities.get(sourceID)
			if ent is not None:
				aura.setSource(ent)
				if target.allClients:
					target.allClients.onAuraStatusUpdate(aura.getID(), aura.getIcon(), aura.getDescription(), GlobalDefine.AURA_UPDATE_STACKED, aura.getDuration())

	def removeTargetAura(self, aura, targetID, sourceID = -1):
		ret = aura.canDetach(self, aura.scObject)

		if ret != GlobalConst.GC_OK:
			DEBUG_MSG("Aura::removeTargetAura(%i): cannot detach aura auraID=%i, targetID=%i, code=%i" % (
			self.id, aura.getID(), targetID, ret))
			return

		DEBUG_MSG("Aura::auraRemove(%i): Removed aura auraID=%i, sourceID=%i." % (
					self.id, aura.getID(), sourceID))

		aura.detach(aura.scObject)

		target = Ouroboros.entities.get(targetID)

		if target is None:
			ERROR_MSG("Aura::applyStackTargetAura(%i):targetID=%i not found" % (self.id, targetID))
			return

		target = Ouroboros.entities.get(targetID)
		if target is not None:
			if target.allClients:
				target.allClients.onAuraStatusUpdate(aura.getID(), aura.getIcon(), aura.getDescription(),
												  GlobalDefine.AURA_UPDATE_REMOVED, aura.getDuration())
		#if sourceID is not -1:
		#	ent = Ouroboros.entities.get(sourceID)
		#	if ent is not None:
		#		if ent.allClients:
		#			ent.allClients.onAuraStatusUpdate(aura.getID(), aura.getIcon(), aura.getDescription(), GlobalDefine.AURA_UPDATE_REMOVED, aura.getDuration())
		#	else:
		#		DEBUG_MSG("Aura::auraRemove(%i): cannot update clients to remove aura auraID=%i, sourceID=%i, Entity not found." % (
		#			self.id, aura.getID(), sourceID))
		#else:
		#	DEBUG_MSG("Aura::auraRemove(%i): cannot update clients to remove aura auraID=%i, sourceID=%i, Entity ." % (
		#		self.id, aura.getID(), sourceID))

	def getAura(self, auraID):
		return auras.getAuraByID(auraID)