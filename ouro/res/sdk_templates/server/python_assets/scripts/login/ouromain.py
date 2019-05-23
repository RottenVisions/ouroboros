# -*- coding: utf-8 -*-
import os
import Ouroboros
from KBEDebug import *

"""
The loginapp process mainly deals with the Ouroboros server login and account creation.
The script currently supports several features:
1: Register an account check
2: Login check
3: Custom socket callback, refer to the Poller implementation in the interface
"""


def onLoginAppReady():
	"""
	Ouroboros method.
	Loginapp is ready
	"""
	INFO_MSG('onLoginAppReady: bootstrapGroupIndex=%s, bootstrapGlobalIndex=%s' % \
	 (os.getenv("OURO_BOOTIDX_GROUP"), os.getenv("OURO_BOOTIDX_GLOBAL")))

	#Ouroboros.addTimer(0.01, 1.0, onTick)

def onTick(timerID):
	"""
	"""
	INFO_MSG('onTick()')

def onLoginAppShutDown():
	"""
	Ouroboros method.
	The callback function before this loginapp is closed
	"""
	INFO_MSG('onLoginAppShutDown()')

def onRequestLogin(loginName, password, clientType, datas):
	"""
	Ouroboros method.
	Callback when the account request is logged in
	Here you can also queue the login and store the queue information in datas.
	"""
	INFO_MSG('onRequestLogin() loginName=%s, clientType=%s' % (loginName, clientType))

	errorno = Ouroboros.SERVER_SUCCESS
	
	if len(loginName) > 64:
		errorno = Ouroboros.SERVER_ERR_NAME

	if len(password) > 64:
		errorno = Ouroboros.SERVER_ERR_PASSWORD

	return (errorno, loginName, password, clientType, datas)

def onLoginCallbackFromDB(loginName, accountName, errorno, datas):
	"""
	Ouroboros method.
	Db verification callback after account request
	loginName: The name entered by the client when the login name is both logged in.
	accountName: The account name is the name obtained by the dbmgr query.
	errorno: Ouroboros.SERVER_ERR_*
	This mechanism is used to log in to the server for an account multi-name system or multiple third-party account systems.
	When the client gets the baseapp address, it will also return the account name. The client login baseapp should use this account name to log in.
	"""
	INFO_MSG('onLoginCallbackFromDB() loginName=%s, accountName=%s, errorno=%s' % (loginName, accountName, errorno))
	
def onRequestCreateAccount(accountName, password, datas):
	"""
	Ouroboros method.
	Callback when requesting account creation
	"""
	INFO_MSG('onRequestCreateAccount() %s' % (accountName))

	errorno = Ouroboros.SERVER_SUCCESS
	
	if len(accountName) > 64:
		errorno = Ouroboros.SERVER_ERR_NAME

	if len(password) > 64:
		errorno = Ouroboros.SERVER_ERR_PASSWORD
		
	return (errorno, accountName, password, datas)

def onCreateAccountCallbackFromDB(accountName, errorno, datas):
	"""
	Ouroboros method.
	Db verification callback after account request registration
	errorno: Ouroboros.SERVER_ERR_*
	"""
	INFO_MSG('onCreateAccountCallbackFromDB() accountName=%s, errorno=%s' % (accountName, errorno))
