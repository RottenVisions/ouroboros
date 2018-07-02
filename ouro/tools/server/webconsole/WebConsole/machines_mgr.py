# -*- coding: utf-8 -*-
import time, weakref, threading

from django.conf import settings

from pycommon import Machines, Define

class MachinesMgr(object):
	"""
	machines management（buffer），
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

		# Whether it has been initialized through the buffer area
		# For when settings.USE_MACHINES_BUFFER == True 时
		self.inited = False

		# The last query time
		# 在settings.USE_MACHINES_BUFFER == False, used to avoid external code in the same time multiple query all interfaces；
		# 在settings.USE_MACHINES_BUFFER == True, for the sub-thread is determined to stop the query all interfaces的时机。
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

		# Although in a multi-threaded, but because it is similar to the pointer is directly assigned the value
		# So even if asynchronous, the main thread takes the value does not exist confusion problems
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
		while use_buffer and settings.USE_MACHINES_BUFFER and time.time() - self.lastQueryTime < settings.STOP_BUFFER_TIME:  # 动态更新需要
			self.queryAllDatas()
			self.inited = True

			# Once every update after sleep a period of time
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
		To determine whether there is a specific machine(machine)
		"""
		self.checkAndQueryInterfaces()

		# The first reference to use, in order to avoid this process of the self. machines reference is modified
		ms = self.machines
		for info in ms:
			if info.intaddr == machineHost:
				return True
		return False

	def makeGUS(self, componentType):
		"""
		Generates a relatively unique gus-global only
		"""
		return self.machineInst.makeGUS(componentType)

	def makeCID(self, componentType):
		"""
		Generates a relatively unique cid non-global only
		"""
		return self.machineInst.makeCID(componentType)

# Global variables
machinesmgr = MachinesMgr()
