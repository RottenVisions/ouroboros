# -*- coding: utf-8 -*-
#
"""
"""

import Ouroboros
import GlobalDefine

g_auraCastObject_maps = {}


class AuraCastObject:
	"""
	"""
	_c_type = GlobalDefine.AURA_OBJECT_TYPE_UNKNOWN

	def __init__(self, obj):
		"""
		"""
		super(AuraCastObject, self).__init__()
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


class AuraCastEntity(AuraCastObject):
	"""
	"""
	_c_type = GlobalDefine.AURA_OBJECT_TYPE_ENTITY

	def __init__(self, obj):
		"""
		"""
		super(AuraCastEntity, self).__init__(obj)

	def getObject(self):
		return Ouroboros.entities.get(self.realobj)

	def getReference(self, entity):
		return self.getObject()

	def getID(self):
		return self.realobj

g_auraCastObject_maps[GlobalDefine.AURA_OBJECT_TYPE_ENTITY] = AuraCastEntity

def createAuraCastEntity(entity):
	return AuraCastEntity(entity.id)

def createAuraCastEntityByID(entityID):
	return AuraCastEntity(entityID)

def createAuraCastObject(type, obj):
	return g_auraCastObject_maps[type](obj)
