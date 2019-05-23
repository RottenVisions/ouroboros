# -*- coding: utf-8 -*-
import time, weakref, threading

from django.conf import settings

from pycommon import Machines, Define

class MachinesMgr(object):
	"""
	Machine management (buffer)
	"""
	instance = None

	def __init__(self):
		"""
		"""
		print("MachinesMgr::__init__(), USE_MACHINES_BUFFER = %s" % settings.USE_MACHINES_BUFFER)
		if self.instance is not None:
			assert False

		self.instance = weakref.proxy(self)

		self.machineInst = Machines.Machines(0, "WebConsole")
		self.interfaces_groups = {} # { machineID : [ComponentInfo, ...], ...}
		self.machines = []

		# Whether the buffer has been initialized
		# used when settings.USE_MACHINES_BUFFER == True
		self.inited = False

		# Last query time
		# When setting.USE_MACHINES_BUFFER == False, it is used to avoid external code multiple times at all times;
		# When settings.USE_MACHINES_BUFFER == True, it is used by the child thread to determine when to stop the query all interfaces.
		self.lastQueryTime = 0.0

	def __delete__(self):
		print("MachinesMgr::__delete__()")

	def queryAllDatas(self):
		"""
		"""
		hosts = "<broadcast>"
		if isinstance(settings.MACHINES_ADDRESS, (tuple, list)) and settings.MACHINES_ADDRESS:
			hosts = settings.MACHINES_ADDRESS

		self.machineInst.queryAllInterfaces(hosts, 0, settings.MACHINES_QUERY_WAIT_TIME)

		# Although in multithreading, but because it is directly pointer-like assignment
		# So even if it is asynchronous, the value of the main thread will not be confused.
		self.interfaces_groups = self.machineInst.interfaces_groups
		self.machines = self.machineInst.machines
		self.machineInst.reset()

	def startThread(self):
		"""
		Open thread
		"""
		self.thread = threading.Thread(None, self.threadRun, "MachinesDetecter")
		self.thread.start()

	def threadRun(self):
		"""
		Sub-thread timing data acquisition
		"""
		use_buffer = settings.USE_MACHINES_BUFFER
				while use_buffer and settings.USE_MACHINES_BUFFER and time.time() - self.lastQueryTime < settings.STOP_BUFFER_TIME:  # Update??
			self.queryAllDatas()
			self.inited = True

			# Sleep for a period of time after each update
			time.sleep(settings.MACHINES_BUFFER_FLUSH_TIME)

		print("MachinesMgr::threadRun(), over! old %s, new %s, last query: %s" % (use_buffer, settings.USE_MACHINES_BUFFER, time.time() - self.lastQueryTime))
		self.thread = None
		self.inited = False
		self.interfaces_groups = []
		self.machines = []

	def checkAndQueryInterfaces(self):
		"""
		"""
		if settings.USE_MACHINES_BUFFER:
			self.lastQueryTime = time.time()
			if not self.inited:
				self.startThread()
				while not self.inited:
					time.sleep(0.5)
		else:
			if time.time() - self.lastQueryTime >= 1.0:
				self.queryAllDatas()
				self.lastQueryTime = time.time()

	def filterComponentsForUID(self, uid, interfaces_groups):
		"""
		"""
		if uid <= 0:
			return interfaces_groups

		result = {} # { machineID : [ComponentInfo, ...], ...}
		for k, v in interfaces_groups.items():
			result[k] = [e for e in v if e.uid == uid]
		return result

	def queryAllInterfaces(self, uid, user):
		"""
		"""
		self.checkAndQueryInterfaces()
		ig = self.interfaces_groups
		return self.filterComponentsForUID(uid, ig)


	def queryMachines(self):
		"""
		Get all the machines data
		"""
		self.checkAndQueryInterfaces()
		return self.machines

	def hasMachine(self, machineHost):
		"""
		Determine if a specific machine exists
		"""
		self.checkAndQueryInterfaces()

		# First reference and reuse to avoid self.machines reference being modified during this process
		ms = self.machines
		for info in ms:
			if info.intaddr == machineHost:
				return True
		return False

	def makeGUS(self, componentType):
		"""
		Generate a relatively unique gus (non-globally unique)
		"""
		return self.machineInst.makeGUS(componentType)

	def makeCID(self, componentType):
		"""
		Generate a relatively unique cid (non-globally unique)
		"""
		return self.machineInst.makeCID(componentType)

# global variable
machinesmgr = MachinesMgr()
