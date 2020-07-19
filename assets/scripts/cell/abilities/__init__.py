# -*- coding: utf-8 -*-
#
"""
"""
from OURODebug import *
import data_abilities

from abilities.AttackAbility import AttackAbility
from abilities.HealAbility import HealAbility

_g_abilities = {}


def onInit():
	"""
	init abilities.
	"""
	for key, data in data_abilities.data.items():
		script = data['script']
		scriptinst = eval(script)()
		_g_abilities[key] = scriptinst
		scriptinst.loadFromDict(data)

def getAbility(abilityID):
	return _g_abilities.get(abilityID)

def getAuraByID(auraID):
	for key, ability in _g_abilities.items():
		if ability.getID() == auraID:
			return ability
	return None
