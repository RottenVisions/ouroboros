# -*- coding: utf-8 -*-
import Ouroboros
import random
from OURODebug import *
from abilitybases.AbilityObject import AbilityObject

class PassiveAbility(AbilityObject):
	def __init__(self):
		AbilityObject.__init__(self)

	def loadFromDict(self, dictDatas):
		"""
		virtual method.
		Create this object from the dictionary
		"""
		AbilityObject.loadFromDict(self, dictDatas)
