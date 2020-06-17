# -*- coding: utf-8 -*-
import Ouroboros
from OURODebug import *

import time

import GlobalConst

from AVATAR_INFO import TAvatarInfo
from AVATAR_INFO import TAvatarInfoList

from AVATAR_DATA import TAvatarData

import data_avatar_initial
import data_spaces

class Account(Ouroboros.Proxy):

	"""
	Account entity
	After the client logs in to the server, the server will automatically create this entity and interact with the client through this entity.
	"""
	def __init__(self):
		Ouroboros.Proxy.__init__(self)
		self.activeAvatar = None
		self.relogin = time.time()

	# --------------------------------------------------------------------------------------------
	#                              Custom
	# --------------------------------------------------------------------------------------------

	def reqAvatarList(self):
		"""
		exposed.
		Client request query role list
		"""
		DEBUG_MSG("Account[%i].reqAvatarList: size=%i." % (self.id, len(self.avatars)))
		self.client.onReqAvatarList(self.avatars)

	def reqCreateAvatar(self, name, roleType, gender, hairStyle):
		"""
		exposed.
		The client requests to create a role
		"""

		avatarData = TAvatarData().createFromDict({"param1": 0, "param2": b''})
		avatarinfo = TAvatarInfo()
		avatarinfo.extend([0, "", 0, 0, 0, 0, avatarData])

		"""
		if name in all_avatar_names:
			retcode = 2
			self.client.onCreateAvatarResult(retcode, avatarinfo)
			return
		"""

		if len(self.avatars) >= 3:
			DEBUG_MSG("Account[%i].reqCreateAvatar:%s. avatars=%s.\n" % (self.id, name, self.avatars))
			self.client.onCreateAvatarResult(3, avatarinfo)
			return

		""" Give a birth point based on the front end category
		UNKNOWN_CLIENT_COMPONENT_TYPE	= 0,
		CLIENT_TYPE_MOBILE				= 1,	// Mobile phone
		CLIENT_TYPE_PC					= 2,	// pc， Generally exe client
		CLIENT_TYPE_BROWSER				= 3,	// Web application, html5，flash
		CLIENT_TYPE_BOTS				= 4,	// bots
		CLIENT_TYPE_MINI				= 5,	// Micro client
		"""

		"""
		# If the robot is logged in, randomly throw in a scene
		if self.getClientType() == 6:
			while True:
				spaceName = random.choice(list(GlobalConst.idmo_maps.keys()))
				if len(spaceName) > 0:
					spaceUType = GlobalConst.idmo_maps[spaceName]
					break
		"""

		spaceUType = GlobalConst.idmo_maps.get(self.getClientDatas()[0], 1)
		spaceData = data_spaces.data.get(spaceUType)
		#spaceData.get("spawnPos")
		
		#"direction"		: (0, 0, data_avatar_initial.data[roleType]["spawnYaw"])

		props = {
			"name"				: name,
			"roleType"			: roleType,
			"gender"			: gender,
			"spaceUType"		: spaceUType,
			"hairStyle"			: hairStyle,
			"direction"			: data_avatar_initial.data[roleType]["spawnDir"],
			"position"			: data_avatar_initial.data[roleType]["spawnPos"],

			# ----------cell---------
			"roleTypeCell"      : roleType,
			# ---------properties
			"level"				: data_avatar_initial.data[roleType]["level"],
			"exp"				: data_avatar_initial.data[roleType]["experience"],
			"currency"			: data_avatar_initial.data[roleType]["currency"],
			"strength"			: data_avatar_initial.data[roleType]["strength"],
			"dexterity"			: data_avatar_initial.data[roleType]["dexterity"],
			"stamina"			: data_avatar_initial.data[roleType]["stamina"],

			"attack_Max"		: data_avatar_initial.data[roleType]["strength"]*2,
			"attack_Min"		: data_avatar_initial.data[roleType]["strength"]*1,
			"defence"			: int(data_avatar_initial.data[roleType]["dexterity"]/4),
			"rating"			: int(data_avatar_initial.data[roleType]["dexterity"]/ 15+100),
			"dodge"				: int(data_avatar_initial.data[roleType]["dexterity"]/ 15+100),
			"HP_Max"            : 10,
			# ---------properties

		}

		avatar = Ouroboros.createEntityLocally('Avatar', props)
		if avatar:
			avatar.writeToDB(self._onAvatarSaved)

		DEBUG_MSG("Account[%i].reqCreateAvatar:%s, spaceUType=%i, spawnPos=%s.\n spawnDir=%s." % (self.id, name, avatar.cellData["spaceUType"], data_avatar_initial.data[roleType]["spawnPos"], data_avatar_initial.data[roleType]["spawnDir"]))

	def reqRemoveAvatar(self, name):
		"""
		exposed.
		The client requests to delete a role
		"""
		DEBUG_MSG("Account[%i].reqRemoveAvatar: Removing: %s" % (self.id, name))
		found = 0
		for key, info in self.avatars.items():
			if info[1] == name:
				del self.avatars[key]
				found = key
				DEBUG_MSG("Account[%i].reqRemoveAvatar: Deleted: %s" % (self.id, name))
				break

		self.client.onRemoveAvatar(found)

	def selectActiveAvatar(self, dbid):
		"""
		exposed.
		The client selects a character to play the game
		"""
		DEBUG_MSG("Account[%i].selectAvatarGame:%i. self.activeAvatar=%s" % (self.id, dbid, self.activeAvatar))
		# Note: The entity using giveClientTo must be the entity on the current baseapp
		if self.activeAvatar is None:
			if dbid in self.avatars:
				# self.lastSelCharacter = dbid
				# Because it needs to load the role from the database, it is an asynchronous process. If the load succeeds or fails, the __onAvatarCreated interface will be called.
				# After the role is created, account will call giveClientTo to switch client control (which can be understood as the binding of the network connection to an entity) to Avatar.
				# After that, all kinds of input and output of the client are proxyed by the Avatar on the server, and any proxy entity will have onEntitiesEnabled when it gains control.
				# Avatar inherits Teleport, and Teleport.onEntitiesEnabled will create players in specific scenes.
				Ouroboros.createEntityFromDBID("Avatar", dbid, self.__onAvatarCreated)
			else:
				ERROR_MSG("Account[%i]::selectAvatarGame: not found dbid(%i)" % (self.id, dbid))
		else:
			self.giveClientTo(self.activeAvatar)

	#--------------------------------------------------------------------------------------------
	#                              Callbacks
	#--------------------------------------------------------------------------------------------


	def onTimer(self, id, userArg):
		"""
		Ouroboros method.
		After using addTimer, the interface is called when the time arrives.

		@param id : return value ID of addTimer

		@param userArg : addTimer The data given by the last parameter

		"""
		DEBUG_MSG(id, userArg)

	def onClientEnabled(self):
		"""
		Ouroboros method.
		The entity is officially activated to be usable. At this point, the entity has established the client corresponding entity, which can be created here.

		Cell part.

		"""
		INFO_MSG("Account[%i]::onClientEnabled:entities enable. entityCall:%s, clientType(%i), clientDatas=(%s), hasAvatar=%s, accountName=%s" % \
			(self.id, self.client, self.getClientType(), self.getClientDatas(), self.activeAvatar, self.__ACCOUNT_NAME__))
		INFO_MSG("account[%i] entities enable. entityCall:%s" % (self.id, self.client))

	def onLogOnAttempt(self, ip, port, password):
		"""
		Ouroboros method.
		When the client fails to log in, it will call back here.

		"""
		INFO_MSG("Account[%i]::onLogOnAttempt: ip=%s, port=%i, selfclient=%s" % (self.id, ip, port, self.client))
		"""
		if self.activeAvatar != None:
			return Ouroboros.LOG_ON_REJECT

		if ip == self.lastClientIpAddr and password == self.password:
			return Ouroboros.LOG_ON_ACCEPT
		else:
			return Ouroboros.LOG_ON_REJECT
		"""

		# If an online account is logged in by a client and onLogOnAttempt returns permission
		# Then kick off the previous client connection
		# Then self.activeAvatar may not be None at this time， The normal process is to destroy the new client such as this role to re-select the role to enter
		if self.activeAvatar:
			if self.activeAvatar.client is not None:
				self.activeAvatar.giveClientTo(self)

			self.relogin = time.time()
			self.activeAvatar.destroySelf()
			self.activeAvatar = None

		return Ouroboros.LOG_ON_ACCEPT

	def onClientDeath(self):
		"""
		Ouroboros method.
		The client corresponding entity has been destroyed

		"""
		if self.activeAvatar:
			self.activeAvatar.accountEntity = None
			self.activeAvatar = None

		DEBUG_MSG("Account[%i].onClientDeath:" % self.id)
		self.destroy()

	def onDestroy(self):
		"""
		Ouroboros method.
		Entity destruction
		"""
		DEBUG_MSG("Account::onDestroy: %i." % self.id)

		if self.activeAvatar:
			self.activeAvatar.accountEntity = None

			try:
				self.activeAvatar.destroySelf()
			except:
				pass

			self.activeAvatar = None

	def __onAvatarCreated(self, baseRef, dbid, wasActive):
		"""
		Called when the character is selected to enter the game
		"""
		if wasActive:
			ERROR_MSG("Account::__onAvatarCreated:(%i): this character is in world now!" % (self.id))
			return
		if baseRef is None:
			ERROR_MSG("Account::__onAvatarCreated:(%i): the character you wanted to created is not exist!" % (self.id))
			return

		avatar = Ouroboros.entities.get(baseRef.id)
		if avatar is None:
			ERROR_MSG("Account::__onAvatarCreated:(%i): when character was created, it died as well!" % (self.id))
			return

		if self.isDestroyed:
			ERROR_MSG("Account::__onAvatarCreated:(%i): i dead, will the destroy of Avatar!" % (self.id))
			avatar.destroy()
			return

		info = self.avatars[dbid]
		chosenRole = info[2]

		avatar.cellData["modelID"] = data_avatar_initial.data[chosenRole]["modelID"]
		avatar.cellData["modelScale"] = data_avatar_initial.data[chosenRole]["modelScale"]
		avatar.cellData["moveSpeed"] = data_avatar_initial.data[chosenRole]["moveSpeed"]
		avatar.accountEntity = self
		self.activeAvatar = avatar
		self.giveClientTo(avatar)

	def _onAvatarSaved(self, success, avatar):
		"""
		New role write database callback
		"""
		INFO_MSG('Account::onAvatarSaved:(%i) Create Avatar Success: %i, Name: %s, DBID: %i' % (
		self.id, success, avatar.cellData["name"], avatar.databaseID))

		# If the account has been destroyed at this time, the role can no longer be recorded, then we clear the role.
		if self.isDestroyed:
			if avatar:
				avatar.destroy(True)

			return

		avatarData = TAvatarData().createFromDict({"param1": 0, "param2": b''})

		avatarinfo = TAvatarInfo()
		avatarinfo.extend([0, "", 0, 0, 0, 0, avatarData])

		INFO_MSG('success: %i, %s %i' % (success, str(bool(success)), int(success)))
		if bool(success) == True or success == 1:
			info = TAvatarInfo()
			info.extend([avatar.databaseID, avatar.cellData["name"], avatar.roleType, 1, avatar.gender, avatar.hairStyle, avatarData])
			self.avatars[avatar.databaseID] = info
			avatarinfo[0] = avatar.databaseID
			avatarinfo[1] = avatar.cellData["name"]
			avatarinfo[2] = avatar.roleType
			avatarinfo[3] = 1
			avatarinfo[4] = avatar.gender
			avatarinfo[5] = avatar.hairStyle
			avatarData[6] = avatarData

			self.writeToDB()
		else:
			avatarinfo[1] = "Creation Failed"

		avatar.destroy()

		if self.client:
			self.client.onCreateAvatarResult(success, avatarinfo)

	def createCell(self, entityCall):
		DEBUG_MSG("Account[%i].createCell:" % self.id)
		pass

	def enterGame(self):
		DEBUG_MSG("Account[%i].enterGame" % self.id)
		randomVar = 1
		if(randomVar == 1):
			error = 0
			self.client.onEnterGameFailed(error)
		else:
			self.client.onEnterGameSuccess()
		pass

	def exitGame(self):
		if self.activeAvatar:
			DEBUG_MSG("Account[%i-%s].exitGame Successful!" % (self.id, self.activeAvatar))
			self.activeAvatar.destroySelf()
			self.activeAvatar = None
			if self.client:
				self.client.onExitGameSuccess()
		else:
			error = 0
			if self.client:
				self.client.onExitGameFailed(error)
			DEBUG_MSG("Account[%i].exitGame Failed - no active avatar!" % (self.id))
		pass