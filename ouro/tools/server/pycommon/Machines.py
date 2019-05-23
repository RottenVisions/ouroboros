# -*- coding: utf-8 -*-

import socket, time, random
import sys, os, struct
import traceback
import select
import getpass

host = '' # Bind to all interfaces

MachineInterface_onFindInterfaceAddr = 1
MachineInterface_startserver = 2
MachineInterface_stopserver = 3
MachineInterface_onQueryAllInterfaceInfos = 4
MachineInterface_onQueryMachines = 5
MachineInterface_killserver = 6

from . import Define, MessageStream

class ComponentInfo( object ):
	"""
	"""
	def __init__( self, streamStr = None ):
		"""
		"""
		if streamStr:
			self.initFromStream( streamStr )
	
	def initFromStream( self, streamStr ):
		"""
		"""
		self.entities = 0 # Ouroboros.Entity 数�?
		Self.clients = 0 # number of clients
		Self.proxies = 0 # Ouroboros.Proxy instance number
		self.consolePort = 0 # console port
		self.genuuid_sections = 0 # --gus
		
		reader = MessageStream.MessageStreamReader(streamStr)
		
		self.uid = reader.readInt32()
		self.username = reader.readString()
		self.componentType = reader.readInt32()
		self.componentID = reader.readUint64()
		self.componentIDEx = reader.readUint64()
		self.globalOrderID = reader.readInt32()
		self.groupOrderID = reader.readInt32()
		self.genuuid_sections = reader.readInt32()
		self.intaddr = socket.inet_ntoa(reader.read(4))
		self.intport = socket.ntohs(reader.readUint16())
		self.extaddr = socket.inet_ntoa(reader.read(4))
		self.extport = socket.ntohs(reader.readUint16())
		self.extaddrEx = reader.readString()
		self.pid = reader.readUint32()
		self.cpu = reader.readFloat()
		self.mem = reader.readFloat()
		self.usedmem = reader.readUint32()
		self.state = reader.readInt8()
		self.machineID = reader.readUint32()
		self.extradata = reader.readUint64()
		self.extradata1 = reader.readUint64()
		self.extradata2 = reader.readUint64()
		self.extradata3 = reader.readUint64()
		self.backaddr = reader.readUint32()
		self.backport = reader.readUint16()
		
		self.componentName = Define.COMPONENT_NAME[self.componentType]
		self.consolePort = self.extradata3

		if self.componentType in [Define.BASEAPP_TYPE, Define.CELLAPP_TYPE]:
			self.fullname = "%s%s" % (self.componentName, self.groupOrderID)
		else:
			self.fullname = self.componentName

		if self.componentType in [Define.BASEAPP_TYPE, Define.CELLAPP_TYPE]:
			self.entities = self.extradata

		if self.componentType == Define.BASEAPP_TYPE:
			self.clients = self.extradata1

		if self.componentType == Define.BASEAPP_TYPE:
			self.proxies = self.extradata2

		#print("%s, uid=%i, cID=%i, gid=%i, groupid=%i, uname=%s" % (Define.COMPONENT_NAME[self.componentType], \
		#	self.uid, self.componentID, self.globalOrderID, self.groupOrderID, self.username))
		

class Machines:
	def __init__(self, uid = None, username = None, listenPort = 0):
		"""
		"""
		self.udp_socket = None
		self.listenPort = listenPort
		
		if uid is None:
			uid = Define.getDefaultUID()
		
		if username is None:
			try:
				username = Define.pwd.getpwuid( uid ).pw_name
			except:
				import getpass
				username = getpass.getuser()

		self.uid = uid
		self.username = username
		if type(self.username) is str:
			self.username = username.encode( "utf-8" )
		else:
			try:
				if type(self.username) is unicode:
					self.username = username.encode( "utf-8" )
			except:
				pass
		
		self.startListen()
		
		self.reset()
		
	def __del__(self):
		#print( "Machines::__del__(), Machines destroy now" )
		self.stopListen()
		
	def startListen(self):
		"""
		"""
		assert self.udp_socket is None
		self.udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
		self.udp_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
		self.udp_socket.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, 5 * 1024 * 1024)
		self.udp_socket.bind((host, self.listenPort))
		self.replyPort = self.udp_socket.getsockname()[1]
		#print( "udp receive addr: %s" % (self.udp_socket.getsockname(), ) )
		
	def stopListen(self):
		"""
		"""
		if self.udp_socket is not None:
			self.udp_socket.close()
			self.udp_socket = None
		
	def reset(self):
		"""
		"""
		self.interfaces = {}            # { componentType : [ComponentInfo, ...], ... }
		self.interfaces_groups = {}     # { machineID : [ComponentInfo, ...], ...}
		self.interfaces_groups_uid = {} # { machineID : [uid, ...], ...}
		self.machines = []
		
	def send(self, msg, ip = "<broadcast>"):
		"""
				Send a message
		"""
		_udp_broadcast_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
		_udp_broadcast_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

		if isinstance(ip, (tuple, list)):
			for addr in ip:
				_udp_broadcast_socket.sendto(msg, (addr, 20086))
		elif ip == "<broadcast>":
			_udp_broadcast_socket.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
			_udp_broadcast_socket.sendto(msg, ('255.255.255.255', 20086))
		else:
			_udp_broadcast_socket.sendto(msg, (ip, 20086))
		
	def sendAndReceive(self, msg, ip = "<broadcast>", trycount = 0, timeout = 1, callback = None):
		"""
		Send a message and wait for the message to return
		"""
		self.send(msg, ip)
		
		self.udp_socket.settimeout(timeout)
		dectrycount = trycount
		
		recvDatas = []
		while True:
			try:
				datas, address = self.udp_socket.recvfrom(10240)
				recvDatas.append(datas)
				#print ("Machine::sendAndReceive(), %s received %s data from %r" % (len(recvDatas), len(datas), address))
				if callable( callback ):
					try:
						if callback( datas, address ):
							return recvDatas
					except:
						traceback.print_exc()
			except socket.timeout: 
				if dectrycount <= 0:
					break
				dectrycount -= 1
				#print("Machine::sendAndReceive(), try count %s" % (trycount - dectrycount))
			except (KeyboardInterrupt, SystemExit):
				raise
			except:
				traceback.print_exc()
				break
		return recvDatas

	def receiveReply(self, timeout = 1):
		"""
		Waiting for message to return
		"""
		self.udp_socket.settimeout(timeout)
		
		try:
			datas, address = self.udp_socket.recvfrom(10240)
			return datas, address
		except socket.timeout:
			return "", ""

	def queryAllInterfaces(self, ip = "<broadcast>", trycount = 1, timeout = 1):
		"""
		"""
		self.reset()
		nameLen = len( self.username ) + 1 # Add 1 to store empty terminators
		
		msg = MessageStream.MessageStreamWriter(MachineInterface_onQueryAllInterfaceInfos)
		msg.writeInt32(self.uid)
		msg.writeString(self.username)
		msg.writeUint16(socket.htons(self.replyPort)) # reply port

		datas = self.sendAndReceive( msg.build(), ip, trycount, timeout )
		self.parseQueryDatas( datas )

	def queryMachines(self, ip = "<broadcast>", trycount = 1, timeout = 1):
		"""
		"""
		self.reset()
		nameLen = len( self.username ) + 1 # Add 1 to generate a null terminator
		
		msg = MessageStream.MessageStreamWriter(MachineInterface_onQueryMachines)
		msg.writeInt32(self.uid)
		msg.writeString(self.username)
		msg.writeUint16(socket.htons(self.replyPort)) # reply port

		datas = self.sendAndReceive( msg.build(), ip, trycount, timeout )
		self.parseQueryDatas( datas )

	def startServer(self, componentType, cid, gus, targetIP, ouro_root, ouro_res_path, ouro_bin_path, trycount = 1, timeout = 1):
		"""
		"""
		msg = MessageStream.MessageStreamWriter(MachineInterface_startserver)
		msg.writeInt32(self.uid)
		msg.writeInt32(componentType)
		msg.writeUint64(cid)
		msg.writeUint16(gus)
		msg.writeUint16(socket.htons(self.replyPort)) # reply port
		msg.writeString(ouro_root)
		msg.writeString(ouro_res_path)
		msg.writeString(ouro_bin_path)

		if trycount <= 0:
			self.send( msg.build(), targetIP )
			self.receiveReply()
		else:
			self.sendAndReceive( msg.build(), targetIP, trycount, timeout )

	def stopServer(self, componentType, componentID = 0, targetIP = "<broadcast>", trycount = 1, timeout = 1):
		"""
		"""
		msg = MessageStream.MessageStreamWriter(MachineInterface_stopserver)
		msg.writeInt32(self.uid)
		msg.writeInt32(componentType)
		msg.writeUint64(componentID)
		msg.writeUint16(socket.htons(self.replyPort)) # reply port

		if trycount <= 0:
			self.send( msg.build(), targetIP )
			self.receiveReply()
		else:
			self.sendAndReceive( msg.build(), targetIP, trycount, timeout )

	def killServer(self, componentType, componentID = 0, targetIP = "<broadcast>", trycount = 1, timeout = 1):
		"""
		"""
		msg = MessageStream.MessageStreamWriter(MachineInterface_killserver)
		msg.writeInt32(self.uid)
		msg.writeInt32(componentType)
		msg.writeUint64(componentID)
		msg.writeUint16(socket.htons(self.replyPort)) # reply port

		if trycount <= 0:
			self.send( msg.build(), targetIP )
			self.receiveReply()
		else:
			self.sendAndReceive( msg.build(), targetIP, trycount, timeout )

	def parseQueryDatas( self, recvDatas ):
		"""
		"""
		for data in recvDatas:
			self.parseQueryData( data )

	def parseQueryData( self, recvData ):
		"""
		"""
		cinfo = ComponentInfo( recvData )

		componentInfos = self.interfaces.get(cinfo.componentType)
		if componentInfos is None:
			componentInfos = []
			self.interfaces[cinfo.componentType] = componentInfos
		
		found = False
		for info in componentInfos:
			if info.componentID == cinfo.componentID and info.pid == cinfo.pid:
				found = True
				break
		
		if found:
			return
			
		componentInfos.append(cinfo)
		
		machineID = cinfo.machineID
		
		gourps = self.interfaces_groups.get(machineID, [])
		if machineID not in self.interfaces_groups:
			self.interfaces_groups[machineID] = gourps
			self.interfaces_groups_uid[machineID] = []
			
		# If pid is equal to machineID, this is the machine process.
		if cinfo.pid != machineID:
			gourps.append(cinfo)
			if cinfo.uid not in self.interfaces_groups_uid[machineID]:
				self.interfaces_groups_uid[machineID].append(cinfo.uid)
		else:
			#是 machineprocess, put it first, and add it to the machines list
			gourps.insert(0, cinfo)
			self.machines.append( cinfo )

	def makeGUS(self, componentType):
		"""
		Generate a relatively unique gus (non-globally unique)
		"""
		if not hasattr( self, "ct2gus" ):
			self.ct2gus = [0] * Define.COMPONENT_END_TYPE
		
		self.ct2gus[componentType] += 1
		return componentType * 100 + self.ct2gus[componentType]
	
	def makeCID(self, componentType):
		"""
		Generate a relatively unique cid (non-globally unique)
		"""
		if not hasattr( self, "cidRand" ):
			self.cidRand = random.randint(1, 99999)

		if not hasattr( self, "ct2cid" ):
			self.ct2cid = [0] * Define.COMPONENT_END_TYPE

		self.ct2cid[componentType] += 1
		t = int( time.time() ) % 99999
		cid = "%02i%05i%05i%04i" % (componentType, t, self.cidRand, self.ct2cid[componentType])
		return int(cid)
	
	def getMachine( self, ip ):
		"""
		Find the corresponding machine info by ip address
		"""
		for info in self.machines:
			if info.intaddr == ip:
				return info
		return None
	
	def hasMachine( self, ip ):
		"""
		"""
		for info in self.machines:
			if info.intaddr == ip:
				return True
		return False
	
	def getComponentInfos( self, componentType ):
		"""
		Get a certain type of component information
		"""
		return self.interfaces.get( componentType, [] )

