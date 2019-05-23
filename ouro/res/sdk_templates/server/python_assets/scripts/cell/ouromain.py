# -*- coding: utf-8 -*-
import Ouroboros
from KBEDebug import *

def onInit(isReload):
	"""
	Ouroboros method.
	This interface is called when all scripts are initialized after the engine is started.
	"""
	DEBUG_MSG('onInit::isReload:%s' % isReload)
	
def onGlobalData(key, value):
	"""
	Ouroboros method.
	globalData change
	"""
	DEBUG_MSG('onGlobalData: %s' % key)
	
def onGlobalDataDel(key):
	"""
	Ouroboros method.
	globalData delete
	"""
	DEBUG_MSG('onDelGlobalData: %s' % key)

def onCellAppData(key, value):
	"""
	Ouroboros method.
	cellAppData change
	"""
	DEBUG_MSG('onCellAppData: %s' % key)
	
def onCellAppDataDel(key):
	"""
	Ouroboros method.
	cellAppData delete
	"""
	DEBUG_MSG('onCellAppDataDel: %s' % key)
	
def onSpaceData( spaceID, key, value ):
	"""
	Ouroboros method.
	spaceData change
	"""
	pass
	
def onAllSpaceGeometryLoaded( spaceID, isBootstrap, mapping ):
	"""
	Ouroboros method.
	Space Some data or all chunks are loaded.
	Which part needs to be determined by the scope of the cell
	"""
	pass