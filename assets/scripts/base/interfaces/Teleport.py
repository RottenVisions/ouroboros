# -*- coding: utf-8 -*-
import Ouroboros
import GlobalConst
import data_spaces
import data_avatar_initial
from OURODebug import *


class Teleport:
	def __init__(self):
		# If the login is a copy, it is placed on the main scene no matter how it is logged in.
		# Because the copy requires a key to be opened, all copies are created using the entity SpaceDuplicate
		# Therefore, we only need to simply determine whether the script type of the scene in the configuration
		# corresponding to the current spaceUType contains 'Duplicate'.
		# Can figure out if it is in a copy
		spacedata = data_spaces.data[self.cellData["spaceUType"]]
		avatar_initial_data = data_avatar_initial.data[self.roleType]

		if "Duplicate" in spacedata["entityType"]:
			self.cellData["spaceUType"] = avatar_initial_data["spaceUType"]
			self.cellData["direction"] = (0, 0, avatar_initial_data["spawnYaw"])
			self.cellData["position"] = avatar_initial_data["spawnPos"]

	# --------------------------------------------------------------------------------------------
	#                              Callbacks
	# --------------------------------------------------------------------------------------------
	def onClientEnabled(self):
		"""
		Ouroboros method.
		The entity is officially activated to be usable.
		At this point, the entity has established the client corresponding entity,
		and its cell part can be created here.
		"""
		if self.cell is not None:
			return

		# Preventing the use of the same number to log in different demos can't
		# find a matching map and can't load resources, so you can't enter the game.
		# Check it here, if it is not correct, force synchronization to the matching map.
		# Ignore the inspection of the robot
		if hasattr(self, "cellData") and self.getClientType() != 6:
			# If the character jumps to other scenes that belong to the same demo,
			# then it is not mandatory to return to the main scene of birth.
			if self.cellData["spaceUType"] in GlobalConst.idmo_maps.values():
				spaceUType = GlobalConst.idmo_maps.get(self.getClientDatas()[0], 1)

				if self.cellData["spaceUType"] != spaceUType:
					spacedata = data_spaces.data[spaceUType]
					self.spaceUTypeB = spaceUType
					self.cellData["spaceUType"] = spaceUType
					self.cellData["position"] = spacedata.get("spawnPos", (0, 0, 0))

		Ouroboros.globalData["Spaces"].loginToSpace(self, self.spaceUTypeB, {})



