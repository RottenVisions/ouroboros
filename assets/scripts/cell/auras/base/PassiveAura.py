# -*- coding: utf-8 -*-
import Ouroboros
import random
from OURODebug import *
from aurabases.AuraObject import AuraObject

class PassiveAura(AuraObject):
	def __init__(self):
		AuraObject.__init__(self)

	def loadFromDict(self, dictDatas):
		"""
		virtual method.
		Create this object from the dictionary
		"""
		AuraObject.loadFromDict(self, dictDatas)
