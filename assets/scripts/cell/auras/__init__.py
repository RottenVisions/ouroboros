# -*- coding: utf-8 -*-
#
"""
"""
from OURODebug import *
import data_auras

# This is needed below for the eval method of script
from auras.HarmfulAura import HarmfulAura
from auras.HelpfulAura import HelpfulAura

_g_auras = {}

def onInit():
	"""
	init auras.
	"""
	for key, data in data_auras.data.items():
		script = data['script']
		scriptinst = eval(script)()
		_g_auras[key] = scriptinst
		scriptinst.loadFromDict(data)

def getAuraByIndex(auraIndex):
	return _g_auras.get(auraIndex)

def getAuraByID(auraID):
	for key, aura in _g_auras.items():
		if aura.getID() == auraID:
			return aura
