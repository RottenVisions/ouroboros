# -*- coding: utf-8 -*-
import Ouroboros
from OURODebug import *

def onInit(isReload):
	"""
	Ouroboros method.
	When the engine is started after initialization is complete all of the script after the interface is invoked
	"""
	DEBUG_MSG('onInit::isReload:%s' % isReload)

def onGlobalData(key, value):
	"""
	Ouroboros method.
	Change globalData
	"""
	DEBUG_MSG('onGlobalData: %s' % key)

def onGlobalDataDel(key):
	"""
	Ouroboros method.
	Delete globalData
	"""
	DEBUG_MSG('onDelGlobalData: %s' % key)

def onCellAppData(key, value):
	"""
	Ouroboros method.
	Change cellAppData
	"""
	DEBUG_MSG('onCellAppData: %s' % key)

def onCellAppDataDel(key):
	"""
	Ouroboros method.
	Delete cellAppData
	"""
	DEBUG_MSG('onCellAppDataDel: %s' % key)

def onSpaceData( spaceID, key, value ):
	"""
	Ouroboros method.
	Change in spaceData
	"""
	pass

def onAllSpaceGeometryLoaded( spaceID, isBootstrap, mapping ):
	"""
	Ouroboros method.
	space a portion or all of the chunk data is loaded
	Specifically which part of the need by the cell responsible for the scope of the decision
	"""
	pass
