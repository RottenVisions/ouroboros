# -*- coding: utf-8 -*-
import os
import Ouroboros
from OURODebug import *

"""
"""

def onDBMgrReady():
	"""
	Ouroboros method.
	dbmgr is ready.
	"""
	INFO_MSG('onDBMgrReady: bootstrapGroupIndex=%s, bootstrapGlobalIndex=%s' % \
	 (os.getenv("OURO_BOOTIDX_GROUP"), os.getenv("OURO_BOOTIDX_GLOBAL")))

	#Ouroboros.addTimer(0.01, 1.0, onTick)

def onTick(timerID):
	"""
	"""
	INFO_MSG('onTick()')

def onDBMgrShutDown():
	"""
	Ouroboros method.
	This dbmgr be closed before the callback function
	"""
	INFO_MSG('onDBMgrShutDown()')

def onSelectAccountDBInterface(accountName):
	"""
	Ouroboros method.
	This callback implementation returns an account and a corresponding database interface, the selected interface after the dbmgr for the account of the relevant operations by the corresponding database interface to complete the
	The database interface in ouroboros_defs. xml->dbmgr->databaseInterfaces definition.
	The use of the interface can be based on the accountName to determine the Account should be stored in which database.
	"""
	return "default"
