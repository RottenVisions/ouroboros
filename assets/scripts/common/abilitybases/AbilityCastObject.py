# -*- coding: utf-8 -*-
#
"""
"""

import Ouroboros
import GlobalDefine

g_abilityCastObject_maps = {}


class AbilityCastObject:
	"""
	"""
	_c_type = GlobalDefine.ABILITY_OBJECT_TYPE_UNKNOWN

	def __init__(self, obj):
		"""
		"""
		super(AbilityCastObject, self).__init__()
		self.realobj = obj

	def getObject(self):
		return self.realobj

	def getObjectReal(self):
		return self.realobj

	def getPosition(self):
		return (0, 0, 0)

	def getDirection(self):
		return (0, 0, 0)

	def getReference(self, entity):
		return entity

	def distToDelay(self, travelSpeed, position):
		return 0.0

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


class AbilityCastEntity(AbilityCastObject):
	"""
	"""
	_c_type = GlobalDefine.ABILITY_OBJECT_TYPE_ENTITY

	def __init__(self, obj):
		"""
		"""
		super(AbilityCastEntity, self).__init__(obj)

	def getObject(self):
		return Ouroboros.entities.get(self.realobj)

	def getPosition(self):
		entity = self.getObject()
		if not entity:
			return (0, 0, 0)
		return entity.position

	def getDirection(self):
		entity = self.getObject()
		if not entity:
			return (0, 0, 0)
		return entity.direction

	def getReference(self, entity):
		return self.getObject()

	def getID(self):
		return self.realobj

	def distToDelay(self, travelSpeed, position):
		"""
		At least 1m/s, less than 1m/sec is treated as instant processing
		"""
		entity = self.getObject()
		if entity:
			if travelSpeed > 1.0:
				return position.distTo(entity.position) / travelSpeed
		return 0.0


class AbilityCastPosition(AbilityCastObject):
	"""
	"""
	_c_type = GlobalDefine.ABILITY_OBJECT_TYPE_POSITION

	def __init__(self, obj):
		"""
		"""
		super(AbilityCastPosition, self).__init__(obj)

	def getPosition(self):
		return self.realobj

	def getDirection(self):
		return (0, 0, 0)

	def getReference(self, entity):
		return entity

	def distToDelay(self, travelSpeed, position):
		"""
		At least 1m/s, less than 1m/sec is treated as instant processing
		"""
		if travelSpeed > 1.0:
			return position.flatDistTo(self.realobj) / travelSpeed
		return 0.0


g_abilityCastObject_maps[GlobalDefine.ABILITY_OBJECT_TYPE_ENTITY] = AbilityCastEntity
g_abilityCastObject_maps[GlobalDefine.ABILITY_OBJECT_TYPE_POSITION] = AbilityCastPosition


def createAbilityCastEntity(entity):
	return AbilityCastEntity(entity.id)


def createAbilityCastEntityByID(entityID):
	return AbilityCastEntity(entityID)


def createAbilityCastPosition(position):
	return AbilityCastPosition(position)


def createAbilityCastObject(type, obj):
	return g_abilityCastObject_maps[type](obj)
