# -*- coding: utf-8 -*-
import Ouroboros
import random
from OURODebug import *
from Space import Space
import data_entities
import data_spaces

class SpaceDuplicate(Space):
	"""
	This is a copy of a space entity
	"""
	def __init__(self):
		Space.__init__(self)
