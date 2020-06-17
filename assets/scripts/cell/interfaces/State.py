# -*- coding: utf-8 -*-
#
"""
Handling some states of the entity
"""
import GlobalDefine
from OURODebug import *

class State:
	"""
	"""
	def __init__(self):
		self._forbidCounter = [0] * len(GlobalDefine.FORBID_ALL)
		self.forbidCounterInc(GlobalDefine.FORBID_ACTIONS[self.state])

	def initEntity(self):
		"""
		virtual method.
		"""
		pass

	def isState(self, state):
		return self.state == state

	def isSubState(self, state):
		return self.subState == state

	def isForbid(self, forbid):
		return self.forbids & forbid

	def getState(self):
		return self.state

	def getSubState(self):
		return self.subState

	def getForbidCounter(self, forbid):
		"""
		Get data for the forbidden counter
		"""
		return self._forbidCounter[GlobalDefine.FORBID_ALL.index(forbid)]

	def changeSubState(self, subState):
		"""
		Change the current substate
		GlobalDefine.ENTITY_SUB_STATE_**
		"""
		if self.subState != subState:
			oldSubState = self.subState
			self.subState = subState
			self.onSubStateChanged_(oldSubState, self.subState)

	def forbidCounterInc(self, forbids):
		"""
		Do not add one counter
		"""
		fbList = []
		for i, fb in enumerate(GlobalDefine.FORBID_ALL):
			if forbids & fb:
				if self._forbidCounter[i] == 0:
					fbList.append(fb)
				self._forbidCounter[i] += 1

		# kbe Any time you assign a value to a defined property, an event is generated.
		if len(fbList) > 0:
			self.forbids |= forbids
			for fb in fbList:
				self.onForbidChanged_(fb, True)

	def forbidCounterDec(self, forbids):
		"""
		Disable counter minus one
		"""
		fbList = []
		for i, fb in enumerate(GlobalDefine.FORBID_ALL):
			if forbids & fb:
				self._forbidCounter[i] -= 1
				if self._forbidCounter[i] == 0:
					fbList.append(fb)

		# kbe Any time you assign a value to a defined property, an event is generated.
		if len(fbList) > 0:
			self.forbids &= ~forbids
			for fb in fbList:
				self.onForbidChanged_(fb, False)

	#--------------------------------------------------------------------------------------------
	#                              Callbacks
	#--------------------------------------------------------------------------------------------
	def onForbidChanged_(self, forbid, isInc):
		"""
		virtual method.
		Entity prohibits condition change
		@param isInc		:	Is it an increase?
		"""
		pass

	def onStateChanged_(self, oldstate, newstate):
		"""
		virtual method.
		The entity state has changed.
		"""
		self.changeSubState(GlobalDefine.ENTITY_SUB_STATE_NORMAL)
		INFO_MSG("%s:onStateChanged_: %i oldstate=%i to newstate=%i, forbids=%s, subState=%i." % (self.getScriptName(), \
				self.id, oldstate, newstate, self._forbidCounter, self.subState))

	def onSubStateChanged_(self, oldSubState, newSubState):
		"""
		virtual method.
		Substate changed
		"""
		#INFO_MSG("%i oldSubstate=%i to newSubstate=%i" % (self.id, oldSubState, newSubState))
		pass

	# ----------------------------------------------------------------
	# property method
	# ----------------------------------------------------------------
	def set_state(self, oldValue):
		DEBUG_MSG("%s::set_state: %i changed:%s->%s" % (self.getScriptName(), self.id, oldValue, self.state))
		self.onStateChanged_(oldValue, self.state)

	def set_effStates(self, oldValue):
		DEBUG_MSG("%s::set_effStates: %i changed:%s->%s" % (self.getScriptName(), self.id, oldValue, self.effStates))
		self.onEffectStateChanged_(oldValue, self.effStates)

	def set_forbids(self, oldValue):
		DEBUG_MSG("%s::set_forbids: %i changed:%s->%s" % (self.getScriptName(), self.id, oldValue, self.forbids))
		self.onForbidChanged_(oldValue, self.forbids)

	#--------------------------------------------------------------------------------------------
	#                              defined
	#--------------------------------------------------------------------------------------------
	def changeState(self, state):
		"""
		defined
		Change the current main state
		GlobalDefine.ENTITY_STATE_**
		"""
		if self.state != state:
			oldstate = self.state
			self.state = state
			self.forbidCounterDec(GlobalDefine.FORBID_ACTIONS[oldstate])
			self.forbidCounterInc(GlobalDefine.FORBID_ACTIONS[state])
			self.onStateChanged_(oldstate, state)