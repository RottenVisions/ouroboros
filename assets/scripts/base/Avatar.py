# -*- coding: utf-8 -*-
import Ouroboros
import GlobalConst
import Helper

import time

from OURODebug import *

from interfaces.GameObject import GameObject
from interfaces.Teleport import Teleport
from interfaces.Chat import Chat
from interfaces.Social import Social
from interfaces.GameMaster import GameMaster

from ItemManagement import ItemManagement

import items

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

		self.setGlobalData()

		self._destroyTimer = 0

		size = GlobalConst.INVENTORY_BASE_SIZE
		if self.itemListSize != GlobalConst.INVENTORY_BASE_SIZE:
			size = self.itemListSize
		self.inventoryManagement = ItemManagement(self, GlobalConst.INVENTORY_TYPE_INVENTORY, size)
		self.equipInventoryManagement = ItemManagement(self, GlobalConst.INVENTORY_TYPE_EQUIPMENT, GlobalConst.INVENTORY_EQUIPMENT_BASE_SIZE)


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

	def setGlobalData(self):
		Helper.setAvatarGlobalProperty(self.id, 'roleType', self.roleType)
		Helper.setAvatarGlobalProperty(self.id, 'playerName', self.playerName)

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

	# BASIC
	def setInventorySize(self, size):
		# Should we put something here to prevent inventory from being less than items count ?
		# as this will delete players items if reducing to smaller inventory size
		if size > GlobalConst.INVENTORY_SIZE_MAX:
			self.itemListSize = GlobalConst.INVENTORY_SIZE_MAX
			self.inventoryManagement.reInitialize(self.itemListSize)
			return False
		if size < 1:
			self.itemListSize = 1
			self.inventoryManagement.reInitialize(self.itemListSize)
			return False
		self.itemListSize = size
		self.inventoryManagement.reInitialize(self.itemListSize)
		return True

	# OTHER
	def addExperience(self, amount):
		if self.currency + amount >= GlobalConst.GC_EXPERIENCE_CAP:
			self.currency = GlobalConst.GC_EXPERIENCE_CAP
			return
		self.experience += amount

	def removeExperience(self, amount):
		if self.experience - amount <= 0:
			self.experience = 0
			return
		self.experience -= amount

	def addCurrency(self, amount):
		if self.currency + amount >= GlobalConst.GC_CURRENCY_CAP:
			self.currency = GlobalConst.GC_CURRENCY_CAP
			return
		self.currency += amount

	def removeCurrency(self, amount):
		if self.currency - amount <= 0:
			self.currency = 0
			return
		self.currency -= amount

	# Requests

	def reqExperience(self):
		if self.client:
			self.client.onReqExperience(self.experience)

	def reqCurrency(self):
		if self.client:
			self.client.onReqCurrency(self.currency)

	# ITEMS
	def reqItemList(self):
		if self.client:
			self.client.onReqItemList(self.itemList, self.equipItemList)

	def reqItemListSize(self):
		if self.client:
			self.client.onReqItemListSize(self.itemListSize)

	def swapItemRequest(self, sourceIndex, destinationIndex):
		DEBUG_MSG(self.inventoryManagement.getInventoryAsString())
		success = self.inventoryManagement.moveItemTo(sourceIndex, destinationIndex)
		if self.client:
			self.client.onReqItemList(self.itemList, self.equipItemList)

	def rejectDropItemRequest(self, sourceIndex, destinationIndex):
		if self.client:
			self.client.onReqItemList(self.itemList, self.equipItemList)

	def splitItemRequest(self, itemIndex, amount):
		result = self.inventoryManagement.splitItemStack(itemIndex, amount)
		if result[0] is GlobalConst.INVENTORY_OPERATION_OK and result[1] is GlobalConst.INVENTORY_OPERATION_OK:
			if self.client:
				self.client.onReqItemList(self.itemList, self.equipItemList)
				DEBUG_MSG(
					'Split %i %s to %s inventory.' % (amount, self.inventoryManagement.getItemAtIndex(itemIndex).getName(), self.playerName))
		else:
			DEBUG_MSG('Failed to split %i %i to %s inventory' % (amount, itemIndex, self.playerName))
			DEBUG_MSG('Result: ', result)

	def useItemRequest(self, itemIndex, amount):
		if self.inventoryManagement.getItemAtIndex(itemIndex) != -1:
			saveName = self.inventoryManagement.getItemAtIndex(itemIndex).getName()
		result = self.inventoryManagement.useItem(itemIndex, amount, self)
		if result[0] is GlobalConst.INVENTORY_OPERATION_OK:
			if self.client:
				self.client.onReqItemList(self.itemList, self.equipItemList)
				DEBUG_MSG(
					'Used %s to %s inventory.' % (
					saveName, self.playerName))
		else:
			DEBUG_MSG('Failed to use %i to %s inventory' % (itemIndex, self.playerName))
			DEBUG_MSG('Result: ', result)

	def quickSplitItemRequest(self, itemIndex):
		result = self.inventoryManagement.quickSplitItemStack(itemIndex)
		if result[0] is GlobalConst.INVENTORY_OPERATION_OK and result[1] == -1:
			if self.client:
				self.client.onReqItemList(self.itemList, self.equipItemList)
				DEBUG_MSG(
					'Split in half %s to %s inventory.' % (
					self.inventoryManagement.getItemAtIndex(itemIndex).getName(), self.playerName))
		else:
			DEBUG_MSG('Failed to split in half %i to %s inventory' % (itemIndex, self.playerName))
			DEBUG_MSG('Result: ', result)

	def deleteItemRequest(self, itemIndex):
		self.inventoryManagement.removeItemAtIndex(itemIndex)
		if self.client:
			self.client.onReqItemList(self.itemList, self.equipItemList)

	def equipItemRequest(self, itemIndex):
		if self.inventoryManagement.getItemAtIndex(itemIndex) != -1:
			saveName = self.inventoryManagement.getItemAtIndex(itemIndex).getName()
		result = self.inventoryManagement.equipItem(itemIndex, self)
		if result[0] is GlobalConst.INVENTORY_OPERATION_OK and result[1] == -1:
			if self.client:
				self.client.onReqItemList(self.itemList, self.equipItemList)
				DEBUG_MSG(
					'Equipped %s for %s.' % (
						saveName, self.playerName))
		else:
			DEBUG_MSG('Failed to equip %i for %s' % (itemIndex, self.playerName))
			DEBUG_MSG('Result: ', result)

	def addItem(self, itemID, count=1, itemIndex=-1):
		if itemIndex != -1:
			result = self.inventoryManagement.addItemToIndex(itemID, itemIndex, count)
		else:
			result = self.inventoryManagement.addItem(itemID, count)
		if result is GlobalConst.INVENTORY_OPERATION_OK:
			if self.client:
				self.client.onReqItemList(self.itemList, self.equipItemList)
				DEBUG_MSG('Added %i %s to %s inventory.' % (count, items.getItemByID(itemID).getName(), self.playerName))
		else:
			DEBUG_MSG('Failed to add %i %i to %s inventory' % (count, itemID, self.getPlayerName()))
			DEBUG_MSG('Result: ', result)

	def removeItem(self, itemID, itemIndex=-1, count=1):
		if itemIndex != -1:
			result = self.inventoryManagement.removeItemAtIndex(itemIndex, count)
		else:
			result = self.inventoryManagement.removeItem(itemID, count)
		if result is GlobalConst.INVENTORY_OPERATION_OK:
			if self.client:
				self.client.onReqItemList(self.itemList, self.equipItemList)
				DEBUG_MSG('Removed %i %s to %s inventory.' % (count, items.getItemByID(itemID).getName(), self.playerName))
		else:
			DEBUG_MSG('Failed to remove %i %i to %s inventory' % (count, itemID, self.playerName))
			DEBUG_MSG('Result: ', result)

	def clearInventory(self):
		self.inventoryManagement.clearAllInventory()
		if self.client:
			self.client.onReqItemList(self.itemList, self.equipItemList)


	def toString(self):
		DEBUG_MSG('Level: %i Currency: %i AP: %i Experience: %i ' % (self.level, self.currency, self.abilityPoints, self.experience))