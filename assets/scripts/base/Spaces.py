# -*- coding: utf-8 -*-
import Ouroboros
import Functor
import data_spaces
import ServerConstantsDefine
import Watcher
from OURODebug import *
from SpaceAllocator import *
from interfaces.GameObject import GameObject

class Spaces(Ouroboros.Entity,
			 GameObject):
	"""
	This is a space manager for script layer encapsulation
	Ouroboros's space is an abstract space concept, a space can be seen by the script layer as a game scene, a game room, or even a universe.
	"""
	def __init__(self):
		Ouroboros.Entity.__init__(self)
		GameObject.__init__(self)

		# Initial space allocator
		self.initAlloc()

		# Register this manager's entityCall with global shared data for easy access in all logical processes
		Ouroboros.globalData["Spaces"] = self
		DEBUG_MSG("Spaces::init")

	def initAlloc(self):
		# Register a timer in which we create some NPCs every cycle until we have created all
		self._spaceAllocs = {}
		self.addTimer(3, 1, ServerConstantsDefine.TIMER_TYPE_CREATE_SPACES)

		self._tmpDatas = list(data_spaces.data.keys())
		for utype in self._tmpDatas:
			spaceData = data_spaces.data.get(utype)
			if spaceData["entityType"] == "SpaceDuplicate":
				self._spaceAllocs[utype] = SpaceAllocDuplicate(utype)
			else:
				self._spaceAllocs[utype] = SpaceAlloc(utype)
		DEBUG_MSG("Spaces::initAlloc")

	def getSpaceAllocs(self):
		return self._spaceAllocs

	def createSpaceOnTimer(self, tid):
		"""
		Create space
		"""
		if len(self._tmpDatas) > 0:
			spaceUType = self._tmpDatas.pop(0)
			self._spaceAllocs[spaceUType].init()

		if len(self._tmpDatas) <= 0:
			del self._tmpDatas
			self.delTimer(tid)

	def loginToSpace(self, avatarEntity, spaceUType, context):
		"""
		defined method.
		A player requests to log in to a space
		"""
		self._spaceAllocs[spaceUType].loginToSpace(avatarEntity, context)

	def logoutSpace(self, avatarID, spaceKey):
		"""
		defined method.
		A player requests to log out of this space
		"""
		for spaceAlloc in self._spaceAllocs.values():
			space = spaceAlloc.getSpaces().get(spaceKey)
			if space:
				space.logoutSpace(avatarID)

	def teleportSpace(self, entityCall, spaceUType, position, direction, context):
		"""
		defined method.
		Request to enter a space
		"""
		self._spaceAllocs[spaceUType].teleportSpace(entityCall, position, direction, context)

	#--------------------------------------------------------------------------------------------
	#                              Callbacks
	#--------------------------------------------------------------------------------------------
	def onTimer(self, tid, userArg):
		"""
		Ouroboros method.
		Engine callback timer trigger
		"""
		#DEBUG_MSG("%s::onTimer: %i, tid:%i, arg:%i" % (self.getScriptName(), self.id, tid, userArg))
		if ServerConstantsDefine.TIMER_TYPE_CREATE_SPACES == userArg:
			self.createSpaceOnTimer(tid)

		GameObject.onTimer(self, tid, userArg)

	def onSpaceLoseCell(self, spaceUType, spaceKey):
		"""
		defined method.
		The space cell is created.
		"""
		self._spaceAllocs[spaceUType].onSpaceLoseCell(spaceKey)

	def onSpaceGetCell(self, spaceUType, spaceEntityCall, spaceKey):
		"""
		defined method.
		The space cell is created.
		"""
		self._spaceAllocs[spaceUType].onSpaceGetCell(spaceEntityCall, spaceKey)
