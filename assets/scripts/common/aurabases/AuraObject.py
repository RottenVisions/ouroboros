# -*- coding: utf-8 -*-
import Ouroboros
from OURODebug import *

import Tools

class AuraObject:
	def __init__(self):
		self._id = 0
		self._name = ''
		self._level = 0
		self._icon = ''
		self._description = ''
		self._school = ''
		self._type = ''
		self._amount = 0
		self._period = 0
		self._duration = 0
		self._maxStacks = 0
		self._stackable = False

		self._isActive = False
		self._source = None
		self._target = None
		self._stacks = 0


	def loadFromDict(self, dictDatas):
		"""
		virtual method.
		Create this object from the dictionary
		"""
		self._id = dictDatas.get('id', 0)
		self._name = dictDatas.get('name', '')
		self._level = dictDatas.get('level', 0)
		self._icon = dictDatas.get('icon', '')
		self._description = dictDatas.get('description', '')
		self._amount = dictDatas.get('amount', 0)
		self._type = dictDatas.get('type', '')
		self._school = dictDatas.get('school', '')
		self._period = dictDatas.get('period', 0)
		self._duration = dictDatas.get('duration', 0)
		self._stackable = dictDatas.get('stackable', False)
		self._maxStacks = dictDatas.get('maxStacks', 0)

	def getID(self):
		return self._id

	def getName(self):
		return self._name

	def getLevel(self):
		return self._level

	def getIcon(self):
		return self._icon

	def getDescription(self):
		return self._description

	def getType(self):
		return self._type

	def getSchool(self):
		return self._school

	def getAmount(self):
		return self._amount

	def getPeriod(self):
		return self._period

	def getDuration(self):
		return self._duration

	def setActiveState(self, value):
		self._isActive = value

	def getActiveState(self):
		return self._isActive

	def setSource(self, value):
		self._source = value

	def getSource(self):
		return self._source

	def setTarget(self, value):
		self._target = value

	def getTarget(self):
		return self._target

	def getStacks(self):
		return self._stacks

	def getMaxStacks(self):
		return self._maxStacks

	def getStackable(self):
		return Tools.stringToBool(self._stackable)

	def addStack(self):
		self._stacks += 1

	def setStacks(self, value):
		self._stacks = value
