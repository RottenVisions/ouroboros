# -*- coding: utf-8 -*-
import Ouroboros
from OURODebug import *

class Account(Ouroboros.Proxy):
	def __init__(self):
		Ouroboros.Proxy.__init__(self)
		
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
		INFO_MSG("account[%i] entities enable. entityCall:%s" % (self.id, self.client))
			
	def onLogOnAttempt(self, ip, port, password):
		"""
		Ouroboros method.
		The client will call back here when the login fails.
		"""
		INFO_MSG(ip, port, password)
		return Ouroboros.LOG_ON_ACCEPT
		
	def onClientDeath(self):
		"""
		Ouroboros method.
		The client corresponding entity has been destroyed
		"""
		DEBUG_MSG("Account[%i].onClientDeath:" % self.id)
		self.destroy()
