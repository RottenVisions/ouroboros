# Defines the file's encoding
# coding: utf8
# Introducing the Ouroboros module
# import Ouroboros is a module of the engine,
# and almost all engine APIs are in the module.
import Ouroboros
# It OURODebug is an auxiliary module for more convenient output of logs
from OURODebug import *

class FirstEntity(Ouroboros.Entity):
	"""
	Implementation of the base part of the first entity
	"""
	def __init__(self):
		Ouroboros.Entity.__init__(self)

	# First, use the name defined in def as the method name.
	# The number of parameters must be the same as defined in def.
	# Here, the parameter content is the received hello content
	def hello(self, content):
		"""
		Say Hello
		:param name: content
		:return:
		"""
		# Log output: We see self.name that this is exactly the same as the property name we declared in the def.
		INFO_MSG("Hello Ouroboros! I'm %s. I got your content=%s" %(self.name, content))
		"""
		The attributes declared in the def can be. Base attribute obtained and set directly on the Entity.
		At the same time, depending on FLAGS , its scope is also different. Since our name scope is
		on the Base, we can use self.name - the name of our entity here.
		Of course, it can also be set by self.name = "new name"means, and the type must be consistent.
		"""

	def createCell(self, sceneCall):
		"""
		Create cell part
		:param sceneCall: scene's cell entityCall
		:return:
		"""
		# API:  the cell part of the entity
		# This method is Entityan API for sceneCell creating an associated entity in the space,
		# in other words, which space to put into.
		self.createCellEntity(sceneCall)

	def onGetCell(self):
		"""
		The cell part of the entity was created successfully.
		:return:
		"""

		props = {
			"name": "MyFirstEntity"
		}
	#	# Among them, onGetCellis the system callback function, which will be called when the
		# cell part of the Scene entity is successfully created.
		# At this time, the FirstEntity can be put into the space at the first time.
		entity = Ouroboros.createEntityLocally("FirstEntity", props)
		# Call loginToScene to log the FirstEntity entity into the space.
		self.loginToScene(entity)
