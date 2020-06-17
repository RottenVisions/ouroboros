# -*- coding: utf-8 -*-
import os
import Ouroboros
from OURODebug import *

"""
The loginapp process mainly deals with the Ouroboros server login and account creation.
Currently the script supports several functions.:
1: Registered account check
2: Login check
3: Custom socket callback, refer to the Poller implementation in the interface
"""


def onLoginAppReady():
	"""
	Ouroboros method.
	loginapp ready.
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
	This loginapp be closed before the callback function
	"""
	INFO_MSG('onLoginAppShutDown()')

def onRequestLogin(loginName, password, clientType, datas):
	"""
	Ouroboros method.
	Account request login callback
	Here you can also login queue, the queue information is stored in the datas
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
	Account request log after the db validation callback
	loginName: the login both the login when the input of the client name.
	accountName: the account name is dbmgr query to get the name.
	errorno: Ouroboros. SERVER_ERR_*
	This mechanism is used for one account more than the name of the system or a multiple party account system login to the server.
	The client gets the baseapp address at the same time will also return the account name, client login baseapp should use this account name for login
	"""
	INFO_MSG('onLoginCallbackFromDB() loginName=%s, accountName=%s, errorno=%s' % (loginName, accountName, errorno))

def onRequestCreateAccount(accountName, password, datas):
	"""
	Ouroboros method.
	Request account creation callback
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
	Account registration request after db authentication callback
	errorno: Ouroboros.SERVER_ERR_*
	"""
	INFO_MSG('onCreateAccountCallbackFromDB() accountName=%s, errorno=%s' % (accountName, errorno))
