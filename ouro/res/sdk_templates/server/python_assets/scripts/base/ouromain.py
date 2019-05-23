# -*- coding: utf-8 -*-
import os
import Ouroboros
from OURODebug import *

def onBaseAppReady(isBootstrap):
	"""
	Ouroboros method.
	Baseapp is ready
	@param isBootstrap: Is it the first baseapp to start?
	@type isBootstrap: BOOL
	"""
	INFO_MSG('onBaseAppReady: isBootstrap=%s, appID=%s, bootstrapGroupIndex=%s, bootstrapGlobalIndex=%s' % \
	 (isBootstrap, os.getenv("OURO_COMPONENTID"), os.getenv("OURO_BOOTIDX_GROUP"), os.getenv("OURO_BOOTIDX_GLOBAL")))

def onReadyForLogin(isBootstrap):
	"""
	Ouroboros method.
	If the return value is greater than or equal to 1.0, the initialization is completed, otherwise the progress value of the preparation is returned to 0.0~1.0.
	Here you can ensure that the login layer is not opened until the script layer has been fully initialized.
	@param isBootstrap: Is it the first baseapp to start?
	@type isBootstrap: BOOL
	"""
	return 1.0

def onReadyForShutDown():
	"""
	Ouroboros method.
	The process asks the script layer: I want to shut down, is the script ready?
	If it returns True, the process will enter the shutdown process, and other values will cause the process to ask again after a while.
	The user can perform data cleaning of the script layer when receiving the message, so that the work result of the script layer is not lost due to shutdown.
	"""
	INFO_MSG('onReadyForShutDown()')
	return True

def onBaseAppShutDown(state):
	"""
	Ouroboros method.
	The callback function before this baseapp is closed
	@param state: 0 : before disconnecting all clients
				  1 : Before writing all entities to the database
				  2 : After all entities are written to the database
	@type state: int
	"""
	INFO_MSG('onBaseAppShutDown: state=%i' % state)
		
def onInit(isReload):
	"""
	Ouroboros method.
	This interface is called when all scripts are initialized after the engine is started.
	@param isReload: Whether it is triggered after being overwritten by the load script
	@type isReload: bool
	"""
	INFO_MSG('onInit::isReload:%s' % isReload)

def onFini():
	"""
	Ouroboros method.
	The engine is officially closed
	"""
	INFO_MSG('onFini()')
	
def onCellAppDeath(addr):
	"""
	Ouroboros method.
	Death of a cellapp
	"""
	WARNING_MSG('onCellAppDeath: %s' % (str(addr)))
	
def onGlobalData(key, value):
	"""
	Ouroboros method.
	globalData has changed
	"""
	DEBUG_MSG('onGlobalData: %s' % key)
	
def onGlobalDataDel(key):
	"""
	Ouroboros method.
	globalData has deleted
	"""
	DEBUG_MSG('onDelGlobalData: %s' % key)
	
def onBaseAppData(key, value):
	"""
	Ouroboros method.
	baseAppData has changed
	"""
	DEBUG_MSG('onBaseAppData: %s' % key)
	
def onBaseAppDataDel(key):
	"""
	Ouroboros method.
	baseAppData has deleted
	"""
	DEBUG_MSG('onBaseAppDataDel: %s' % key)

def onLoseChargeCB(ordersID, dbid, success, datas):
	"""
	Ouroboros method.
	There is an unknown order being processed, it may be timeout causing the record to be billing
	Clear, but receive a third-party recharge processing callback
	"""
	DEBUG_MSG('onLoseChargeCB: ordersID=%s, dbid=%i, success=%i, datas=%s' % \
							(ordersID, dbid, success, datas))


