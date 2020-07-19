import datetime
import inspect, re

def varname(p):
  for line in inspect.getframeinfo(inspect.currentframe().f_back)[3]:
    m = re.search(r'\bvarname\s*\(\s*([A-Za-z_][A-Za-z0-9_]*)\s*\)', line)
    if m:
      return m.group(1)

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

def printDict(dict):
	for x in dict:
		print(x)
		for y in dict[x]:
			print(y, ':', dict[x][y])

def stringToBool(v):
	return v.lower() in ("yes", "true", "t", "1")

def classCopy(c,name=None):
    if not name: name = 'CopyOf'+c.__name__
    if hasattr(c,'__slots__'):
        slots = c.__slots__ if type(c.__slots__) != str else (c.__slots__,)
        dict_ = dict()
        sloted_members = dict()
        for k,v in c.__dict__.items():
            if k not in slots:
                dict_[k] = v
            elif type(v) != types.MemberDescriptorType:
                sloted_members[k] = v
        CopyOfc = type(name, c.__bases__, dict_)
        for k,v in sloted_members.items():
            setattr(CopyOfc,k,v)
        return CopyOfc
    else:
        dict_ = dict(c.__dict__)
        return type(name, c.__bases__, dict_)