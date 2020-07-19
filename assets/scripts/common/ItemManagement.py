# -*- coding: utf-8 -*-
import Ouroboros
import items
import GlobalConst
import weakref
import math
import GlobalDefine
import ServerConstantsDefine
from OURODebug import DEBUG_MSG

import data_items
from ITEM_INFO import TItemInfo
from ITEM_INFO import TItemInfoList

class ItemManagement:

	def __init__(self, entity, invType, size):
		self._entity = weakref.proxy(entity)
		self.inventory = []
		self.currentSize = size
		self.invType = invType

		if self.invType is GlobalConst.INVENTORY_TYPE_INVENTORY:
			self.currentEntityItemListInfo = self._entity.itemList
			self.currentEntityItemList = self._entity.itemList.items()
		if self.invType is GlobalConst.INVENTORY_TYPE_EQUIPMENT:
			self.currentEntityItemListInfo = self._entity.equipItemList
			self.currentEntityItemList = self._entity.equipItemList.items()

		self.initialize()

	def initialize(self):
		self.inventory = []
		self.setSize(self.currentSize)

		# Initialize the backpack index to Uid
		self.invIndex2Uids = [-1] * self.currentSize
		for key, info in self.currentEntityItemList:
			self.invIndex2Uids[info[3]] = key

		# Convert saved items to item object list
		self.setInventoryFromOuroItemList()

	def reInitialize(self, newSize = -1):
		sizeToUse = newSize
		if newSize == -1:
			sizeToUse = self.currentSize
		self.currentSize = sizeToUse
		# Remove excess items first
		self.setOuroItemListFromNewSize(self.currentSize)
		# Now initialize
		self.initialize()

	def addItem(self, id, count=1):
		remainingCount = count
		itemAdded = False
		addItemResult = None
		if items.getItemByID(id) is None:
			return GlobalConst.INVENTORY_OPERATION_ITEM_NONEXISTENT
		completelyFull = self.isInventoryCompletelyFull()
		if completelyFull is GlobalConst.INVENTORY_OPERATION_COMPLETELY_FULL:
			return completelyFull
		for i in range(len(self.inventory)):
			item = self.inventory[i]
			# Blank space
			blankSpaceResult = self.isSpaceEmpty(i)
			# Operations are finished, No items left to add
			if remainingCount == 0:
				break
			# Blank Space
			if blankSpaceResult is GlobalConst.INVENTORY_OPERATION_OK:
				# Create new item in blank space
				newItem = items.getItemByID(id)
				amountToAdd = remainingCount
				if remainingCount > newItem.getMaxStack():
					amountToAdd = newItem.getMaxStack()
				addItemResult, updatedBlankItem = self.addItemToIndex(id, i, amountToAdd)
				# Catch any errors occuring in this procedure
				if addItemResult is not GlobalConst.INVENTORY_OPERATION_OK:
					return addItemResult
				self.inventory[i] = updatedBlankItem
				self.setItemInOuroItemList(updatedBlankItem, i)
				remainingCount -= amountToAdd
				if addItemResult is GlobalConst.INVENTORY_OPERATION_OK:
					itemAdded = True
			# Non-Blank Space
			elif blankSpaceResult is not GlobalConst.INVENTORY_OPERATION_OK and item.getID() is id and remainingCount > 0:
				amountCanAdd = item.getMaxStack() - item.getCount()
				amountToAdd = remainingCount
				if remainingCount > amountCanAdd:
					amountToAdd = amountCanAdd
				finalAmountToAdd = amountToAdd + item.getCount()
				# Get updated item
				addItemResult, updatedItem = self.updateItem(item, i, finalAmountToAdd)
				# Update failed, discontinue add item procedure
				if addItemResult is not GlobalConst.INVENTORY_OPERATION_OK:
					break
				# Set updated item
				self.inventory[i] = updatedItem
				self.setItemInOuroItemList(updatedItem, i)
				remainingCount -= amountToAdd
				if addItemResult is GlobalConst.INVENTORY_OPERATION_OK:
					itemAdded = True
			else:
				continue
		if itemAdded is True:
			return GlobalConst.INVENTORY_OPERATION_OK
		else:
			full = self.isInventoryFull()
			if full is GlobalConst.INVENTORY_OPERATION_FULL:
				return full
		return addItemResult

	def addItemToIndex(self, id, index, count=1):
		overflowed = False
		# Check parameters before initiation
		if self.countIsValid(items.getItemByID(id), count) is GlobalConst.INVENTORY_OPERATION_INVALID_COUNT:
			return GlobalConst.INVENTORY_OPERATION_INVALID_COUNT, None
		# Item exists in the space requested
		if not self.isSpaceEmpty(index):
			# item requested to be added is the same as the slot it is going in
			if self.inventory[index].getID() == id:
				# item is stackable
				if self.inventory[index].getStackable() or self.inventory[index].getMaxStack() > 0:
					newCount = count
					# Do not let the count overflow
					if count > self.inventory[index].getMaxStack():
						newCount = self.inventory[index].getMaxStack()
						overflowed = True
					updateResult, updatedItem = self.updateItem(self.inventory[index], index, newCount)
					self.inventory[index] = updatedItem
					self.setItemInOuroItemList(updatedItem, index)
					return updateResult, updatedItem

		item = items.getNewItemByID(id)

		if item is None:
			return GlobalConst.INVENTORY_OPERATION_ITEM_NONEXISTENT, None

		if self.indexIsValid(index) is GlobalConst.INVENTORY_OPERATION_INVALID_INDEX:
			return GlobalConst.INVENTORY_OPERATION_INVALID_INDEX, None
		setupResult, setupItem = self.setupItem(item, index, count)
		if overflowed:
			setupResult = GlobalConst.INVENTORY_OPERATION_OVERFLOWED
		self.inventory[index] = setupItem
		self.setItemInOuroItemList(setupItem, index)
		return setupResult, setupItem

	def removeItem(self, id, count=1):
		remainingCount = count
		itemRemoved = False
		removeItemResult = GlobalConst.INVENTORY_OPERATION_ERROR
		for i in range(len(self.inventory)):
			item = self.inventory[i]
			# Blank space
			blankSpaceResult = self.isSpaceEmpty(i)
			# Operations are finished, No items left to add
			if remainingCount == 0:
				break
			# Non-Blank Space
			if blankSpaceResult is not GlobalConst.INVENTORY_OPERATION_OK and item.getID() is id:
				amountCanRemove = item.getCount()
				amountToRemove = remainingCount
				if remainingCount > amountCanRemove:
					amountToRemove = amountCanRemove
				# Get updated item
				removeItemResult, updatedItem = self.removeItemAtIndex(i, amountToRemove)
				# Update failed, discontinue add item procedure
				if removeItemResult is not GlobalConst.INVENTORY_OPERATION_OK:
					break
				# Set updated item
				self.inventory[i] = updatedItem
				self.setItemInOuroItemList(updatedItem, i)
				remainingCount -= amountToRemove
				if removeItemResult is GlobalConst.INVENTORY_OPERATION_OK:
					itemRemoved = True
			else:
				continue
		if itemRemoved is True:
			return GlobalConst.INVENTORY_OPERATION_OK
		return removeItemResult

	def removeItemAtIndex(self, index, count=1):
		# Check parameters before initiation
		if self.indexIsValid(index) is GlobalConst.INVENTORY_OPERATION_INVALID_INDEX:
			return GlobalConst.INVENTORY_OPERATION_INVALID_INDEX, None
		# Declare item here before count because count is valid needs it
		item = self.inventory[index]
		if self.countIsValid(item, count) is GlobalConst.INVENTORY_OPERATION_INVALID_COUNT:
			return GlobalConst.INVENTORY_OPERATION_INVALID_COUNT, None

		removalAmount = count
		# Do not exceed the actual items count
		if count > item.getCount():
			removalAmount = item.getCount()
		updatedAmount = item.getCount() - removalAmount
		updateResult, updatedItem = self.updateItem(self.inventory[index], index, updatedAmount)
		self.inventory[index] = updatedItem
		self.setItemInOuroItemList(updatedItem, index)
		return updateResult, updatedItem

	def moveItemTo(self, sourceIndex, destinationIndex):
		# Check if the source is empty
		emptySourceResult = self.isSpaceEmpty(sourceIndex)
		if emptySourceResult is GlobalConst.INVENTORY_OPERATION_OK:
			return GlobalConst.INVENTORY_OPERATION_EMPTY_SOURCE

		tempSourceItem = self.inventory[sourceIndex]

		emptyDestinationResult = self.isSpaceEmpty(destinationIndex)
		# Moving to an empty destination
		if emptyDestinationResult is GlobalConst.INVENTORY_OPERATION_OK:
			updateDestinationResult, updatedDestinationItem = self.moveItem(
				tempSourceItem, destinationIndex, tempSourceItem.getCount())
			updateSourceResult, updatedSourceItem = self.updateItem(
				tempSourceItem, sourceIndex, 0)
			self.inventory[destinationIndex] = updatedDestinationItem
			self.inventory[sourceIndex] = updatedSourceItem
			self.setItemInOuroItemList(updatedDestinationItem, destinationIndex)
			self.setItemInOuroItemList(updatedSourceItem, sourceIndex)
			print(444)
			return updateSourceResult, updateDestinationResult
		else:
			return self.swapItems(sourceIndex, destinationIndex)


	def swapItems(self, sourceIndex, destinationIndex):
		# Check if the source is empty
		emptySourceResult = self.isSpaceEmpty(sourceIndex)
		if emptySourceResult is GlobalConst.INVENTORY_OPERATION_OK:
			return GlobalConst.INVENTORY_OPERATION_EMPTY_SOURCE, None
		# Check if the destination is empty
		emptyDestinationResult = self.isSpaceEmpty(destinationIndex)
		if emptyDestinationResult is GlobalConst.INVENTORY_OPERATION_OK:
			return GlobalConst.INVENTORY_OPERATION_EMPTY_SOURCE, None

		# Check parameters before initiation
		if self.indexIsValid(sourceIndex) is GlobalConst.INVENTORY_OPERATION_INVALID_INDEX:
			return GlobalConst.INVENTORY_OPERATION_INVALID_INDEX, None
		if self.indexIsValid(destinationIndex) is GlobalConst.INVENTORY_OPERATION_INVALID_INDEX:
			return GlobalConst.INVENTORY_OPERATION_INVALID_INDEX, None

		tempSourceItem = self.inventory[sourceIndex]
		tempDestinationItem = self.inventory[destinationIndex]

		updateSourceResult = GlobalConst.INVENTORY_OPERATION_ERROR
		updateDestinationResult = GlobalConst.INVENTORY_OPERATION_ERROR

		# Same item detected for move
		if tempSourceItem.getID() == tempDestinationItem.getID():
			# Stackable item
			if tempDestinationItem.getStackable() and tempDestinationItem.getMaxStack() > 0:
				# Find the max count we can add
				maxCountCanAdd = tempDestinationItem.getMaxStack() - tempDestinationItem.getCount()
				# Source item has maximum stacks
				if tempSourceItem.getCount() == tempSourceItem.getMaxStack():
					updateSourceResult, updatedSourceItem = self.updateItem(tempSourceItem, sourceIndex,
																			tempDestinationItem.getCount())
					updateDestinationResult, updatedDestinationItem = self.updateItem(tempDestinationItem, destinationIndex,
																					  tempDestinationItem.getMaxStack())
				# Destination has max stacks
				elif tempDestinationItem.getCount() == tempDestinationItem.getMaxStack():
					updateSourceResult, updatedSourceItem = self.updateItem(tempSourceItem, sourceIndex,
																			tempSourceItem.getCount())
					updateDestinationResult, updatedDestinationItem = self.updateItem(tempDestinationItem, destinationIndex,
																					  tempDestinationItem.getCount())
				# Overflow will occur, so max out destination item
				# We do not use equal here because the first source item max stacks should catch that possibility
				elif tempSourceItem.getCount() > maxCountCanAdd:
					updateDestinationResult, updatedDestinationItem = self.updateItem(
						self.inventory[destinationIndex], sourceIndex, tempDestinationItem.getMaxStack())
					newSourceAmount = tempSourceItem.getCount() - maxCountCanAdd
					updateSourceResult, updatedSourceItem = self.updateItem(
						self.inventory[sourceIndex], destinationIndex, newSourceAmount)
					print(888)
				# Normal add to destination
				else:
					newDestinationAmount = tempDestinationItem.getCount() + tempSourceItem.getCount()
					updateDestinationResult, updatedDestinationItem = self.updateItem(
						tempDestinationItem, destinationIndex, newDestinationAmount)
					newSourceAmount = tempSourceItem.getCount() - maxCountCanAdd
					updateSourceResult, updatedSourceItem = self.updateItem(
						tempSourceItem, sourceIndex, 0)
					print(111)
				self.inventory[destinationIndex] = updatedDestinationItem
				self.inventory[sourceIndex] = updatedSourceItem
				self.setItemInOuroItemList(updatedDestinationItem, destinationIndex)
				self.setItemInOuroItemList(updatedSourceItem, sourceIndex)
				print(777)
			# Non stackable item
			else:
				# Simple swap
				updateSourceResult, updatedSourceItem = self.updateItem(tempSourceItem, destinationIndex, tempSourceItem.getCount())
				updateDestinationResult, updatedDestinationItem = self.updateItem(tempDestinationItem, sourceIndex, tempDestinationItem.getCount())
				self.inventory[destinationIndex] = updatedSourceItem
				self.inventory[sourceIndex] = updatedDestinationItem
				self.setItemInOuroItemList(updatedSourceItem, sourceIndex)
				self.setItemInOuroItemList(updatedDestinationItem, destinationIndex)
				print(666)
		# Different Items
		else:
			# Simple swap
			updateSourceResult, updatedSourceItem = self.updateItem(tempSourceItem, destinationIndex, tempSourceItem.getCount())
			updateDestinationResult, updatedDestinationItem = self.updateItem(tempDestinationItem, sourceIndex, tempDestinationItem.getCount())
			self.inventory[destinationIndex] = updatedSourceItem
			self.inventory[sourceIndex] = updatedDestinationItem
			self.setItemInOuroItemList(updatedSourceItem, sourceIndex)
			self.setItemInOuroItemList(updatedDestinationItem, destinationIndex)
			print(555)
		return updateSourceResult, updateDestinationResult

	def splitItemStack(self, index, amount, toFreeSpace=True):
		# Check if the source is empty
		emptySourceResult = self.isSpaceEmpty(index)
		if emptySourceResult is GlobalConst.INVENTORY_OPERATION_OK:
			return GlobalConst.INVENTORY_OPERATION_EMPTY_SOURCE, None

		# Check parameters before initiation
		if self.indexIsValid(index) is GlobalConst.INVENTORY_OPERATION_INVALID_INDEX:
			return GlobalConst.INVENTORY_OPERATION_INVALID_INDEX, None

		tempSourceItem = self.inventory[index]
		# Must have 2 or more in a stack to split
		if tempSourceItem.getCount() < 2:
			return GlobalConst.INVENTORY_OPERATION_INVALID_COUNT, None

		firstFreeSlotIndex = self.getFirstFreeSpace()

		updateSourceResult = GlobalConst.INVENTORY_OPERATION_ERROR
		updateDestinationResult = GlobalConst.INVENTORY_OPERATION_ERROR

		if amount > tempSourceItem.getCount():
			return GlobalConst.INVENTORY_OPERATION_INVALID_COUNT, None

		# Split
		item = items.getNewItemByID(tempSourceItem.getID())

		newSourceAmount = tempSourceItem.getCount() - amount
		updateSourceResult, updatedSourceItem = self.updateItem(tempSourceItem, index, newSourceAmount)
		if toFreeSpace:
			updateDestinationResult, updatedDestinationItem = self.setupItem(item, firstFreeSlotIndex, amount)
			self.inventory[index] = updatedSourceItem
			self.inventory[firstFreeSlotIndex] = updatedDestinationItem
			self.setItemInOuroItemList(updatedSourceItem, index)
			self.setItemInOuroItemList(updatedDestinationItem, firstFreeSlotIndex)
		else:
			return self.addItem(tempSourceItem.getID(), amount)
		return updateSourceResult, updateDestinationResult

	def quickSplitItemStack(self, index, useLowHalf=True, toFreeSpace=True):
		# Check if the source is empty
		emptySourceResult = self.isSpaceEmpty(index)
		if emptySourceResult is GlobalConst.INVENTORY_OPERATION_OK:
			return GlobalConst.INVENTORY_OPERATION_EMPTY_SOURCE, None

		# Check parameters before initiation
		if self.indexIsValid(index) is GlobalConst.INVENTORY_OPERATION_INVALID_INDEX:
			return GlobalConst.INVENTORY_OPERATION_INVALID_INDEX, None

		tempSourceItem = self.inventory[index]

		# Must have 2 or more in a stack to split
		if tempSourceItem.getCount() < 2:
			return GlobalConst.INVENTORY_OPERATION_INVALID_COUNT, None

		highHalf, lowHalf = self.getHalfStackSizes(tempSourceItem.getCount())

		amount = highHalf
		if useLowHalf:
			amount = lowHalf

		return self.splitItemStack(index, amount, toFreeSpace)

	def useItem(self, index, amount, user):
		# Check if the source is empty
		emptySourceResult = self.isSpaceEmpty(index)
		if emptySourceResult is GlobalConst.INVENTORY_OPERATION_OK:
			return GlobalConst.INVENTORY_OPERATION_EMPTY_SOURCE, None

		# Check parameters before initiation
		if self.indexIsValid(index) is GlobalConst.INVENTORY_OPERATION_INVALID_INDEX:
			return GlobalConst.INVENTORY_OPERATION_INVALID_INDEX, None

		tempSourceItem = self.inventory[index]

		if type(tempSourceItem) is not items.ConsumeItem:
			return GlobalConst.INVENTORY_ITEM_CAN_NOT_USE

		if tempSourceItem.canUse(user) is GlobalConst.GC_OK:
			tempSourceItem.use(user)
			return self.removeItemAtIndex(index, amount)
		else:
			return GlobalConst.INVENTORY_ITEM_CAN_NOT_USE

	def equipItem(self, index, user):
		# Check if the source is empty
		emptySourceResult = self.isSpaceEmpty(index)
		if emptySourceResult is GlobalConst.INVENTORY_OPERATION_OK:
			return GlobalConst.INVENTORY_OPERATION_EMPTY_SOURCE, None

		# Check parameters before initiation
		if self.indexIsValid(index) is GlobalConst.INVENTORY_OPERATION_INVALID_INDEX:
			return GlobalConst.INVENTORY_OPERATION_INVALID_INDEX, None

		tempSourceItem = self.inventory[index]

		print(type(tempSourceItem))
		if type(tempSourceItem) is not items.EquipItem:
			return GlobalConst.INVENTORY_ITEM_CAN_NOT_USE

		if tempSourceItem.canUse(user) is GlobalConst.GC_OK:
			tempSourceItem.use(user)
			# Move to another inventory (DO THIS)
			return self.removeItemAtIndex(index, tempSourceItem.getCount())
		else:
			return GlobalConst.INVENTORY_ITEM_CAN_NOT_EQUIP


	# Modifiers
	def setupItem(self, item, index, count):
		if item is None or item == -1:
			return GlobalConst.INVENTORY_OPERATION_ERROR, None
		if self.indexIsValid(index) is GlobalConst.INVENTORY_OPERATION_INVALID_INDEX:
			return GlobalConst.INVENTORY_OPERATION_INVALID_INDEX, None
		if self.countIsValid(item, count) is GlobalConst.INVENTORY_OPERATION_INVALID_COUNT:
			return GlobalConst.INVENTORY_OPERATION_INVALID_COUNT, None
		item.setCount(count)
		item.setIndex(index)
		item.setUUID(Ouroboros.genUUID64())
		return GlobalConst.INVENTORY_OPERATION_OK, item

	def updateItem(self, item, index, count):
		if item is None or item == -1:
			return GlobalConst.INVENTORY_OPERATION_ERROR, None
		if self.indexIsValid(index) is GlobalConst.INVENTORY_OPERATION_INVALID_INDEX:
			return GlobalConst.INVENTORY_OPERATION_INVALID_INDEX, None
		if self.countIsValid(item, count, True) is GlobalConst.INVENTORY_OPERATION_INVALID_COUNT:
			return GlobalConst.INVENTORY_OPERATION_INVALID_COUNT, None
		# Catch an item being deleted
		if count == 0:
			item = -1
		else:
			item.setCount(count)
			item.setIndex(index)
		return GlobalConst.INVENTORY_OPERATION_OK, item

	def moveItem(self, item, index, count):
		if item is None or item == -1:
			return GlobalConst.INVENTORY_OPERATION_ERROR, None
		if self.indexIsValid(index) is GlobalConst.INVENTORY_OPERATION_INVALID_INDEX:
			return GlobalConst.INVENTORY_OPERATION_INVALID_INDEX, None
		if self.countIsValid(item, count) is GlobalConst.INVENTORY_OPERATION_INVALID_COUNT:
			return GlobalConst.INVENTORY_OPERATION_INVALID_COUNT, None
		item.setCount(count)
		item.setIndex(index)
		return GlobalConst.INVENTORY_OPERATION_OK, item

	def setSize(self, size):
		for n in range(len(self.inventory), size):
			self.inventory.append(-1)

	def clear(self):
		for i in range(len(self.inventory)):
			self.inventory[i] = -1

	def clearAll(self):
		self.clear()
		self.clearItemList()

	def clearItemList(self):
		self.currentEntityItemListInfo = TItemInfoList()

	# Ouroboros Inventory
	def setItemInOuroItemList(self, item, index):
		itemInfo = TItemInfo()
		if item == -1:
			self.invIndex2Uids[index] = -1
			for key, info in list(self.currentEntityItemList):
				if info[3] == index:
					print('remove it', index)
					del self.currentEntityItemListInfo[key]
		else:
			itemInfo.extend([item.getUUID(), item.getID(), item.getCount(), item.getIndex()])
			self.invIndex2Uids[item.getIndex()] = item.getUUID()
			self.currentEntityItemListInfo[item.getUUID()] = itemInfo

	def setInventoryFromOuroItemList(self):
		for key, info in self.currentEntityItemList:
			item = items.getNewItemByID(info[1])  # 1 is the item ID
			setupResult, setupItem = self.setupItem(item, info[3], info[2])
			if setupResult == GlobalConst.INVENTORY_OPERATION_OK:
				self.inventory[info[3]] = setupItem

	def setOuroItemListFromNewSize(self, size):
		for key, info in list(self.currentEntityItemList):
			# Index is outside inventory bounds
			if info[3] > (size - 1):
				# Delete
				del self.currentEntityItemListInfo[key]

	def getItemsInOuroItemListAsString(self):
		length = len(self.currentEntityItemList)
		invAsString = 'OuroItemList: #' + str(length) + ' ['
		iter = 0
		for key, info in self.currentEntityItemList:
			invAsString += '[UUID: ' + str(info[0]) + ' ID: ' + str(info[1]) + ' Count: ' + str(info[2]) + ' Index: ' + str(info[3]) + ']'
			if iter < length - 1:
				invAsString += ', '
			iter += 1
		invAsString += ']'
		return invAsString

	#Utility
	def indexIsValid(self, index):
		if index < 0 or index >= len(self.inventory):
			return GlobalConst.INVENTORY_OPERATION_INVALID_INDEX
		return GlobalConst.INVENTORY_OPERATION_OK

	def countIsValid(self, item, count, countCanBeZero=False):
		if count < 0 or (count == 0 and countCanBeZero is False) or count > item.getMaxStack():
			return GlobalConst.INVENTORY_OPERATION_INVALID_COUNT
		return GlobalConst.INVENTORY_OPERATION_OK

	def isSpaceEmpty(self, index):
		if self.indexIsValid(index) is GlobalConst.INVENTORY_OPERATION_INVALID_INDEX:
			return GlobalConst.INVENTORY_OPERATION_INVALID_INDEX
		if self.isBlankSpace(index) or self.isNoneSpace(index):
			return GlobalConst.INVENTORY_OPERATION_OK
		return GlobalConst.INVENTORY_OPERATION_ERROR

	def getFirstFreeSpace(self):
		for i in range(0, len(self.inventory)):
			if self.inventory[i] == -1:
				return i

	def getFirstItemWithID(self, id):
		for item in self.inventory:
			if item is None or item == -1:
				continue
			if item.getID() == id:
				return item
		return None

	def getItemsWithID(self, id):
		foundItems = []
		for item in self.inventory:
			if item is None or item == -1:
				continue
			if item.getID() == id:
				foundItems.append(item)
		return foundItems

	def getItemAtIndex(self, index):
		return self.inventory[index]

	def isInventoryFull(self):
		emptyIndex = -1
		# Find the first empty index in the inventory
		for i in range(0, len(self.inventory)):
			if self.inventory[i] == -1:
				emptyIndex = i
				break

		# Inventory is full
		if emptyIndex == -1:
			return GlobalConst.INVENTORY_OPERATION_FULL
		return GlobalConst.INVENTORY_OPERATION_OK

	def isInventoryCompletelyFull(self):
		emptyIndex = -1
		unFullStackFound = False
		# Find the first empty index in the inventory
		for i in range(0, len(self.inventory)):
			if self.inventory[i] == -1:
				emptyIndex = i
				break
			else:
				item = self.inventory[i]
				if item.getStackable():
					if item.getCount() != item.getMaxStack():
						unFullStackFound = True
						break

		# Inventory is full
		if emptyIndex == -1:
			return GlobalConst.INVENTORY_OPERATION_FULL
		elif unFullStackFound:
			return GlobalConst.INVENTORY_OPERATION_FULL
		elif not unFullStackFound:
			return GlobalConst.INVENTORY_OPERATION_COMPLETELY_FULL

		return GlobalConst.INVENTORY_OPERATION_OK

	def isBlankSpace(self, index):
		if self.inventory[index] == -1:
			return True
		return False

	def isNoneSpace(self, index):
		if self.inventory[index] is None:
			return True
		return False

	def getSize(self):
		return len(self.inventory)

	def getHalfStackSizes(self, size):
		lowHalf = math.floor(size / 2)
		highHalf = size - lowHalf
		return highHalf, lowHalf

	def getInventoryAsString(self):
		invString = ''
		for i in range(len(self.inventory)):
			item = self.inventory[i]
			if item == -1:
				invString += str(item)
				if i != len(self.inventory) - 1:
					invString += ', '
			elif item is None:
				invString += 'None'
				invString += ', '
			else:
				invString += item.toString()
				if i != len(self.inventory) - 1:
					invString += ', '
		return ''.join(('[', invString, ']'))