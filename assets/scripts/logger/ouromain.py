# -*- coding: utf-8 -*-
import os
import Ouroboros
from OURODebug import *

"""
the logger process the main processing Ouroboros the service side of the log save jobs.
"""

def onLoggerAppReady():
	"""
	Ouroboros method.
	the logger is ready.
	"""
	INFO_MSG('onLoggerAppReady: bootstrapGroupIndex=%s, bootstrapGlobalIndex=%s' % \
	 (os.getenv("OURO_BOOTIDX_GROUP"), os.getenv("OURO_BOOTIDX_GLOBAL")))

def onLoggerAppShutDown():
	"""
	Ouroboros method.
	This logger is closed before the callback function
	"""
	INFO_MSG('onLoggerAppShutDown()')

def onReadyForShutDown():
	"""
	Ouroboros method.
	Process ask the script Layer: I want to shutdown, the script is ready?
	If it returns True, then the process will enter the shutdown process, the other value will make the process after a period of time ask again.
	The user can receive a message when the script layer of the data cleanup work to make the script layer the results of the work not because of the shutdown and lost.
	"""
	INFO_MSG('onReadyForShutDown()')
	return True

def onLogWrote(logData):
	"""
	Ouroboros method.
	logger write a log post callback,
	There is a need of the user where the log is written to other place, such as a database
	@param logData: bytes
	"""
	pass
