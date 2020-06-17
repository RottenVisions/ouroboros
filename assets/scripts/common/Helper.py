import Ouroboros
import datetime

def stringToBool(v):
	return v.lower() in ("yes", "true", "t", "1")

def getTime():
	timeObj = datetime.datetime.now()
	return "%s:%s:%s" % (timeObj.hour, timeObj.minute, timeObj.second)

def getTimeDots():
	timeObj = datetime.datetime.now()
	return "%s.%s.%s" % (timeObj.hour, timeObj.minute, timeObj.second)

def getDate():
	timeObj = datetime.datetime.now()
	return "%s/%s/%s" % (timeObj.month, timeObj.day, timeObj.year)

def getDateDots():
	timeObj = datetime.datetime.now()
	return "%s.%s.%s" % (timeObj.month, timeObj.day, timeObj.year)

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