# -*- coding: utf-8 -*-
import Ouroboros
from OURODebug import *


class AbilityObject:
	def __init__(self):
		self._id = 0
		self._name = ''
		self._level = 0
		self._icon = ''

	def loadFromDict(self, dictDatas):
		"""
		virtual method.
		Create this object from the dictionary
		"""
		self._id = dictDatas.get('id', 0)
		self._name = dictDatas.get('name', '')
		self._level = dictDatas.get('level', 0)
		self._icon = dictDatas.get('icon', 0)

	def getID(self):
		return self._id

	def getName(self):
		return self._name

	def getLevel(self):
		return self._level

	def getIcon(self):
		return self._icon
