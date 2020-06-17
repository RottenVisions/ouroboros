# -*- coding: utf-8 -*-
import ServerConstantsDefine
from OURODebug import *
from interfaces.GameObject import GameObject
import data_entities

class SpawnPoint(Ouroboros.Entity, GameObject):
	def __init__(self):
		Ouroboros.Entity.__init__(self)
		self.addTimer(1, 0, ServerConstantsDefine.TIMER_TYPE_SPAWN)

	def spawnTimer(self):
		data = data_entities.data.get(self.spawnEntityNO)

		if data is None:
			ERROR_MSG("SpawnPoint::spawn:%i not found." % self.spawnEntityNO)
			return

		params = {
			"spawnID"	: self.id,
			"spawnPos" : tuple(self.position),
			"uid" : data["id"],
			"utype" : data["etype"],
			"modelID" : data["modelID"],
			"modelScale" : self.modelScale,
			"dialogID" : data["dialogID"],
			"name" : data["name"],
			"descr" : data.get("descr", ''),
			"itemId" : 2,
			"type": data['entityType'],
			"attack_Max" : data.get("attack_Max",10),
			"attack_Min" : data.get("attack_Min",0),
			"defence" : data.get("defence",10),
			"rating" : data.get("rating",100),
			"dodge" : data.get("dodge",100),
			"HP_Max" : data.get("HP_Max",200),
		}
		e = Ouroboros.createEntity(data["entityType"], self.spaceID, tuple(self.position), tuple(self.direction), params)

	#--------------------------------------------------------------------------------------------
	#                              Callbacks
	#--------------------------------------------------------------------------------------------
	def onTimer(self, tid, userArg):
		"""
		Ouroboros method.
		Engine callback timer trigger
		"""
		#DEBUG_MSG("%s::onTimer: %i, tid:%i, arg:%i" % (self.getScriptName(), self.id, tid, userArg))
		if ServerConstantsDefine.TIMER_TYPE_SPAWN == userArg:
			self.spawnTimer()

		GameObject.onTimer(self, tid, userArg)

	def onRestore(self):
		"""
		Ouroboros method.
		The cell part of the entity is successfully restored.
		"""
		GameObject.onRestore(self)
		self.addTimer(1, 0, ServerConstantsDefine.TIMER_TYPE_SPAWN)

	def onDestroy(self):
		"""
		Ouroboros method.
		The current entity will soon be destroyed by the engine.
		You can do some pre-destruction work here.
		"""
		DEBUG_MSG("onDestroy(%i)" % self.id)

	def onEntityDestroyed(self, entityNO):
		"""
		defined.
		Is the birth entity destroyed and needs to be rebuilt?
		"""
		self.addTimer(1, 0, ServerConstantsDefine.TIMER_TYPE_SPAWN)
