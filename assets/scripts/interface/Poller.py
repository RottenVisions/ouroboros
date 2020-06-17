# -*- coding: utf-8 -*-
import Ouroboros
import Functor
import socket
from OURODebug import *

class Poller:
	"""
	Demo:
	Can to ouroboros register a socket, by the engine layer of the Network Module for processing asynchronous notification transceiver.
	Usage:
	from Poller import Poller
	poller = Poller()

	Open(in onBaseappReady execution)
	poller. start("localhost", 12345)

	Stop
	poller. stop()
	"""
	def __init__(self):
		self._socket = None
		self._clients = {}

	def start(self, addr, port):
		"""
		virtual method.
		"""
		self._socket = socket.socket()
		self._socket.bind((addr, port))
		self._socket.listen(10)

		Ouroboros.registerReadFileDescriptor(self._socket.fileno(), self.onRecv)
		# Ouroboros.registerWriteFileDescriptor(self._socket.fileno(), self.onWrite)

	def stop(self):
		if self._socket:
			Ouroboros.deregisterReadFileDescriptor(self._socket.fileno())
			self._socket.close()
			self._socket = None

	def onWrite(self, fileno):
		pass

	def onRecv(self, fileno):
		if self._socket.fileno() == fileno:
			sock, addr = self._socket.accept()
			self._clients[sock.fileno()] = (sock, addr)
			Ouroboros.registerReadFileDescriptor(sock.fileno(), self.onRecv)
			DEBUG_MSG("Poller::onRecv: new channel[%s/%i]" % (addr, sock.fileno()))
		else:
			sock, addr = self._clients.get(fileno, None)
			if sock is None:
				return

			data = sock.recv(2048)
			DEBUG_MSG("Poller::onRecv: %s/%i get data, size=%i" % (addr, sock.fileno(), len(data)))
			self.processData(sock, data)
			Ouroboros.deregisterReadFileDescriptor(sock.fileno())
			sock.close()
			del self._clients[fileno]

	def processData(self, sock, datas):
		"""
		Processing the received data
		"""
		pass
