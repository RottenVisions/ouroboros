# -*- coding: utf-8 -*-
import Ouroboros
from OURODebug import *

def onInit(isReload):
	"""
	Ouroboros method.
	When the engine is started after initialization is complete all of the script after the interface is invoked
	@param isReload: whether is be to rewrite the load script after the triggered
	@type isReload: bool
	"""
	DEBUG_MSG('onInit::isReload:%s' % isReload)

def onStart():
	"""
	Ouroboros method.
	In the onInitialize called after, ready to start the game engine calls this interface.
	"""
	pass

def onFinish():
	"""
	Ouroboros method.
	The client will be closed, the engine invokes this interface
	Can This do some in-game resource cleanup work
	"""
	pass
