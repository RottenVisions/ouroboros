# -*- coding: utf-8 -*-
import Ouroboros

import copy
import random
from OURODebug import *

class Account(Ouroboros.Entity):
	def __init__(self):
		Ouroboros.Entity.__init__(self)
		DEBUG_MSG("Account::__init__:%s." % (self.__dict__))
		self.base.reqAvatarList()

	def onReqAvatarList(self, infos):
		"""
		defined method.
		"""

		DEBUG_MSG("Account:onReqAvatarList::%s" % (list(infos['values'])))
		self.base.reqCreateAvatar("ouro_bot_%s" % self.id, random.randrange(1, 2), random.randrange(0, 1), random.randrange(0, 5))
		self.avatars = copy.deepcopy(infos["values"])

	def onCreateAvatarResult(self, retcode, info):
		"""
		defined method.
		"""
		DEBUG_MSG("Account:onCreateAvatarResult::%s, retcode=%i" % (dict(info), retcode))

		if retcode == 0:
			self.base.selectActiveAvatar(info["dbid"])
		elif retcode == 1:
			self.base.selectActiveAvatar(info["dbid"])
			self.base.enterGame()
		else:
			print(len(self.avatars), 'len')
			if len(self.avatars) > 0:
				for infos in self.avatars:
					self.base.selectActiveAvatar(infos["dbid"])
					self.base.enterGame()
					break

	def onCreateAvatarSuccess(self, info):
		"""
		defined method.
		"""
		self.base.enterGame()
		DEBUG_MSG("Account:onCreateAvatarSuccess::%s" % (dict(info)))

	def onCreateAvatarFailed(self, errorCode):
		"""
		defined method.
		"""
		ERROR_MSG("Account:onCreateAvatarFailed:: errorCode=%i" % (errorCode))

	def onRemoveAvatar(self, dbid):
		"""
		defined method.
		"""
		DEBUG_MSG("Account:onRemoveAvatar:: dbid=%i" % (dbid))

	def onEnterGameFailed(self, error):
		DEBUG_MSG("%s::onEnterGameFailed: %i error: %i" % (self.__class__.__name__, self.id, error))
		pass

	def onExitGameFailed(self, error):
		DEBUG_MSG("%s::onExitGameFailed: %i error: %i" % (self.__class__.__name__, self.id, error))
		pass

	def onEnterGameSuccess(self):
		DEBUG_MSG("%s::onEnterGameSuccess: %i" % (self.__class__.__name__, self.id))
		pass

	def onExitGameSuccess(self):
		DEBUG_MSG("%s::onExitGameSuccess: %i" % (self.__class__.__name__, self.id))
		pass

	def onFirstEntityHello(self, message):
		DEBUG_MSG("%s::onExitGameSuccess: %i msg: %s" % (self.__class__.__name__, self.id, message))
		pass