# -*- coding: utf-8 -*-
import Ouroboros
import GlobalDefine

import random
from OURODebug import *
from auras.base.ActiveAura import ActiveAura

class HarmfulAura(ActiveAura):
	def __init__(self):
		ActiveAura.__init__(self)
		
	def onTimer(self, tid, userArg):
		"""
		Ouroboros method.
		Engine callback timer trigger
		"""
		ActiveAura.onTimerTick(self, tid, userArg, self)

	def canAttach(self, caster, scObject):
		"""
		virtual method.
		Can use
		@param caster: Entity that gives Aura
		@param receiver: Entity
		"""
		# GlobalConst.GC_OK
		return ActiveAura.canAttach(self, caster, scObject)

	def attach(self, attacher, scObject):
		"""
		virtual method.
		Attach an aura
		@param attacher: Entity that attaches Aura
		@param receiver: Entity
		"""
		self.onAttached(attacher, scObject)
		return ActiveAura.attachTo(self, attacher, scObject)

	def canDetach(self, remover, scObject):
		"""
		virtual method.
		Can use
		@param remover: Entity that wants to remove Aura
		@param receiver: Entity
		"""
		return ActiveAura.canDetach(self, remover, scObject)

	def detach(self, scObject):
		self.onDetached(scObject)
		return ActiveAura.detachFrom(self, scObject)

	def refresh(self, caster, scObject):
		"""
		virtual method.
		Attach an aura
		@param caster: Entity that casts Aura
		@param receiver: Entity
		"""
		self.onAttached(caster, scObject)
		return ActiveAura.refreshIt(self, caster, scObject, self)

	def onAuraCycleTick(self, tid, userArg, auraCastObject):
		entToApplyTo = auraCastObject.getTarget(self)
		entToApplyTo.receiveDamage(entToApplyTo.id, auraCastObject.getSource(self).id, GlobalDefine.SOURCE_TYPE_AURA, auraCastObject.getID(self), auraCastObject.getIcon(self), auraCastObject.getSchool(self), auraCastObject.getAmount(self))

	def onAttached(self, attacher, auraCastObject):
		self.scObject = auraCastObject

	def onDetached(self, scObject):
		self.scObject = None
		self.setStacks(0)
	
	def onRefreshed(self):
		pass

	def applyStack(self, caster, scObject):
		if ActiveAura.getStacks(self) >= ActiveAura.getMaxStacks(self):
			ActiveAura.setStacks(self, ActiveAura.getMaxStacks(self))
			return
		ActiveAura.addStack(self)
		#print('Stacks: %i of %i' % (ActiveAura.getStacks(self), ActiveAura.getMaxStacks(self)))