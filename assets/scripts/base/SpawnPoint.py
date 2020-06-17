# -*- coding: utf-8 -*-
import Ouroboros
from OURODebug import *
from interfaces.GameObject import GameObject


class SpawnPoint(Ouroboros.Entity, GameObject):
	def __init__(self):
		Ouroboros.Entity.__init__(self)
		GameObject.__init__(self)
		self.createCellEntity(self.createToCell)


