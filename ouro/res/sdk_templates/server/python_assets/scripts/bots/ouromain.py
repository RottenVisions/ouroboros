# -*- coding: utf-8 -*-
import Ouroboros
from KBEDebug import *

def onInit(isReload):
	"""
	Ouroboros method.
	This interface is called when all scripts are initialized after the engine is started.
	@param isReload: Whether it is triggered after being overwritten by the load script
	@type isReload: bool
	"""
	DEBUG_MSG('onInit::isReload:%s' % isReload)

def onStart():
	"""
	Ouroboros method.
	After the onInitialize call, the engine calls this interface when it is ready to start the game.
	"""
	pass

def onFinish():
	"""
	Ouroboros method.
	When the client is about to shut down, the engine calls this interface.
	Can do some game resource cleaning work here.
	"""
	pass
