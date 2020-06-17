# -*- coding: utf-8 -*-
import Ouroboros
import time

from OURODebug import *

from interfaces.GameObject import GameObject
from interfaces.Teleport import Teleport
from interfaces.Chat import Chat
from interfaces.Social import Social
from interfaces.GameMaster import GameMaster

class Avatar(Ouroboros.Proxy,
			 GameObject,
			 Teleport,
			 Chat,
			 Social,
			 GameMaster):
	"""
	Entity Role
	"""
	def __init__(self):
		Ouroboros.Proxy.__init__(self)
		GameObject.__init__(self)
		Teleport.__init__(self)
		Chat.__init__(self)
		Social.__init__(self)
		GameMaster.__init__(self)

		self.accountEntity = None
		self.cellData["dbid"] = self.databaseID
		self.playerName = self.cellData["name"]
		self.spaceUTypeB = self.cellData["spaceUType"]

		self._destroyTimer = 0

		#self.inventory = InventoryMgr(self)

	def onClientEnabled(self):
		"""
		Ouroboros method.
		The entity is officially activated to be usable.
		At this point, the entity has established the client corresponding entity,
		and its cell part can be created here.
		"""
		INFO_MSG("Avatar[%i-%s] entities enable. spaceUTypeB=%s, entityCall:%s" % (self.id, self.playerName, self.spaceUTypeB, self.client))
		Teleport.onClientEnabled(self)

		if self._destroyTimer > 0:
			self.delTimer(self._destroyTimer)
			self._destroyTimer = 0

	def onClientDeath(self):
		"""
		Ouroboros method.
		Entity lost client entity
		"""
		DEBUG_MSG("Avatar[%i].onClientDeath:" % self.id)
		self.destroySelf()

	def destroySelf(self):
		"""
		"""
		if self.client is not None:
			return

		if self.cell is not None:
			# Destroy the cell entity
			self.destroyCellEntity()
			return

		# If the account ENTITY exists, it is also notified to destroy it.
		if self.accountEntity != None:
			if time.time() - self.accountEntity.relogin > 1:
				self.accountEntity.destroy()
			else:
				DEBUG_MSG("Avatar[%i].destroySelf: relogin =%i" % (self.id, time.time() - self.accountEntity.relogin))

		# Destroy base
		if not self.isDestroyed:
			self.destroy()

	def onDestroy(self):
		"""
		Ouroboros method.
		Entity destruction
		"""
		DEBUG_MSG("Avatar::onDestroy: %i." % self.id)

		if self.accountEntity != None:
			self.accountEntity.activeAvatar = None
			self.accountEntity = None

	def onGetCell(self):
		"""
		Ouroboros method.
		The cell part of the entity was created successfully.
		"""
		DEBUG_MSG('Avatar::onGetCell: %s' % self.cell)

	def createCell(self, space):
		"""
		Defined method.
		Create a cell entity
		"""
		self.createCellEntity(space)
		DEBUG_MSG("Avatar::createCell")

	def updateProperties(self):
		avatarCell = self.cell
		avatarCell.resetProperties()
		#for key, info in self.equipItemList.items():
			#items.getItem(info[1]).use(self)

	def deactivate(self):
		'''
		Allows player to logout of their current avatar
		'''

		if self.cell is not None:
			# Destroy the cell entity
			self.destroyCellEntity()
		# Destroy base
		if not self.isDestroyed:
			self.destroy()

		DEBUG_MSG("Avatar::Deactivate")

	def getPlayerName(self):
		return self.playerName