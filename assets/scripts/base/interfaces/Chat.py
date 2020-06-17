import json
import time
import datetime
import os.path

import GlobalConst
import GlobalEnums
import Helper

from os import path
from OURODebug import DEBUG_MSG

class Chat:
	"""
	Chat Manager
	"""
	def __init__(self):
		#self.chatChannelGlobal = {}
		self.createChannel(GlobalEnums.ChatChannel.CHAT_CHANNEL_GLOBAL)

	def sendChatMessage(self, entityID, message, channel):
		if (message.startswith('.', 0,1)):
			entity = Helper.getEntity(entityID)
			if (entity == None):
				msg = 'Failed to find entity %i! Could not send message.' % entityID
				self.client.receiveChatMessage(entityID, entity.playerName, msg, GlobalEnums.ChatChannel.CHAT_CHANNEL_GM.value)
				return
			if (self.isGameMaster() == True):
				retMsg = self.parseCommand(entityID, message)
				self.client.receiveChatMessage(entityID, entity.playerName, retMsg, GlobalEnums.ChatChannel.CHAT_CHANNEL_GM.value)
				return
			else:
				notGmMsg = self.notGameMasterMessage()
				self.client.receiveChatMessage(entityID, entity.playerName, notGmMsg, GlobalEnums.ChatChannel.CHAT_CHANNEL_GM.value)
				return
		self.parseChatMessage(entityID, message, channel)
		DEBUG_MSG('Send received! %i: %s' % (entityID, message))

	def parseChatMessage(self, entityID, message, channel):
		entity = Helper.getEntity(entityID)
		if (entity == None):
			DEBUG_MSG('Failed to find entity %i! Could not parse message.' % entityID)
			return
		self.appendMessage(GlobalEnums.ChatChannel(channel), entity.playerName, message)
		self.client.receiveChatMessage(entityID, entity.playerName, message, channel)
		pass

	def createChannel(self, channelType):
		if channelType == GlobalEnums.ChatChannel.CHAT_CHANNEL_GLOBAL:
			self.chatChannelGlobal = {}

	def appendMessage(self, channelType, sender, message):
		createdMessageObj = {
			'sender': sender,
			'message': message,
			'date': Helper.getDate(),
			'time': Helper.getTime()
		}
		if channelType == GlobalEnums.ChatChannel.CHAT_CHANNEL_GLOBAL:
			index = len(self.chatChannelGlobal) + 1 # so it is not a zwro index
			self.chatChannelGlobal[index] = createdMessageObj
		# Each message added, make sure we aren't over our chat limit.
		self.channelGarbageCollector()

	def updateMessage(self, channelType, sender, message):
		createdMessageObj = {
			'sender': sender,
			'message': message,
			'date': Helper.getDate(),
			'time': Helper.getTime()
		}
		if channelType == GlobalEnums.ChatChannel.CHAT_CHANNEL_GLOBAL:
			self.chatChannelGlobal.update(createdMessageObj)  # use this only for existing values

	def channelGarbageCollector(self):
		if len(self.chatChannelGlobal) >= GlobalConst.CHAT_CHANNEL_GLOBAL_CAPACITY:
			self.writeAndClear(GlobalEnums.ChatChannel.CHAT_CHANNEL_GLOBAL)

	def writeAndClear(self, channelType):
		self.writeChannelTypeToFile(channelType)
		if channelType == GlobalEnums.ChatChannel.CHAT_CHANNEL_GLOBAL:
			self.chatChannelGlobal.clear()

	def writeChannelTypeToFile(self, channelType):
		dictToWrite = None
		if channelType == GlobalEnums.ChatChannel.CHAT_CHANNEL_GLOBAL:
			dictToWrite = self.chatChannelGlobal
		self.writeChannelToFile(dictToWrite, channelType)

	def writeChannelToFile(self, dict, channelType):
		fileName = "%s.%s.%s" % (str(channelType.name), Helper.getDateDots(), Helper.getTimeDots())
		# This is a testing check and should not happen in the real world, basically this will 
		# track many messages coming in at once and overwriting the last wrote file
		dupeIndex = 0
		while path.exists('../../data/ChatLogs/%s.txt' % fileName):
			dupeIndex += 1
			fileName = "%s.%s.%s.%s" % (str(channelType.name), Helper.getDateDots(), Helper.getTimeDots(), dupeIndex)
		with open('ChatLogs/%s.txt' % fileName, 'w') as file:
			file.write(json.dumps(dict, indent=2))  # use `json.loads` to do the reverse, indent adds to new line