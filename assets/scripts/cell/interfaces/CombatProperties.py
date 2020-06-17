# -*- coding: utf-8 -*-
import Ouroboros
import GlobalDefine
from OURODebug import *


class CombatProperties:
	"""
	All about combat attributes
	If you are perfect, you can directly generate this module according to the planning excel table.
	"""

	def __init__(self):
		# self.HP_Max = 100
		# self.EG_Max = 100

		# Non-death status needs to be filled
		if not self.isState(GlobalDefine.ENTITY_STATE_DEAD) and self.HP == 0 and self.EG == 0:
			self.fullPower()

	def fullPower(self):
		"""
		"""
		self.setHP(self.HP_Max)
		self.setEG(self.EG_Max)

	def addHP(self, val):
		"""
		defined.
		"""
		v = self.HP + int(val)
		if v < 0:
			v = 0
		if v > self.HP_Max:
			v = self.HP_Max

		if self.HP == v:
			return

		self.HP = v

	def addEG(self, val):
		"""
		defined.
		"""
		v = self.EG + int(val)
		if v < 0:
			v = 0
		if v > self.EG_Max:
			v = self.EG_Max

		if self.EG == v:
			return

		self.EG = v

	def addDefence(self, val):
		v = self.defence + int(val)
		if v < 0:
			v = 0

		if self.defence == v:
			return
		self.defence = v

	def addAttack_Max(self, val):
		v = self.attack_Max + int(val)
		if v < 0:
			v = 0

		if self.attack_Max == v:
			return
		self.attack_Max = v

	def addAttack_Min(self, val):
		v = self.attack_Min + int(val)
		if v < 0:
			v = 0

		if self.attack_Min == v:
			return
		self.attack_Min = v

	def setHP(self, hp):
		"""
		defined
		"""
		hp = int(hp)
		if hp < 0:
			hp = 0
		if hp > self.HP_Max:
			hp = self.HP_Max

		if self.HP == hp:
			return

		self.HP = hp

	def setEG(self, eg):
		"""
		defined
		"""
		eg = int(eg)
		if eg < 0:
			eg = 0
		if eg > self.EG_Max:
			eg = self.EG_Max
		if self.EG == eg:
			return

		self.EG = eg

	def setHPMax(self, hpMax):
		"""
		defined
		"""
		hpmax = int(hpMax)
		self.HP_Max = hpmax

	def setEGMax(self, egMax):
		"""
		defined
		"""
		egMax = int(egMax)
		self.EG_Max = egMax


