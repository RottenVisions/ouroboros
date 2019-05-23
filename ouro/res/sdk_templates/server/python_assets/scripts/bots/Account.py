# -*- coding: utf-8 -*-
import Ouroboros
import copy
from OURODebug import *

class Account(Ouroboros.Entity):
	def __init__(self):
		Ouroboros.Entity.__init__(self)
		DEBUG_MSG("Account::__init__:%s." % (self.__dict__))

