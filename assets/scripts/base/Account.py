# -*- coding: utf-8 -*-
import Ouroboros
from OURODebug import *

class Account(Ouroboros.Proxy):
	def __init__(self):
		Ouroboros.Proxy.__init__(self)

	def onTimer(self, id, userArg):
		"""
		Ouroboros method.
		Using addTimer, when the time arrives then the interface is called
		@param id : addTimer returns the value of the ID
		@param userArg : addTimer last parameter given the data
		"""
		DEBUG_MSG(id, userArg)

	def onClientEnabled(self):
		"""
		Ouroboros method.
		The entity was officially activated to be used, in this case the entity has established a client corresponding to the entity, you can create it
		cell portion.
		"""
		INFO_MSG("account[%i] entities enable. entityCall:%s" % (self.id, self.client))

	def onLogOnAttempt(self, ip, port, password):
		"""
		Ouroboros method.
		Client login fails the callback to here
		"""
		INFO_MSG(ip, port, password)
		return Ouroboros.LOG_ON_ACCEPT

	def onClientDeath(self):
		"""
		Ouroboros method.
		Corresponding to the client entity has been destroyed
		"""
		DEBUG_MSG("Account[%i].onClientDeath:" % self.id)
		self.destroy()
