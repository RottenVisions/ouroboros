# -*- coding: utf-8 -*-
import os
import Ouroboros
from OURODebug import *

"""
"""

def onDBMgrReady():
	"""
	Ouroboros method.
	Dbmgr is ready

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
	The callback function before this dbmgr is closed

	"""
	INFO_MSG('onDBMgrShutDown()')

def onSelectAccountDBInterface(accountName):
	"""
	Ouroboros method.
	This callback implementation returns the database interface corresponding to an account. After the selected interface, the related operations of dbmgr for this account are completed by the corresponding database interface.

	The database interface is defined in ouroboros_defs.xml->dbmgr->databaseInterfaces.

	Use this interface to determine which database the account should be stored in based on accountName.

	"""
	return "default"
