# -*- coding: utf-8 -*-
#
"""
"""

#import Ouroboros
import GlobalDefine

g_itemCastObject_maps = {}


class ItemCastObject:
	"""
	"""
	_c_type = GlobalDefine.ITEM_OBJECT_TYPE_UNKNOWN

	def __init__(self, obj):
		"""
		"""
		super(ItemCastObject, self).__init__()
		self.realobj = obj

	def getObject(self):
		return self.realobj

	def getObjectReal(self):
		return self.realobj

	def getReference(self, entity):
		return entity

	def getID(self):
		return 0

	@classmethod
	def getType(SELF):
		return SELF._c_type

	@classmethod
	def isType(SELF, otype):
		return SELF._c_type == otype

	def asDict(self):
		return {
			"type": self.getType(),
			"param": self.realobj,
		}


class ItemCastEntity(ItemCastObject):
	"""
	"""
	_c_type = GlobalDefine.ABILITY_OBJECT_TYPE_ENTITY

	def __init__(self, obj):
		"""
		"""
		super(ItemCastEntity, self).__init__(obj)

	def getObject(self):
		return Ouroboros.entities.get(self.realobj)

	def getReference(self, entity):
		return self.getObject()

	def getID(self):
		return self.realobj

g_itemCastObject_maps[GlobalDefine.ITEM_OBJECT_TYPE_ENTITY] = ItemCastEntity


def createItemCastEntity(entity):
	return ItemCastEntity(entity.id)


def createItemCastEntityByID(entityID):
	return ItemCastEntity(entityID)


def createItemCastObject(type, obj):
	return g_itemCastObject_maps[type](obj)
