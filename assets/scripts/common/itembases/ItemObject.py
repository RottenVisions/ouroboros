# -*- coding: utf-8 -*-
#import Ouroboros
#from OURODebug import *


class ItemObject:
	def __init__(self):
		self._id = 0
		self._name = ''
		self._level = 0
		self._icon = ''
		self._maxStack = 0
		self._stackable = False

		self._index = 0
		self._count = 1
		self._uuid = ''

	def loadFromDict(self, dictDatas):
		"""
		virtual method.
		Create this object from the dictionary
		"""
		self._id = dictDatas.get('id', 0)
		self._name = dictDatas.get('name', '')
		self._maxStack = dictDatas.get('maxStack', 0)
		self._icon = dictDatas.get('icon', '')
		self._stackable = dictDatas.get('stackable', False)

	def copyInto(self, itemObj):
		self._id = itemObj.getID()
		self._name = itemObj.getName()
		self._level = itemObj.getLevel()
		self._icon = itemObj.getIcon()
		self._maxStack = itemObj.getMaxStack()
		self._stackable = itemObj.getStackable()

		self._index = itemObj.getIndex()
		self._count = itemObj.getCount()
		self._uuid = itemObj.getUUID()

	def getID(self):
		return self._id

	def getName(self):
		return self._name

	def getLevel(self):
		return self._level

	def getStackable(self):
		return self._stackable

	def getMaxStack(self):
		# Catch maxStack being 0 or less
		if self._maxStack <= 0:
			return 1
		return self._maxStack

	def getIcon(self):
		return self._icon

	def getIndex(self):
		return self._index

	def setIndex(self, index):
		self._index = index

	def getCount(self):
		return self._count

	def setCount(self, count):
		self._count = count

	def getUUID(self):
		return self._uuid

	def setUUID(self, uuid):
		self._uuid = uuid

	def toString(self):
		return '[Count: %i UUID: %s Index: %i ID: %i Name: %s MaxStack: %i Icon: %s]' % (self._count, self._uuid, self._index, self._id, self._name, self._maxStack, self._icon)