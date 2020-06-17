# -*- coding: utf-8 -*-
import Ouroboros
from OURODebug import *
#import dialogmgr
import abilities
import auras

def onInit(isReload):
	"""
	Ouroboros method.
	When the engine is started after initialization is complete all of the script after the interface is invoked
	"""
	DEBUG_MSG('onInit::isReload:%s' % isReload)
	#dialogmgr.onInit()
	abilities.onInit()
	auras.onInit()

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


def onSpaceData(spaceID, key, value):
	"""
	Ouroboros method.
	spaceData change
	@spaceID:  The data is set in the space of this spaceID.
	@key:  Key set.
	@value:  The value that is set, or None if the value is deleted.
	"""
	DEBUG_MSG('onSpaceData: spaceID=%s, key=%s, value=%s.' % (spaceID, key, value))


def onSpaceGeometryLoaded(spaceID, mapping):
	"""
	Ouroboros method.
	Space Some or all chunks and other data are loaded.
	Which part needs to be determined by the scope of the cell
	"""
	DEBUG_MSG('onSpaceGeometryLoaded: spaceID=%s, mapping=%s.' % (spaceID, mapping))


def onAllSpaceGeometryLoaded(spaceID, isBootstrap, mapping):
	"""
	Ouroboros method.
	Space Some or all chunks and other data are loaded.
	Which part needs to be determined by the scope of the cell
	"""
	DEBUG_MSG('onAllSpaceGeometryLoaded: spaceID=%s, isBootstrap=%i, mapping=%s.' % (spaceID, isBootstrap, mapping))
