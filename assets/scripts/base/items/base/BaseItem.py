# -*- coding: utf-8 -*-

from itembases.ItemObject import ItemObject

class BaseItem(ItemObject):
	def __init__(self):
		ItemObject.__init__(self)

	def loadFromDict(self, dictDatas):
		"""
		virtual method.
		Create this object from the dictionary
		Runs when all Items are added to the from their files on init
		"""
		ItemObject.loadFromDict(self, dictDatas)