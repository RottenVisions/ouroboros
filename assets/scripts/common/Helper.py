import Ouroboros

def getEntity(id):
	for e in Ouroboros.entities.values():
		if(e.id == id):
			return e
	return None

def getPlayerByName(name):
	for player in Ouroboros.entities.values():
		if player.__class__.__name__ == "Avatar":
			if player.getPlayerName() == name:
				return player
	return None

def getAvatarGlobalProperty(id, property):
	return Ouroboros.globalData["avatar_%i.%s" % (id, property)]

def setAvatarGlobalProperty(id, property, value):
	Ouroboros.globalData["avatar_%i.%s" % (id, property)] = value