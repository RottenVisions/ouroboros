# -*- coding: utf-8 -*-
import Ouroboros
import Functor
from OURODebug import *
import data_entities
import data_spaces
import copy

CONST_WAIT_CREATE = -1

class SpaceAlloc:
	"""
	Ordinary scene allocator
	"""
	def __init__(self, utype):
		self._spaces = {}
		self._utype = utype
		self._pendingLogonEntities = {}
		self._pendingEnterEntityMBs = {}

	def init(self):
		"""
		virtual method.
		"""
		self.createSpace(0, {})

	def getSpaces(self):
		return self._spaces

	def createSpace(self, spaceKey, context):
		"""
		"""
		if spaceKey <= 0:
			spaceKey = Ouroboros.genUUID64()

		context = copy.copy(context)
		spaceData = data_spaces.data.get(self._utype)
		Ouroboros.createEntityAnywhere(spaceData["entityType"], \
											{"spaceUType" : self._utype,	\
											"spaceKey" : spaceKey,	\
											"context" : context,	\
											}, \
											Functor.Functor(self.onSpaceCreatedCB, spaceKey))
		DEBUG_MSG("Spaces::createSpace %s", spaceData)

	def onSpaceCreatedCB(self, spaceKey, space):
		"""
		Callback after a space is created
		"""
		DEBUG_MSG("Spaces::onSpaceCreatedCB: space %i. entityID=%i" % (self._utype, space.id))

	def onSpaceLoseCell(self, spaceKey):
		"""
		The space cell is created.
		"""
		del self._spaces[spaceKey]
		DEBUG_MSG("SpaceAllocator::onSpaceLoseCell")

	def onSpaceGetCell(self, spaceEntityCall, spaceKey):
		"""
		The space cell is created.
		"""
		DEBUG_MSG("Spaces::onSpaceGetCell: space %i. entityID=%i, spaceKey=%i" % (self._utype, spaceEntityCall.id, spaceKey))
		self._spaces[spaceKey] = spaceEntityCall

		pendingLogonEntities = self._pendingLogonEntities.pop(spaceKey, [])
		pendingEnterEntityMBs = self._pendingEnterEntityMBs.pop(spaceKey, [])

		for e, context in pendingLogonEntities:
			self.loginToSpace(e, context)

		for mb, pos, dir, context in pendingEnterEntityMBs:
			self.teleportSpace(mb, pos, dir, context)

	def alloc(self, context):
		"""
		virtual method.
		Assign a space
		"""
		if self._spaces == {}:
			return None

		return list(self._spaces.values())[0]

	def loginToSpace(self, avatarEntity, context):
		"""
		virtual method.
		A player requests to log in to a space
		"""
		spaceKey = context.get("spaceKey", 0)
		space = self.alloc({"spaceKey" : spaceKey})
		if space is None:
			ERROR_MSG("Spaces::loginToSpace: not found space %i. login to space is failed! spaces=%s" % (self._utype, self._spaces))
			return

		if space == CONST_WAIT_CREATE:
			if spaceKey not in self._pendingLogonEntities:
				self._pendingLogonEntities[spaceKey] = [(avatarEntity, context)]
			else:
				self._pendingLogonEntities[spaceKey].append((avatarEntity, context))

			DEBUG_MSG("Spaces::loginToSpace: avatarEntity=%s add pending." % avatarEntity.id)
			return

		DEBUG_MSG("Spaces::loginToSpace: avatarEntity=%s" % avatarEntity.id)
		space.loginToSpace(avatarEntity, context)

	def teleportSpace(self, entityCall, position, direction, context):
		"""
		virtual method.
		Request to enter a space
		"""
		space = self.alloc(context)
		if space is None:
			ERROR_MSG("Spaces::teleportSpace: not found space %i. login to space is failed!" % self._utype)
			return

		if space == CONST_WAIT_CREATE:
			spaceKey = context.get("spaceKey", 0)
			if spaceKey not in self._pendingEnterEntityMBs:
				self._pendingEnterEntityMBs[spaceKey] = [(entityCall, position, direction, context)]
			else:
				self._pendingEnterEntityMBs[spaceKey].append((entityCall, position, direction, context))

			DEBUG_MSG("Spaces::teleportSpace: avatarEntity=%s add pending." % entityCall.id)
			return

		DEBUG_MSG("Spaces::teleportSpace: entityCall=%s" % entityCall)
		space.teleportSpace(entityCall, position, direction, context)

class SpaceAllocDuplicate(SpaceAlloc):
	"""
	Copy distributor
	"""
	def __init__(self, utype):
		SpaceAlloc.__init__(self, utype)

	def init(self):
		"""
		virtual method.
		"""
		pass # The copy does not need to be initialized to create one

	def alloc(self, context):
		"""
		virtual method.
		Assign a space
		For a copy, creating a copy will use the player's dbid as the key for the space.
		Anyone who wants to get into this copy needs to know this key.
		"""
		spaceKey = context.get("spaceKey", 0)
		space = self._spaces.get(spaceKey)

		assert spaceKey != 0

		if space is None:
			self.createSpace(spaceKey, context)
			return CONST_WAIT_CREATE

		return space
