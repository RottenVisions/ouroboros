# -*- coding: utf-8 -*-
import Ouroboros
from OURODebug import *
import data_spaces

class SpaceContext(dict):
	"""
	Generate space context
	"""
	def __init__(self, entity):
		pass

	@staticmethod
	def create(entity):
		return {}


class SpaceDuplicateContext(SpaceContext):
	"""
	The context in which the copy of the space is generated
	To enter the copy you need to hold the key (spaceKey)
	"""
	def __init__(self, entity):
		SpaceContext.__init__(self, entity)

	@staticmethod
	def create(entity):
		return {"spaceKey" : entity.dbid}

def createContext(entity, spaceUType):
	"""
	"""
	spaceData = data_spaces.data.get(spaceUType)

	return {
		"Space" : SpaceContext,
		"SpaceDuplicate" : SpaceDuplicateContext,
	}[spaceData["entityType"]].create(entity)
