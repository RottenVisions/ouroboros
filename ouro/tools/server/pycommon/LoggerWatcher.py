# -*- coding: utf-8 -*-

import socket, select
import sys
import os
import struct
import traceback
import select
import getpass
import time

from . import Define

Logger_onAppActiveTick			= 701
Logger_registerLogWatcher		= 702
Logger_deregisterLogWatcher		= 703
Logger_writeLog					= 704

CONSOLE_LOG_MSGID = 65501 # log message


OUROLOG_UNKNOWN			= 0x00000000
OUROLOG_PRINT			= 0x00000001
OUROLOG_ERROR			= 0x00000002
OUROLOG_WARNING			= 0x00000004
OUROLOG_DEBUG			= 0x00000008
OUROLOG_INFO				= 0x00000010
OUROLOG_CRITICAL			= 0x00000020
OUROLOG_SCRIPT_INFO		= 0x00000040
OUROLOG_SCRIPT_ERROR		= 0x00000080
OUROLOG_SCRIPT_DEBUG 	= 0x00000100
OUROLOG_SCRIPT_WARNING	= 0x00000200
OUROLOG_SCRIPT_NORMAL	= 0x00000400

logName2type = {
	"PRINT"		:	OUROLOG_PRINT,
	"ERROR"		:	OUROLOG_ERROR,
	"WARNING"	:	OUROLOG_WARNING,
	"DEBUG"		:	OUROLOG_DEBUG,
	"INFO"		:	OUROLOG_INFO,
	"CRITICAL"	:	OUROLOG_CRITICAL,
	"S_NORM"	:	OUROLOG_SCRIPT_NORMAL,
	"S_INFO"	:	OUROLOG_SCRIPT_INFO,
	"S_ERR"		:	OUROLOG_SCRIPT_ERROR,
	"S_DBG"		:	OUROLOG_SCRIPT_DEBUG,
	"S_WARN"	:	OUROLOG_SCRIPT_WARNING,
}

class LoggerWatcher:
	"""
	Log observer base class
	"""
	def __init__( self ):
		"""
		"""
		self.socket = None
		self.msgBuffer = "".encode()

	def connect( self, ip, port ):
		"""
		Connect to the logger
		"""
		self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.socket.setblocking( True )
		#self.socket.settimeout( 1.0 )
		self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_KEEPALIVE, True)
		self.socket.connect( (ip, port) )
		return self.socket

	def close( self ):
		"""
		"""
		if self.socket is not None:
			self.socket.close()
			self.socket = None

	def registerToLogger( self, uid):
		"""
		Register with the logger
		"""

		msg = Define.BytesIO()
		msg.write( struct.pack("=H", Logger_registerLogWatcher ) ) # command
		msg.write( struct.pack("=H", struct.calcsize("=iIiiccB" + "i" * Define.COMPONENT_END_TYPE + "BB") ) ) # package len	
		msg.write( struct.pack("=i", uid ) )
		msg.write( struct.pack("=I",0xffffffff) ) # logtypes filter
		# msg.write( struct.pack("=I",OUROLOG_WARNING ) ) # logtypes filter
		msg.write( struct.pack("=iicc", 0, 0, '\0'.encode(), '\0'.encode())) # globalOrder, groupOrder, date, keyStr
		msg.write( struct.pack("=B" ,Define.COMPONENT_END_TYPE) ) # component type filter count 
		msg.write( struct.pack("="+"i" * Define.COMPONENT_END_TYPE, *list(range(Define.COMPONENT_END_TYPE)))) # component type filter
		msg.write( struct.pack("=BB", 0, 1 ) ) # isfind, first
		self.socket.sendall( msg.getvalue() )

	def registerToLoggerForWeb( self, uid, components_check, logtype, globalOrder, groupOrder, searchDate, keystr ):
		"""
		Register with the logger
		"""

		msg = Define.BytesIO()
		d1 = str(len(searchDate.encode()))
		d2 = str(len(keystr.encode()))
		msg.write( struct.pack("=H", Logger_registerLogWatcher ) ) # command
		msg.write( struct.pack("=H", struct.calcsize("=iIiiccB" + "i" * Define.COMPONENT_END_TYPE + "BB") + len(searchDate.encode()) + len(keystr.encode())) ) # package len
		msg.write( struct.pack("=i", uid ) )
		msg.write( struct.pack("=I",logtype) ) # logtypes filter
		# msg.write( struct.pack("=I",OUROLOG_WARNING ) ) # logtypes filter
		msg.write( struct.pack("=ii" + d1 + "sc" + d2 + "sc", globalOrder, groupOrder, searchDate.encode() ,'\0'.encode() , keystr.encode(), '\0'.encode() )) # globalOrder, groupOrder, date, keyStr
		msg.write( struct.pack("=B" ,Define.COMPONENT_END_TYPE) ) # component type filter count 
		msg.write( struct.pack("="+"i" * Define.COMPONENT_END_TYPE, *list(list(components_check)))) # component type filter
		msg.write( struct.pack("=BB", 0, 1 ) ) # isfind, first
		self.socket.sendall( msg.getvalue() )

	def deregisterFromLogger( self ):
		"""
		Unregister from logger
		"""
		msg = Define.BytesIO()
		msg.write( struct.pack("=H", Logger_deregisterLogWatcher ) ) # command
		msg.write( struct.pack("=H", 0) ) # package len
		self.socket.sendall( msg.getvalue() )

	def sendActiveTick( self ):
		"""
		Send heartbeat package
		"""
		msg = Define.BytesIO()
		msg.write( struct.pack("=H", Logger_onAppActiveTick ) ) # command
		msg.write( struct.pack("=iQ", Define.WATCHER_TYPE, 0) ) # componentType, componentID
		self.socket.sendall( msg.getvalue() )

	def sendLog( self, uid, type, logStr ):
		"""
		Send a log to the logger
		"""
		if type not in logName2type:
			print( "invalid log type '%s'" % type )
			return
			
		if not isinstance(logStr, bytes):
			logStr = logStr.encode( "utf-8" )
		
		if logStr[-1] != '\n'.encode():
			logStr += '\n'.encode()
				
		logSize = len( logStr )
		
		msg = Define.BytesIO()
		msg.write( struct.pack("=H", Logger_writeLog ) ) # command
		msg.write( struct.pack("=H", struct.calcsize("=iIiQiiqII") + logSize ) ) # package len
		msg.write( struct.pack("=i", uid ) )
		msg.write( struct.pack("=I", logName2type[type]) ) # logtype
		msg.write( struct.pack("=i", Define.WATCHER_TYPE ) )
		msg.write( struct.pack("=Qii", 0, 0, 0) ) # componentID, globalOrder, groupOrder
		msg.write( struct.pack("=qI", int(time.time()), 0) ) # time, ourotime
		msg.write( struct.pack("=I", logSize) ) # log size
		msg.write( logStr )
		
		#print(struct.calcsize("=iIiQiiqII") + logSize, len(msg.getvalue()))
		self.socket.sendall( msg.getvalue() )

	def parseLog( self, stream ):
		"""
		Decompose logs from the data stream
		
		@return: array of bytes
		"""
		self.msgBuffer += stream
		buffLen = len( self.msgBuffer )
		pos = 0
		result = []
		while buffLen >= pos + 4:
			cmdID, dataLen = struct.unpack("=HH", self.msgBuffer[pos:pos + 4])
			pos += 4
			if buffLen < pos + dataLen:
				self.msgBuffer = self.msgBuffer[pos - 4:]
				return result 
			
			if cmdID != CONSOLE_LOG_MSGID:
				print( "Unknown command.(id = %s)" % cmdID )
				pos += dataLen
				continue

			msgLen, = struct.unpack("=I", self.msgBuffer[pos:pos + 4])
			pos += 4 
		
			result.append( self.msgBuffer[pos:pos + msgLen] )
			pos += msgLen
		
		return result

	def receiveLog( self, callbackFunc, loop = False ):
		"""
				BB
		"""
		rl = [ self.socket.fileno() ]
		while True:
			rlist, wlist, xlist = select.select( rl, [], [], 1.0 )
			if rlist:
				msg = self.socket.recv( 4096 )
				if len( msg ) == 0:
					print( "Receive 0 bytes, over! fileno '%s'" % self.socket.fileno() )
					return
			
				ms = self.parseLog( msg )
				if ms:
					callbackFunc( ms )
				continue
			
			if not loop:
				break

			# Regardless, send a heartbeat every once in a while to avoid dropped calls
			self.sendActiveTick()