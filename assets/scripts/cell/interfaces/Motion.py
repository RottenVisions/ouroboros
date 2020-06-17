# -*- coding: utf-8 -*-
import Ouroboros
import math
import Math
import time
import random
from OURODebug import *
import GlobalDefine

import data_entities

class Motion:
	"""
	Movement related package
	"""

	def __init__(self):
		self.nextMoveTime = int(time.time() + random.randint(5, 15))
		# Default walk speed (declared in .def file)
		self.moveSpeed = self.getDatas()['moveSpeed']
		entityData = data_entities.data.get(self.uid)
		# Entity walk speed
		if entityData != None:
			self.moveSpeed = entityData['moveSpeed']

	def stopMotion(self):
		"""
		Stop moving
		"""
		if self.isMoving:
			# INFO_MSG("%i stop motion." % self.id)
			self.cancelController("Movement")
			self.isMoving = False
			self.onMotionStateChanged(GlobalDefine.ENTITY_MOTION_STATE_STATIONARY)

	def randomWalk(self, basePos):
		"""
		Randomly move entity
		"""
		if self.isMoving:
			return False

		if time.time() < self.nextMoveTime:
			return False

		while True:
			# The moving radius is within 30 meters
			if self.canNavigate():
				destPos = self.getRandomPoints(basePos, 30.0, 1, 0)

				if len(destPos) == 0:
					self.nextMoveTime = int(time.time() + random.randint(5, 15))
					return False

				destPos = destPos[0]
			else:
				rnd = random.random()
				a = 30.0 * rnd  # The moving radius is within 30 meters
				b = 360.0 * rnd  # Random angle
				x = a * math.cos(b)  # Radius*
				z = a * math.sin(b)

				destPos = (basePos.x + x, basePos.y, basePos.z + z)

			if self.position.distTo(destPos) < 2.0:
				continue

			self.gotoPosition(destPos)
			self.isMoving = True
			self.onMotionStateChanged(GlobalDefine.ENTITY_MOTION_STATE_WALK)
			self.nextMoveTime = int(time.time() + random.randint(5, 15))
			break

		return True

	def resetSpeed(self):
		entityData = data_entities.data.get(self.uid)
		# Entity walk speed
		if entityData != None:
			walkSpeed = entityData['moveSpeed']
		# Default walk speed (declared in .def file)
		walkSpeed = self.getDatas()["moveSpeed"]
		if walkSpeed != self.moveSpeed:
			self.moveSpeed = walkSpeed

	def backSpawnPos(self):
		"""
		virtual method.
		"""
		INFO_MSG("%s::backSpawnPos: %i, pos=%s, speed=%f." % \
				 (self.getScriptName(), self.id, self.spawnPos, self.moveSpeed))

		self.resetSpeed()
		self.onMotionStateChanged(GlobalDefine.ENTITY_MOTION_STATE_RUN)
		self.gotoPosition(self.spawnPos)

	def gotoEntity(self, targetID, dist=0.0):
		"""
		virtual method.
		Move to the entity location
		"""
		if self.isMoving:
			self.stopMotion()

		entity = Ouroboros.entities.get(targetID)
		if entity is None:
			DEBUG_MSG("%s::gotoEntity: not found targetID=%i" % (targetID))
			return

		if entity.position.distTo(self.position) <= dist:
			return

		self.isMoving = True
		self.onMotionStateChanged(GlobalDefine.ENTITY_MOTION_STATE_WALK)
		self.moveToEntity(targetID, self.moveSpeed, dist, None, True, False)

	def gotoPosition(self, position, dist=0.0):
		"""
		virtual method.
		Move to location
		"""
		if self.isMoving:
			self.stopMotion()

		if self.position.distTo(position) <= 0.05:
			return

		self.isMoving = True
		self.onMotionStateChanged(GlobalDefine.ENTITY_MOTION_STATE_WALK)
		speed = self.moveSpeed
		DEBUG_MSG("speed %f %f" % (speed, self.moveSpeed))

		if self.canNavigate():
			self.navigate(Math.Vector3(position), speed, dist, speed, 512.0, 1, 0, None)
		else:
			if dist > 0.0:
				destPos = Math.Vector3(position) - self.position
				destPos.normalise()
				destPos *= dist
				destPos = position - destPos
			else:
				destPos = Math.Vector3(position)

			self.moveToPoint(destPos, speed, 0, None, 1, False)

	def getStopPoint(self, yaw=None, rayLength=100.0):
		"""
		"""
		if yaw is None: yaw = self.yaw
		yaw = (yaw / 2);
		vv = Math.Vector3(math.sin(yaw), 0, math.cos(yaw))
		vv.normalise()
		vv *= rayLength

		lastPos = self.position + vv;

		pos = Ouroboros.raycast(self.spaceID, self.layer, self.position, vv)
		if pos == None:
			pos = lastPos

		return pos

	# --------------------------------------------------------------------------------------------
	#                              Callbacks
	# --------------------------------------------------------------------------------------------
	def onMove(self, controllerId, userarg):
		"""
		Ouroboros method.
		Use any of the engine's mobile-related interfaces to call this interface once the entity has completed a move
		"""
		# DEBUG_MSG("%s::onMove: %i controllerId =%i, userarg=%s" % \
		#				(self.getScriptName(), self.id, controllerId, userarg))
		self.isMoving = True

	def onMoveFailure(self, controllerId, userarg):
		"""
		Ouroboros method.
		Use any of the engine's mobile-related interfaces to call this interface once the entity has completed a move
		"""
		DEBUG_MSG("%s::onMoveFailure: %i controllerId =%i, userarg=%s" % \
				  (self.getScriptName(), self.id, controllerId, userarg))

		self.isMoving = False
		self.onMotionStateChanged(GlobalDefine.ENTITY_MOTION_STATE_STATIONARY)

	def onMoveOver(self, controllerId, userarg):
		"""
		Ouroboros method.
		Use any of the engine's mobile-related interfaces to call this interface at the end of the entity move
		"""
		self.isMoving = False
		self.onMotionStateChanged(GlobalDefine.ENTITY_MOTION_STATE_STATIONARY)

	# --------------------------------------------------------------------------------------------
	#                              Callbacks
	# --------------------------------------------------------------------------------------------
	def onMotionStateChanged(self, toState):
		self.motionState = toState
		self.onCalculateAnimationMove(toState)

	def onCalculateAnimationMove(self, motionState):
		if motionState == GlobalDefine.ENTITY_MOTION_STATE_STATIONARY:
			self.currentState = GlobalDefine.ENTITY_ANIMATION_STATE_IDLE
		if motionState == GlobalDefine.ENTITY_MOTION_STATE_WALK:
			self.currentState = GlobalDefine.ENTITY_ANIMATION_STATE_WALK
		if motionState == GlobalDefine.ENTITY_MOTION_STATE_RUN:
			self.currentState = GlobalDefine.ENTITY_ANIMATION_STATE_RUN

