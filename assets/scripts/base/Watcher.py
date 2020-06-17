# -*- coding: utf-8 -*-
import Ouroboros
from OURODebug import *


def countPlayers():
	"""
	Ouroboros.addWatcher("players", "UINT32", countPlayers)
	The above code adds this function to the monitor,
	which can be monitored in real time from tools such as GUIConsole.
	"""
	i = 0
	for e in Ouroboros.entities.values():
		if e.__class__.__name__ == "Avatar":
			i += 1

	return i


def setup():
	Ouroboros.addWatcher("players", "UINT32", countPlayers)