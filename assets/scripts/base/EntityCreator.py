#coding: utf-8

import Ouroboros
from OURODebug import *

class EntityCreator:

	def CreateBaseEntities(self):
		# Set the properties of the entity to be created
		props = {
			"name": "MyFirstEntity"
		}
		# Create FirstEntity
		Ouroboros.createEntityLocally("FirstEntity", props)
		INFO_MSG("I was called")
		