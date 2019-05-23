# -*- coding: utf-8 -*-
import os
import Ouroboros
from OURODebug import *

"""
The logger process mainly handles the log save of the Ouroboros server.
"""

def onLoggerAppReady():
	"""
	Ouroboros method.
	The logger is ready
	"""
	INFO_MSG('onLoggerAppReady: bootstrapGroupIndex=%s, bootstrapGlobalIndex=%s' % \
	 (os.getenv("OURO_BOOTIDX_GROUP"), os.getenv("OURO_BOOTIDX_GLOBAL")))

def onLoggerAppShutDown():
	"""
	Ouroboros method.
	The callback function before this logger is closed
	"""
	INFO_MSG('onLoggerAppShutDown()')

def onReadyForShutDown():
	"""
	Ouroboros method.
	The process asks the script layer: I want to shut down, is the script ready?
	If it returns True, the process will enter the shutdown process, and other values will cause the process to ask again after a while.
	The user can perform data cleaning of the script layer when receiving the message, so that the work result of the script layer is not lost due to shutdown.
	"""
	INFO_MSG('onReadyForShutDown()')
	return True

def onLogWrote(logData):
	"""
	Ouroboros method.
	The callback after the logger wrote a log.
	Users in need can write logs to other places (such as databases) here.
	If False is returned, the log will not be written to the disk file.
	If a string is returned, the log will be replaced with the returned content.
	@param logData: bytes
	"""
	return True

