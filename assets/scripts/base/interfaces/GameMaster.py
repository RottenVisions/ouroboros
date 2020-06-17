# -*- coding: utf-8 -*-
import Ouroboros
from OURODebug import *

import GlobalEnums
import Helper
import Functor

class GameMaster:
	"""
	The base interface class of the server game master abilities
	"""

	def __init__(self):
		pass

	def setGameMaster(self, value):
		if value is True:
			msg = 'GameMaster privileges have been enabled for you %s, congratulations!' % self.playerName
		else:
			msg = 'GameMaster privileges have been disabled for you %s, condolences...' % self.playerName
		self.client.receiveChatMessage(self.id, self.playerName, msg,
									   GlobalEnums.ChatChannel.CHAT_CHANNEL_GM.value)
		self.accountEntity.gameMaster = value

	def isGameMaster(self):
		return self.accountEntity.gameMaster

	def notGameMasterMessage(self):
		return 'Your account is not a GameMaster, cannot execute command!'

	def command(self, command, value):
		pass

	def parseCommand(self, entityID, command):
		#Remove the dot that begins the command
		command = command[1:]
		parts = command.split()
		cmd = parts[0]
		valOne = None
		valTwo = None
		valThree = None
		targetID = -1
		valid = False

		if (len(parts) > 1):
			valOne = parts[1]
			if (len(parts) > 2):
				valTwo = parts[2]
				if (len(parts) > 3):
					valThree = parts[3]
		if cmd == 'help':
			return self.help()
		if cmd == 'set':
			if valOne == 'eg':
				if valTwo == 'self':
					targetID = entityID
				elif valTwo.isdigit():
					targetID = int(valTwo)
				else:
					player = Helper.getPlayerByName(valTwo)
					if player == None:
						return 'Set EG failed, player name %s not found!' % valTwo
					else:
						targetID = player.id
				targetEntity = Helper.getEntity(int(targetID))
				if targetEntity == None:
					return 'Cannot set EG without a valid target! [%s]' % targetID
				elif valThree.isdigit():
					targetEntity.cell.setEG(int(valThree))
					return "Set EG of %i to %i." % (targetID, int(valThree))
				else:
					return 'Cannot set EG without a valid value! [%s]' % valTwo
			elif valOne == 'hp':
				if valTwo == 'self':
					targetID = entityID
				else:
					targetID = int(valTwo)
				targetEntity = Helper.getEntity(int(targetID))
				if targetEntity == None:
					return 'Cannot set HP without a valid target! [%s]' % targetID
				elif valThree.isdigit():
					targetEntity.cell.setHP(int(valThree))
					return "Set HP of %i to %i." % (targetID, int(valThree))
				else:
					return 'Cannot set HP without a valid value! [%s]' % valTwo
			else:
				return 'Cannot set without a valid property!'
		if cmd == 'addAura' or cmd == 'aa':
			if valOne == 'self' or valOne is None:
				targetID = entityID
			elif not valOne.isdigit():
				return 'Cannot add aura with a invalid target %s' % valOne
			else:
				targetID = int(valOne)
			targetEntity = Helper.getEntity(int(targetID))
			if targetEntity == None:
				return 'Cannot add aura without a valid target! [%s]' % targetID
			elif valTwo.isdigit() or (valOne is None and valTwo is None):
				#get and check aura exists here and send msg to user
				targetEntity.cell.applyTargetAura(int(valTwo), targetID, self.id)
				return "Adding Aura %i to %i." % (int(valTwo), targetID)
			else:
				return 'Cannot add aura without a valid value! [%s]' % valTwo
		if cmd == 'restoreAll' or cmd == 'ra':
			if valOne == 'self' or valOne is None:
				targetID = entityID
			else:
				targetID = int(valOne)
			targetEntity = Helper.getEntity(int(targetID))
			if targetEntity == None:
				return 'Cannot restore without a valid target! [%s]' % targetID
			else:
				targetEntity.cell.fullPower()
		pass

	def parseValue(self, value):
		pass

	def help(self):
		helpInfo = 'GM Commands Help\n' \
				   'addAura [target] [aura]\n' \
				   'set [target] [type] [value]\n' \
				   'addAura [target] [aura]\n' \
				   'addAura [target] [aura]\n'
		return helpInfo