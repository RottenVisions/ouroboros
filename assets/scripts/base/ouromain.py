# -*- coding: utf-8 -*-
import os
import Ouroboros
from OURODebug import *

import Watcher
import data_spaces

import items

from EntityCreator import *

def onBaseAppReady(isBootstrap):
	"""
	Ouroboros method.
	baseapp is ready.
	@param isBootstrap: Whether the first start of the baseapp
	@type isBootstrap: BOOL
	"""
	#INFO_MSG("ya")
	#EntityCreator.CreateBaseEntities()
	props = {
		"name": "MyFirstEntity"
	}
	# Create FirstEntity
	#Ouroboros.createEntityLocally("FirstEntity", props)
	# Create a Scene entity when the baseapp is ready
	#Ouroboros.createEntityLocally("Scene", {})

	INFO_MSG('onBaseAppReady: isBootstrap=%s' % isBootstrap)

	# Installation monitor
	Watcher.setup()

	if isBootstrap:
		# Create spacemanager
		Ouroboros.createEntityLocally("Spaces", {})

	#INFO_MSG('onBaseAppReady: isBootstrap=%s, appID=%s, bootstrapGroupIndex=%s, bootstrapGlobalIndex=%s' % \
	 #(isBootstrap, os.getenv("OURO_COMPONENTID"), os.getenv("OURO_BOOTIDX_GROUP"), os.getenv("OURO_BOOTIDX_GLOBAL")))

def onReadyForLogin(isBootstrap):
	"""
	Ouroboros method.
	If the return value is greater than or equal to 1. 0 the initialization is completed, otherwise ready to return the progress value of 0. 0~1.0 a.
	This can ensure that the script layer all initialization is complete only after the open log in.
	@param isBootstrap: whether the first start of the baseapp
	@type isBootstrap: BOOL
	"""
	if not isBootstrap:
		INFO_MSG('initProgress: completed!')
		return 1.0

	spacesEntity = Ouroboros.globalData["Spaces"]

	tmpDatas = list(data_spaces.data.keys())
	count = 0
	total = len(tmpDatas)

	for utype in tmpDatas:
		spaceAlloc = spacesEntity.getSpaceAllocs()[utype]
		if spaceAlloc.__class__.__name__ != "SpaceAllocDuplicate":
			if len(spaceAlloc.getSpaces()) > 0:
				count += 1
		else:
			count += 1

	if count < total:
		v = float(count) / total
		# INFO_MSG('initProgress: %f' % v)
		return v;

	INFO_MSG('initProgress: completed!')
	return 1.0

def onReadyForShutDown():
	"""
	Ouroboros method.
	Process ask the script Layer: I want to shutdown, the script is ready?
	If it returns True, then the process will enter the shutdown process, the other value will make the process after a period of time ask again.
	The user can receive a message when the script layer of the data cleanup work to make the script layer the results of the work not because of the shutdown and lost.
	"""
	INFO_MSG('onReadyForShutDown()')
	return True

def onBaseAppShutDown(state):
	"""
	Ouroboros method.
	The baseapp is closed before the callback function
	@param state: 0 : to disconnect all clients before
	1 : in the All entity written to the database before
	2 : all of the entity is written to the database after
	@type state: int
	"""
	INFO_MSG('onBaseAppShutDown: state=%i' % state)

def onAutoLoadEntityCreate(entityType, dbid):
	"""
	Ouroboros method.
	Automatically loaded entity creation method, the engine allows the script
	layer to re-implement the creation of the entity,
	if the script does not implement this method
	The underlying engine uses createEntityAnywhereFromDBID to create entities.
	"""
	INFO_MSG('onAutoLoadEntityCreate: entityType=%s, dbid=%i' % (entityType, dbid))
	Ouroboros.createEntityAnywhereFromDBID(entityType, dbid)

def onInit(isReload):
	"""
	Ouroboros method.
	When the engine is started after initialization is complete all of the script after the interface is invoked
	@param isReload: whether is be to rewrite the load script after the triggered
	@type isReload: bool
	"""
	items.onInit()
	INFO_MSG('onInit::isReload:%s' % isReload)

def onFini():
	"""
	Ouroboros method.
	Engine officially closed
	"""
	INFO_MSG('onFini()')

def onCellAppDeath(addr):
	"""
	Ouroboros method.
	A cellapp death
	"""
	WARNING_MSG('onCellAppDeath: %s' % (str(addr)))

def onGlobalData(key, value):
	"""
	Ouroboros method.
	There is a change globalData
	"""
	DEBUG_MSG('onGlobalData: %s' % key)

def onGlobalDataDel(key):
	"""
	Ouroboros method.
	Delete globalData
	"""
	DEBUG_MSG('onDelGlobalData: %s' % key)

def onBaseAppData(key, value):
	"""
	Ouroboros method.
	There is a change baseAppData
	"""
	DEBUG_MSG('onBaseAppData: %s' % key)

def onBaseAppDataDel(key):
	"""
	Ouroboros method.
	Delete baseAppData
	"""
	DEBUG_MSG('onBaseAppDataDel: %s' % key)

def onLoseChargeCB(ordersID, dbid, success, data):
	"""
	Ouroboros method.
	There is an unidentified order is processed, it may be a timeout caused the recording to be billing
	Cleared, and receipt of third-party prepaid processing callback
	"""
	DEBUG_MSG('onLoseChargeCB: ordersID=%s, dbid=%i, success=%i, data=%s' % \
							(ordersID, dbid, success, data))
