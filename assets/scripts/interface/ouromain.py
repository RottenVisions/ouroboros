# -*- coding: utf-8 -*-
import os
import Ouroboros
from OURODebug import *
from Poller import Poller

"""
	interfaces process primary processing Ouroboros service end with the third-party platform to access connected to the work.
	(Note: Due to the interfaces is a single-threaded server, if you need to use the python http server library, we recommend using asynchronous, for example: the Tornado, otherwise will snap the main thread resulting in blocking)
	Currently supports several functions:
	1: register an account
	When a client requests a registered account, the request will be made loginapp forwarded to dbmgr, if dbmgr attached to the interfaces, the dbmgr forwards the request to here Ouroboros. onRequestCreateAccount）
	In this case the script receives the request can then be used in various ways with the third-party platform for communication, you can use python's http library can directly use the socket, when the third-party platform is completed should be
	The result of the interaction is returned to the engine baseapp layer, by Ouroboros. createAccountResponse be able to push information to the baseapp layer.

	2: account login
	When a client requests a login account, the request will be made loginapp forwarded to dbmgr, if dbmgr attached to the interfaces, the dbmgr forwards the request to here Ouroboros. onRequestAccountLogin）
	In this case the script receives the request can then be used in various ways with the third-party platform for communication, you can use python's http library can directly use the socket, when the third-party platform is completed should be
	The result of the interaction is returned to the engine baseapp layers, by Ouroboros. accountLoginResponse be able to push information to the baseapp layer.

	3: recharge billing
	When baseapp on request the billing entity. charge()after request by loginapp forwarded to dbmgr, if dbmgr attached to the interfaces, the dbmgr forwards the request to here Ouroboros. onRequestCharge）
	In this case the script receives the request can then be used in various ways with the third-party platform for communication, you can use python's http library can directly use the socket, when the third-party platform is completed should be
	The result of the interaction is returned to the engine baseapp layer, by Ouroboros. chargeResponse be able to push information to the baseapp layer entity. charge to the callback or callback to onLoseChargeCB interface.

	Some of the platform requirements of the client directly with the platform requests the billing platform using a callback server to complete the request, the reference to“platform-back tune”.

	4: platform callback
	To complete this function should be in the script layer to create a socket,
	And the socket attached to the Ouroboros, which prevents the blockage leads to the main thread, and then listening on the specified port.
	The use of OURO the Ouroboros. registerReadFileDescriptor()and Ouroboros. registerWriteFileDescriptor (), the Check API documentation specifically the Poller.py.
"""

g_poller = Poller()

def onInterfaceAppReady():
	"""
	Ouroboros method.
	interfaces ready.
	"""
	INFO_MSG('onInterfaceAppReady: bootstrapGroupIndex=%s, bootstrapGlobalIndex=%s' % \
	 (os.getenv("OURO_BOOTIDX_GROUP"), os.getenv("OURO_BOOTIDX_GLOBAL")))

	#Ouroboros.addTimer(0.01, 1.0, onTick)
	g_poller.start("localhost", 30040)

def onTick(timerID):
	"""
	"""
	INFO_MSG('onTick()')

def onInterfaceAppShutDown():
	"""
	Ouroboros method.
	This interfaces are closed before the callback function
	"""
	INFO_MSG('onInterfaceAppShutDown()')
	g_poller.stop()

def onRequestCreateAccount(registerName, password, datas):
	"""
	Ouroboros method.
	Request to create account callback
	@param registerName: the client request when the submitted name
	@type registerName: string

	@param password: password
	@type password: string

	@param datas: the client requests the data, the data can be forwarded to third party platform
	@type datas: bytes
	"""
	INFO_MSG('onRequestCreateAccount: registerName=%s' % (registerName))

	commitName = registerName

	# The default account name is submitted in the name of
	realAccountName = commitName

	# Here by http and other means to submit the request to the third party platform, the platform returns data can also be placed into the datas
	# datas will callback to the client
	# If you use http access, because the interfaces are single-threaded, synchronous http access is easy to get stuck the main thread, recommended to use
	# Binding Ouroboros.registerReadFileDescriptor() And Ouroboros.registerWriteFileDescriptor()
	# tornado asynchronous access. Can also be combined with the socket simulate http and platform interaction.

	Ouroboros.createAccountResponse(commitName, realAccountName, datas, Ouroboros.SERVER_SUCCESS)

def onRequestAccountLogin(loginName, password, datas):
	"""
	Ouroboros method.
	Request login account callback
	@param loginName: the client request when the submitted name
	@type loginName: string

	@param password: password
	@type password: string

	@param datas: the client requests the data, the data can be forwarded to third party platform
	@type datas: bytes
	"""
	INFO_MSG('onRequestAccountLogin: registerName=%s' % (loginName))

	commitName = loginName

	# The default account name is submitted in the name of
	realAccountName = commitName

	# Here by http and other means to submit the request to the third party platform, the platform returns data can also be placed into the datas
	# datas will callback to the client
	# If you use http access, because the interfaces are single-threaded, synchronous http access is easy to get stuck the main thread, recommended to use
	# Binding Ouroboros.registerReadFileDescriptor() and Ouroboros.registerWriteFileDescriptor()
	# tornado asynchronous access. Can also be combined with the socket simulate http and platform interaction.

	# If the return code is Ouroboros.SERVER_ERR_LOCAL_PROCESSING indicates verification successful landing, but the dbmgr need to check the account password, Ouroboros.SERVER_SUCCESS you do not need to re-check the password
	Ouroboros.accountLoginResponse(commitName, realAccountName, datas, Ouroboros.SERVER_ERR_LOCAL_PROCESSING)

def onRequestCharge(ordersID, entityDBID, datas):
	"""
	Ouroboros method.
	Request to the accounting callback
	@param ordersID: Order ID
	@type ordersID: uint64

	@param entityDBID: submit the order of the entity DBID
	@type entityDBID: uint64

	@param datas: the client requests the data, the data can be forwarded to third party platform
	@type datas: bytes
	"""
	INFO_MSG('onRequestCharge: entityDBID=%s, entityDBID=%s' % (ordersID, entityDBID))

	# Here by http and other means to submit the request to the third party platform, the platform returns data can also be placed into the datas
	# datas will callback to the baseapp the order of the callback, with particular reference to the API manual charge
	# If you use http access, because the interfaces are single-threaded, synchronous http access is easy to get stuck the main thread, recommended to use
	# Binding Ouroboros.registerReadFileDescriptor() and Ouroboros.registerWriteFileDescriptor()
	# tornado asynchronous access. Can also be combined with the socket simulate http and platform interaction.

	Ouroboros.chargeResponse(ordersID, datas, Ouroboros.SERVER_SUCCESS)
