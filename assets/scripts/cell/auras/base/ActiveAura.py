# -*- coding: utf-8 -*-
import Ouroboros
import GlobalConst
import ServerConstantsDefine

from OURODebug import *

from aurabases.AuraObject import AuraObject

class ActiveAura(AuraObject):
	def __init__(self):
		AuraObject.__init__(self)
		self.auraTimer = -1
		self.tickCount = -1
		self.scObject = None

	def loadFromDict(self, dictDatas):
		"""
		virtual method.
		Create this object from the dictionary
		Runs when all Auras are added to the from their files on init
		"""
		AuraObject.loadFromDict(self, dictDatas)
		#self._period = dictDatas.get('period', 0)
		#self._duration = dictDatas.get('duration', 0)

	def onTimerTick(self, tid, userArg, superScript):
		if not AuraObject.getActiveState(self) or self.auraTimer <= 0:
			self.auraTimer = 0
			return

		self.auraTimer -= ServerConstantsDefine.TICK_TYPE_AURA

		if self.auraTimer % AuraObject.getPeriod(self) == 0:
			self.tickCount += 1
			self.onAuraTick(tid, userArg, superScript)
		if self.auraTimer <= 0:
			self.detachFrom(superScript)

	def onAttach(self, attacher, auraCastObject):
		"""
		virtual method.
		When an aura is bound
		"""
		if self.auraTimer is not -1:
			return False

		self.auraTimer = AuraObject.getDuration(self)
		self.tickCount = 0

		AuraObject.setActiveState(self, True)
		ActiveAura.setSource(self, attacher)

	def onDetach(self, context):
		"""
		virtual method.
		When an aura is unbound
		"""
		AuraObject.setActiveState(self, False)
		ActiveAura.setSource(self, None)
		self.auraTimer = -1
		self.tickCount = -1

	def onAuraTick(self, tid, userArg, superScript):
		"""
		virtual method.
		Cycle trigger
		"""
		superScript.onAuraCycleTick(tid, userArg, ActiveAura)
	
	def canAttach(self, attacher, auraCastObject):
		
		#Run any checks here then, run the super
		return GlobalConst.GC_OK

	def canDetach(self, detacher, auraCastObject):
		# Run any checks here then, run the super
		return GlobalConst.GC_OK
	
	def attachTo(self, attacher, auraCastObject):
		self.onAttach(attacher, auraCastObject)
		return self # Note this return self here is re-used when attaching to a de-tached aura. If this becomes a problem use deep copy when creating an aura (like with items)

	def detachFrom(self, superScript):
		self.onDetach(AuraObject)
	
	def refreshIt(self, attacher, auraCastObject, superScript):
		if self.auraTimer > AuraObject.getDuration(self) and \
				self.auraTimer > 0:
			return False

		self.auraTimer = AuraObject.getDuration(self)
		self.tickCount = 0
		AuraObject.setActiveState(self, True)
		ActiveAura.setSource(self, attacher)
		superScript.onRefreshed()