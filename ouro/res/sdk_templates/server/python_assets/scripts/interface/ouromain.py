# -*- coding: utf-8 -*-
import os
import Ouroboros
from OURODebug import *
from Poller import Poller

"""
The interfaces process mainly handles the access and access work of the Ouroboros server and the third-party platform.
(Note: Since interfaces is a single-threaded server, if you need to use python's http server library, it is recommended to use asynchronous (for example: Tornado), otherwise the main thread of the card will cause blocking. External http requests can use Ouroboros.urlopen asynchronous request.) )
Several features are currently supported:
1: Register an account
	When the client requests to register the account, the request will be forwarded to dbmgr by loginapp. If dbmgr hooks the interfaces, dbmgr will forward the request here (Ouroboros.onRequestCreateAccount)
	At this point, after receiving the request, the script can use various methods to communicate with the third-party platform. You can use the python http library to directly use the socket. When interacting with the third-party platform, you should
	The result of the interaction is returned to the engine baseapp layer, which can be pushed to the baseapp layer via Ouroboros.createAccountResponse.
	
2: Account login
	When the client requests to log in to the account, the request will be forwarded to dbmgr by loginapp. If dbmgr hooks the interfaces, dbmgr will forward the request here (Ouroboros.onRequestAccountLogin)
	At this point, after receiving the request, the script can use various methods to communicate with the third-party platform. You can use the python http library to directly use the socket. When interacting with the third-party platform, you should
	The result of the interaction is returned to the engine baseapp layer, which can be pushed to the baseapp layer via Ouroboros.accountLoginResponse.
	
3: Recharge billing
	When the baseapp requests charging for entity.charge(), the request will be forwarded to dbmgr by loginapp. If dbmgr hooks the interfaces, dbmgr will forward the request here (Ouroboros.onRequestCharge)
	At this point, after receiving the request, the script can use various methods to communicate with the third-party platform. You can use the python http library to directly use the socket. When interacting with the third-party platform, you should
	The result of the interaction is returned to the engine baseapp layer, which can be pushed to the onLoseChargeCB interface by Ouroboros.chargeResponse to push the information to the baseapp layer entity.charge.
	
	Some platforms require the client to request billing directly with the platform. The platform uses the callback server to complete the request. Refer to the â€œplatform callbackâ€?.
	
4: Platform callback
	To accomplish this, you should create a socket in the script layer.
	Attach the socket to Ouroboros (this prevents blocking from causing the main thread card) and then listen on the specified port.
	Use OURO's Ouroboros.registerReadFileDescriptor() and Ouroboros.registerWriteFileDescriptor() to see the API documentation and Poller.py.
"""

g_poller = Poller()

def onInterfaceAppReady():
	"""
	Ouroboros method.
	The interfaces are ready
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
	The callback function before this interface is closed
	"""
	INFO_MSG('onInterfaceAppShutDown()')
	g_poller.stop()

def onRequestCreateAccount(registerName, password, datas):
	"""
	Ouroboros method.
	Request to create an account callback
	@param registerName: the name submitted by the client when requested
	@type  registerName: string
	
	@param password: password
	@type  password: string
	
	@param datas: Data attached to the client request to forward the data to a third-party platform
	@type  datas: bytes
	"""
	INFO_MSG('onRequestCreateAccount: registerName=%s' % (registerName))
	
	commitName = registerName
	
	#Default account name is the name at the time of submission
	realAccountName = commitName 
	
	# Here, the request can be submitted to a third-party platform through http or other means, and the data returned by the platform can also be put into datas.
	# datas will call back to the client
	# If using http access, because interfaces are single-threaded, synchronous http access is easy to get stuck in the main thread, it is recommended to use
		# Ouroboros.registerReadFileDescriptor()å’ŒOuroboros.registerWriteFileDescriptor()ç»“å?ˆ
	# Ouroboros.urlopen("https://www.baidu.com", onHttpCallback) Asynchronous access. It can also interact with the platform in the same way as sockets.
	
	Ouroboros.createAccountResponse(commitName, realAccountName, datas, Ouroboros.SERVER_SUCCESS)
	
def onRequestAccountLogin(loginName, password, datas):
	"""
	Ouroboros method.
	Request login account callback
	@param loginName: The name submitted by the client when requested
	@type  loginName: string
	
	@param password: password
	@type  password: string
	
	@param datas: Data attached to the client request to forward the data to a third-party platform
	@type  datas: bytes
	"""
	INFO_MSG('onRequestAccountLogin: registerName=%s' % (loginName))
	
	commitName = loginName
	
	#Default account name is the name at the time of submission
	realAccountName = commitName 
	
	# Here, the request can be submitted to a third-party platform through http or other means, and the data returned by the platform can also be put into datas.
	# datas will call back to the client
	# If using http access, because interfaces are single-threaded, synchronous http access is easy to get stuck in the main thread, it is recommended to use
		# Ouroboros.registerReadFileDescriptor()å’ŒOuroboros.registerWriteFileDescriptor()ç»“å?ˆ
	# Ouroboros.urlopen("https://www.baidu.com", onHttpCallback) Asynchronous access. It can also interact with the platform in the same way as sockets.
	
	# If the return code is Ouroboros.SERVER_ERR_LOCAL_PROCESSING, the verification login is successful, but dbmgr needs to check the account password, Ouroboros.SERVER_SUCCESS does not need to check the password again.
	Ouroboros.accountLoginResponse(commitName, realAccountName, datas, Ouroboros.SERVER_ERR_LOCAL_PROCESSING)
	
def onRequestCharge(ordersID, entityDBID, datas):
	"""
	Ouroboros method.
	Requesting a chargeback callback
	@param ordersID: The ID of the order
	@type  ordersID: uint64
	
	@param entityDBID: The entity DBID of the submitted order
	@type  entityDBID: uint64
	
	@param datas: Data attached to the client request to forward the data to a third-party platform
	@type  datas: bytes
	"""
	INFO_MSG('onRequestCharge: entityDBID=%s, entityDBID=%s' % (ordersID, entityDBID))
	
	# Here, the request can be submitted to a third-party platform through http or other means, and the data returned by the platform can also be put into datas.
	# datas will be callback to the baseapp order callback, refer to the API manual charge
	# If using http access, because interfaces are single-threaded, synchronous http access is easy to get stuck in the main thread, it is recommended to use
		# Ouroboros.registerReadFileDescriptor()å’ŒOuroboros.registerWriteFileDescriptor()ç»“å?ˆ
	# Ouroboros.urlopen("https://www.baidu.com", onHttpCallback) Asynchronous access. It can also interact with the platform in the same way as sockets.
	
	Ouroboros.chargeResponse(ordersID, datas, Ouroboros.SERVER_SUCCESS)

